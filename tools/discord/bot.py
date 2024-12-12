import asyncio
import discord
from dotenv import load_dotenv
import os
from typing import Optional

# Global variable management
load_dotenv()
TOKEN = os.getenv("token")
DOLPHIN_PATH = os.getenv("dolphin_path")
SP_PATH = os.getenv("sp_path")
NAND_PATH = os.getenv("nand_path")
IS_GENERATING_KRKG = False

client = discord.Client(intents=discord.Intents.default())
tree = discord.app_commands.CommandTree(client)

# ================================
#     DOLPHIN
# ================================


async def dolphin_clear_io():
    rkg_path = os.path.join(NAND_PATH, "test.rkg")
    krkg_path = os.path.join(NAND_PATH, "test.krkg")
    ok_path = os.path.join(NAND_PATH, "ok")
    fail_path = os.path.join(NAND_PATH, "fail")

    if os.path.exists(rkg_path):
        os.remove(rkg_path)

    if os.path.exists(krkg_path):
        os.remove(krkg_path)

    if os.path.exists(ok_path):
        os.remove(ok_path)

    if os.path.exists(fail_path):
        os.remove(fail_path)


async def dolphin_set_ghost(ghost: bytes):
    with open(os.path.join(NAND_PATH, "test.rkg"), "wb") as f:
        f.write(ghost)


async def dolphin_run():
    uncap_speed = ["-C", "Dolphin.Core.EmulationSpeed=0"]
    null_renderer = ["-v", "Null"]
    silent_run = ["-C", "Dolphin.Display.RenderToMain=true"]
    no_audio = ["-C", 'Dolphin.DSP.Backend="No Audio Output"']

    proc = await asyncio.create_subprocess_exec(
        DOLPHIN_PATH,
        "-e",
        SP_PATH,
        "-b",
        *uncap_speed,
        *null_renderer,
        *silent_run,
        *no_audio,
    )
    await proc.communicate()


async def dolphin_ok() -> bool:
    return os.path.exists(os.path.join(NAND_PATH, "ok"))


async def dolphin_fail() -> Optional[str]:
    fail_path = os.path.join(NAND_PATH, "fail")
    if os.path.exists(fail_path):
        with open(fail_path, "r") as f:
            return f.read()
    else:
        return None


async def dolphin_krkg() -> Optional[bytes]:
    krkg_path = os.path.join(NAND_PATH, "test.krkg")
    if os.path.exists(krkg_path):
        with open(krkg_path, "rb") as f:
            return f.read()
    else:
        return None


async def dolphin_generate_krkg(ghost: bytes, interaction: discord.Interaction):
    await dolphin_clear_io()
    await dolphin_set_ghost(ghost)

    await dolphin_run()

    if await dolphin_ok():
        krkg = await dolphin_krkg()
        if not krkg:
            await respond_bug_error(
                interaction, "Dolphin returned OK, but there's no KRKG"
            )
            return

        await respond_krkg_success(
            interaction,
            discord.File(os.path.join(NAND_PATH, "test.krkg"), filename="test.krkg"),
        )
        return

    fail = await dolphin_fail()
    if fail:
        await respond_fail_error(interaction, fail)
        return
    # Empty string is falsey, leading to two situations where fail is False
    elif os.path.exists(os.path.join(NAND_PATH, "fail")):
        await respond_bug_error(
            interaction, "Dolphin returned fail, but there's no explanation"
        )
        return

    await respond_bug_error(interaction, "Dolphin returned nothing")


# ================================
#     RESPONSES
# ================================


async def respond_generic_error(
    interaction: discord.Interaction, error: str, tip: Optional[str] = None
):
    embed = discord.Embed(color=0xFF595E, title="Error!", description=f"### {error}")
    embed.set_footer(text=tip)
    if (
        interaction.response.type
        == discord.InteractionResponseType.deferred_channel_message
    ):
        await interaction.followup.send(embed=embed, ephemeral=True)
    else:
        await interaction.response.send_message(embed=embed, ephemeral=True)


async def respond_fail_error(
    interaction: discord.Interaction, error: str, tip: Optional[str] = None
):
    embed = discord.Embed(color=0xFF595E, title="Fail!", description=f"### {error}")
    embed.set_footer(text=tip)

    if (
        interaction.response.type
        == discord.InteractionResponseType.deferred_channel_message
    ):
        await interaction.followup.send(embed=embed, ephemeral=True)
    else:
        await interaction.response.send_message(embed=embed, ephemeral=True)


async def respond_bug_error(interaction: discord.Interaction, error: str):
    embed = discord.Embed(color=0xFFCA3A, title="BUG!", description=f"### {error}")
    embed.set_footer(
        text="This should never be visible. "
        "Please report this to a Kinoko developer if you see this!"
    )

    if (
        interaction.response.type
        == discord.InteractionResponseType.deferred_channel_message
    ):
        await interaction.followup.send(embed=embed, ephemeral=True)
    else:
        await interaction.response.send_message(embed=embed, ephemeral=True)


async def respond_krkg_success(interaction: discord.Interaction, file: discord.File):
    if (
        interaction.response.type
        == discord.InteractionResponseType.deferred_channel_message
    ):
        await interaction.followup.send(file=file, ephemeral=True)
    else:
        await interaction.response.send_message(file=file, ephemeral=True)


# ================================
#     EVENTS
# ================================


@client.event
async def on_ready():
    await tree.sync()
    print(f"Synced command tree and logged in as {client.user}")


# ================================
#     COMMANDS
# ================================


@tree.command(name="ping")
async def command_ping(interaction: discord.Interaction):
    await interaction.response.send_message("Pong!", ephemeral=True)


@tree.command(name="generate_krkg")
async def command_generate_krkg(
    interaction: discord.Interaction, ghost: discord.Attachment
):
    global IS_GENERATING_KRKG

    # Log when people generate KRKGs
    print(f"{interaction.user.name} is requesting to generate a KRKG!")

    # Check if the file claims to be a ghost
    file_extension = ghost.filename.split(".")[-1]
    if file_extension != "rkg":
        await respond_generic_error(
            interaction,
            "File is not an RKG",
            "Double check that the file extension is .rkg!",
        )
        return

    # Check if the size is too small or big to be a ghost
    if ghost.size < 0x8C or ghost.size > 0x2800:
        await respond_generic_error(
            interaction,
            f"File is too {"small" if ghost.size < 0x8c else "big"} to be an RKG",
        )
        return

    # We can only generate one KRKG at a time, due to how we I/O with Dolphin
    # This theoretically creates a race condition, but with low traffic in mind,
    # the chances of a collision are incredibly unlikely
    # TODO: This command should use a mutex to add to a queue that another thread reads from
    if IS_GENERATING_KRKG:
        await respond_generic_error(
            interaction,
            "Already generating a KRKG",
            "I can only handle one ghost at a time. Try again later!",
        )
        return

    IS_GENERATING_KRKG = True
    await interaction.response.defer(ephemeral=True, thinking=True)
    await dolphin_generate_krkg(await ghost.read(), interaction)
    IS_GENERATING_KRKG = False


if __name__ == "__main__":
    assert TOKEN, 'Missing token for Discord bot, ensure "token" exists in environment'
    assert (
        DOLPHIN_PATH
    ), 'Missing path for Dolphin, ensure "dolphin_path" exists in environment'
    assert SP_PATH, 'Missing path for SP DOL, ensure "sp_path" exists in environment'
    assert (
        NAND_PATH
    ), 'Missing path for save data, ensure "nand_path" exists in environment'

    client.run(TOKEN)
