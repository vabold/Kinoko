import csv
import binascii
from dataclasses import dataclass


# fmt: off
class FaceButtonInput:
    def __init__(self, AButton: int, BButton: int, ItemButton: int, csv_line: int):
        assert AButton == 0 or AButton == 1, \
            f"A button input has value `{AButton}` at line {csv_line}"

        assert BButton == 0 or BButton == 1, \
            f"B button input has value `{BButton}` at line {csv_line}"

        assert ItemButton == 0 or ItemButton == 1, \
            f"Item button input has value `{ItemButton}` at line {csv_line}"

        self.AButton = AButton
        self.BButton = BButton
        self.ItemButton = ItemButton

    def __eq__(self, other):
        return (
            self.AButton == other.AButton
            and self.BButton == other.BButton
            and self.ItemButton == other.ItemButton
        )


class DirectionInput:
    def __init__(self, YDirection: int, XDirection: int, csv_line: int):
        assert -7 <= YDirection <= 7, \
            f"Y direction input has value `{YDirection}` at line {csv_line}"

        assert -7 <= XDirection <= 7, \
            f"X direction input has value `{XDirection}` at line {csv_line}"

        self.YDirection = YDirection + 7
        self.XDirection = XDirection + 7

    def __eq__(self, other):
        return (
            self.YDirection == other.YDirection and self.XDirection == other.XDirection
        )


class TrickInput:
    def __init__(self, TrickInput: int, csv_line: int):
        assert 0 <= TrickInput <= 4, \
            f"Trick input has value `{TrickInput}` at line {csv_line}"

        self.TrickInput = TrickInput

    def __eq__(self, other):
        return self.TrickInput == other.TrickInput

# fmt: on


def read_csv_file(file_name: str):
    inputs = []
    with open(file_name, "r") as f:
        reader = csv.reader(f)
        for line in reader:
            if len(line) < 6:
                raise Exception(f"Malformed line at line {reader.line_num}: `{line}`")
            inputs.append(
                [
                    FaceButtonInput(
                        int(line[0]), int(line[1]), int(line[2]), reader.line_num
                    ),
                    DirectionInput(int(line[4]), int(line[3]), reader.line_num),
                    TrickInput(int(line[5]), reader.line_num),
                ]
            )
    return inputs


@dataclass
class MetaData:
    TrackId: int
    Character: int
    Vehicle: int
    ManualDrift: bool


def get_dummy_data():
    """Generate dummy metadata to put in the header of the rkg file"""
    return MetaData(0, 0, 0, True)


def write_rkg(inputs: list, metadata: MetaData):
    """Writes an uncompressed RKG file with a minimal header
    More information on the RKG format: https://wiki.tockdom.com/wiki/RKG_(File_Format)
    """

    # The header of the rkg file
    rkg_header = [0x00] * 0x88
    # header magic
    rkg_header[0:4] = [ord("R"), ord("K"), ord("G"), ord("D")]
    # TrackId
    rkg_header[0x7] = metadata.TrackId << 0x2
    # Vehicle
    rkg_header[0x8] = metadata.Vehicle << 0x2
    # Character
    rkg_header[0x8] = rkg_header[0x8] & (metadata.Character & 0x30)
    rkg_header[0x9] = metadata.Character << 0x4
    # Drift
    drift_type = 0 if metadata.ManualDrift else 1
    rkg_header[0xD] = rkg_header[0xD] | drift_type << 2

    # The input section
    # Lists containing tuples of the form [input, frames held]
    face_buttons = [[inputs[0][0], 1]]
    direction_inputs = [[inputs[0][1], 1]]
    trick_inputs = [[inputs[0][2], 1]]

    for input in inputs[1:]:
        # Face buttons
        if input[0] != face_buttons[-1][0] or face_buttons[-1][1] == 255:
            face_buttons.append([input[0], 1])
        else:
            face_buttons[-1][1] += 1

        # Direction inputs
        if input[1] != direction_inputs[-1][0] or direction_inputs[-1][1] == 255:
            direction_inputs.append([input[1], 1])
        else:
            direction_inputs[-1][1] += 1

        # Trick inputs
        if input[2] != trick_inputs[-1][0] or trick_inputs[-1][1] == 4096:
            trick_inputs.append([input[2], 1])
        else:
            trick_inputs[-1][1] += 1

    # The header of the input data
    input_header = [0x00] * 0x8
    # Face button count
    input_header[0x0] = len(face_buttons) >> 0x8
    input_header[0x1] = len(face_buttons) & 0xFF
    # Direction input count
    input_header[0x2] = len(direction_inputs) >> 0x8
    input_header[0x3] = len(direction_inputs) & 0xFF
    # Direction input count
    input_header[0x4] = len(trick_inputs) >> 0x8
    input_header[0x5] = len(trick_inputs) & 0xFF

    # Input section
    inputs = []
    for face_button_input in face_buttons:
        new_input = face_button_input[0].AButton
        new_input |= face_button_input[0].BButton << 0x1
        new_input |= face_button_input[0].ItemButton << 0x2
        inputs.extend([new_input, face_button_input[1]])

    for direction_input in direction_inputs:
        new_input = direction_input[0].YDirection
        new_input |= direction_input[0].XDirection << 0x4
        inputs.extend([new_input, direction_input[1]])

    for trick_input in trick_inputs:
        new_input = (trick_input[1] - 255) >> 0x8 if trick_input[1] > 255 else 0
        new_input |= trick_input[0].TrickInput << 0x4
        inputs.extend([new_input, trick_input[1] if trick_input[1] <= 255 else 255])

    if len(inputs) > 0x2774:
        print("Too many ghost inputs for a standard RKG file")
    else:
        # Pad input data until it is of length 0x2774
        inputs.extend([0x00] * (0x2774 - len(inputs)))

    # Calculate CRC32 of input data
    crc32 = binascii.crc32(bytes(inputs))
    inputs.extend(list(crc32.to_bytes(4, "big")))

    # Write to file
    with open("out.rkg", "wb") as f:
        f.write(bytes(rkg_header))
        f.write(bytes(input_header))
        f.write(bytes(inputs))


if __name__ == "__main__":
    inputs = read_csv_file("MKW_Player_Inputs.csv")
    write_rkg(inputs, get_dummy_data())
