#pragma once

#include <Common.hh>

namespace Kinoko::Field {

/// @brief Unique identifiers for each object
/// @note Even though @ref ObjectId::Itembox is also defined in @ref BlacklistedObjectId, it is
/// still included here so that @ref ObjectItemboxBlock::id() can return it for completeness.
enum class ObjectId {
    None = 0x0,              ///< Represents the absence of an object
    Psea = 0x2,              ///< The GCN Peach Beach rising water
    Itembox = 0x65,          ///< An item box
    DummyPole = 0x66,        ///< Used for some finish line poles, like on Mario Circuit
    Woodbox = 0x70,          ///< The DS Delfino Square breakable wooden boxes
    SunDS = 0x72,            ///< DS Desert Hills sun that shoots @ref ObjectFireSnake projectiles
    WLWallGC = 0xcb,         ///< The GCN Waluigi Stadium giant pipes (not including the piranhas)
    CarA1 = 0xcc,            ///< The last car before the last turn of Coconut Mall
    Basabasa = 0xcd,         ///< The Wario's Gold Mine and Dry Dry Ruins bat spawners
    HeyhoShipGBA = 0xce,     ///< GBA Shy Guy Beach ship shooting @ref ObjectHeyhoBall projectiles
    KoopaBall = 0xcf,        ///< The Bowser's Castle rolling/exploding fireball
    KartTruck = 0xd0,        ///< The Moonview Highway trucks
    CarBody = 0xd1,          ///< The Moonview Highway cars
    W_Woodbox = 0xd3,        ///< The Toad's Factory box spawners outside the factory
    ItemboxLine = 0xd5,      ///< Represents the Toad's Factory side stompers after the first turn
    VolcanoBall = 0xd6,      ///< The Grumble Volcano fireballs that erupt from the volcano
    PenguinS = 0xd7,         ///< N64 Sherbet Land penguins that slide on their belly
    PenguinM = 0xd8,         ///< N64 Sherbet Land penguins that walk in the cave
    Dossunc = 0xdb,          ///< A Thwomp or group of Thwomps, like on N64 Bowser's Castle
    DossuncSoko = 0xdc,      ///< ID for a Thwomp that is actively crushing
    Boble = 0xdd,            ///< The GBA Bowser Castle 3 rising/falling fireballs
    Hanachan = 0xe2,         ///< The Maple Treeway Wiggler
    Seagull = 0xe3,          ///< Flying group of seagulls, like on Toad's Factory
    Crab = 0xe5,             ///< The GBA Shy Guy Beach crabs
    BasabasaDummy = 0xe6,    ///< A bat spawned from an @ref ObjectBasabasa spawner
    CarA2 = 0xe7,            ///< The middle car before the last turn of Coconut Mall
    CarA3 = 0xe8,            ///< The first car before the last turn of Coconut Mall
    Hwanwan = 0xe9,          ///< The DS Peach Gardens walking Chain Chomps
    HeyhoBallGBA = 0xea,     ///< The GBA Shy Guy Beach cannonball shot from an @ref ObjectHeyhoShip
    DokanSFC = 0x12e,        ///< The SNES Mario Circuit 3 pipes
    CastleTree1c = 0x130,    ///< The Mario Circuit trees
    MarioTreeGCc = 0x134,    ///< The GCN Mario Circuit trees
    PeachTreeGCc = 0x138,    ///< The GCN Peach Beach trees
    ObakeBlockSFCc = 0x13c,  ///< A subset of the wall blocks on SNES Ghost Valley 2
    WLDokanGC = 0x13f,       ///< The GCN Waluigi Stadium giant piranhas (not including the pipes)
    MarioGo64c = 0x140,      ///< The N64 Mario Raceway rotating signs
    KinokoT1 = 0x142,        ///< The super tall mushrooms on Mushroom Gorge
    Pylon = 0x144,           ///< The Daisy Circuit traffic cones
    PalmTree = 0x145,        ///< The Coconut Mall palm trees
    Parasol = 0x146,         ///< The Coconut Mall umbrellas
    HeyhoTreeGBAc = 0x14a,   ///< The GBA Shy Guy Beach trees
    GardenTreeDSc = 0x151,   ///< The DS Peach Gardens trees
    DKtreeA64c = 0x158,      ///< A subset of the N64 DK's Jungle Parkway trees
    DKTreeB64c = 0x15a,      ///< A subset of the N64 DK's Jungle Parkway trees
    TownTreeDsc = 0x15b,     ///< The DS Delfino Square trees
    OilSFC = 0x15d,          ///< The SNES Mario Circuit 3 oil slick
    ParasolR = 0x16e,        ///< The GBA Shy Guy Beach umbrellas
    ObakeBlock2SFCc = 0x16f, ///< A subset of the wall blocks on SNES Ghost Valley 2
    ObakeBlock3SFCc = 0x170, ///< A subset of the wall blocks on SNES Ghost Valley 2
    KoopaFigure64 = 0x18b,   ///< The N64 Bowser's Castle Bowser statues
    Kuribo = 0x191,          ///< Goombas, like on Mario Circuit
    Choropu = 0x192,         ///< The Moo Moo Meadows moles that move along a @ref Rail
    Cow = 0x193,             ///< The Moo Moo Meadows cows
    PakkunF = 0x194,         ///< The GCN Mario Circuit piranhas (not including the pipe)
    WLFirebarGC = 0x195,     ///< The GCN Waluigi Stadium rotating firebars
    Wanwan = 0x196,          ///< The Mario Circuit and GCN Mario Circuit Chain Chomps
    Poihana = 0x197,         ///< The GCN Peach Beach cataquacks
    DKRockGC = 0x198,        ///< The GCN DK Mountain rocks that roll down the mountain
    Sanbo = 0x199,           ///< The Dry Dry Ruins and DS Desert Hills Pokies
    Choropu2 = 0x19a,        ///< The DS Peach Gardens moles which do not move
    TruckWagon = 0x19b,      ///< The Wario's Gold Mine minecart spawners
    Heyho = 0x19c,           ///< The DK Summit snowboarding Shy Guys
    Press = 0x19d,           ///< The Toad's Factory stompers
    PressSoko = 0x19e,       ///< ID for a stomper which is actively crushing
    WLFireRingGC = 0x1a1,    ///< The GCN Waluigi Stadium rotating fireball rings
    PakkunDokan = 0x1a2,     ///< The GCN Mario Circuit pipes holding piranha plants
    FireSnake = 0x1a4,       ///< The DS Desert Hills bouncing firesnakes
    KoopaFirebar = 0x1a5,    ///< The Bowser's Castle rotating firebar before the last turn
    Propeller = 0x1a6,       ///< The Koopa Cape rotating shell lasers
    DCPillarC = 0x1a7,       ///< The part of the Dry Dry Ruins pillars that falls
    FireSnakeV = 0x1a8,      ///< The Grumble Volcano bouncing firesnakes
    PuchiPakkun = 0x1aa,     ///< The N64 Mario Raceway piranhas
    KinokoUd = 0x1f5,        ///< Oscillating mushrooms on MG (functionality unused in-game)
    KinokoBend = 0x1f6,      ///< Bending mushrooms on MG (functionality unused in-game)
    VolcanoRock = 0x1f7,     ///< The Grumble Volcano oscillating platforms
    BulldozerL = 0x1f8,      ///< The left bulldozer on Toad's Factory
    BulldozerR = 0x1f9,      ///< The right bulldozer on Toad's Factory
    KinokoNm = 0x1fa,        ///< The green non-trickable mushrooms on Mushroom Gorge
    Crane = 0x1fb,           ///< The Toad's Factory moving platforms outside the factory
    VolcanoPiece = 0x1fc,    ///< The Grumble Volcano shaking/falling sections of the floor
    FlamePole = 0x1fd,       ///< The Bowser's Castle fire pillars erupting from geysers
    TwistedWay = 0x1fe,      ///< The Bowser's Castle twisting road at the start of the castle
    TownBridge = 0x1ff,      ///< The DS Delfino Square drawbridge
    DKShip64 = 0x200,        ///< The N64 DK's Jungle Parkway paddle steamer
    Turibashi = 0x202,       ///< The GCN DK Mountain swaying wooden bridge
    FlamePoleEff = 0x203,    ///< The fire pillar effect of a @ref ObjectFlamePole
    Aurora = 0x204,          ///< The Rainbow Road wavy trick road
    DCPillar = 0x208,        ///< Represents the base + pillar of the Dry Dry Ruins falling pillars
    Sandcone = 0x209,        ///< The Dry Dry Ruins rising sand cones
    Ami = 0x20e,             ///< The Maple Treeway net
    FlamePoleV = 0x212,      ///< A vertical fire pillar, like on Grumble Volcano
    FlamePoleVBig = 0x216,   ///< The large fire pillars at the end of N64 Bowser's Castle
    Mdush = 0x217,           ///< The Mario Circuit trick ramp near the Chain Chomp
    BeltEasy = 0x25a,        ///< The Toad's Factory conveyor belts outside the factory
    BeltCrossing = 0x25b,    ///< The Toad's Factory left+right conveyor belt after the first turn
    BeltCurveA = 0x25c,      ///< The Toad's Factory curved conveyor belts
    Escalator = 0x25e,       ///< The Coconut Mall single escalators
    EscalatorGroup = 0x260,  ///< The Coconut Mall double-escalators
};

/// @brief Unique identifiers for objects which are not implemented in Kinoko
/// @details These objects are purely cosmetic and do not have actual collision, or in the case of
/// Itembox it is not enabled in Time Trial mode, which Kinoko currently targets.
enum class BlacklistedObjectId {
    Itembox = 0x65,        ///< An item box
    CastleTree2 = 0x131,   ///< Decorative trees on Mario Circuit
    CastleFlower1 = 0x132, ///< Decorative flowers on Mario Circuit
    MarioTreeGC = 0x133,   ///< Decorative trees on GCN Mario Circuit
    DonkyTree1GC = 0x135,  ///< Decorative trees on GCN DK Mountain
    DonkyTree2GC = 0x136,  ///< Decorative trees on GCN DK Mountain
    PeachTreeGC = 0x137,   ///< Decorative trees on GCN Peach Beach
    PeachHunsuiGC = 0x141, ///< Decorative fountain on GCN Peach Beach
    GardenTreeDS = 0x150,  ///< Decorative trees on DS Peach Gardens
    FlagA1 = 0x152,        ///< Starting line flag on Wario's Gold Mine
    FlagA2 = 0x153,        ///< Starting line flag on Moo Moo Meadows
    FlagB1 = 0x154,        ///< "W" flag on Wario's Gold Mine
    FlagB2 = 0x155,        ///< "L" flag on Luigi Circuit
    FlagA3 = 0x156,        ///< Bright shell flag on Koopa Cape
    DKTreeA64 = 0x157,     ///< Decorative trees on N64 DK's Jungle Parkway
    DKTreeB64 = 0x159,     ///< Decorative trees on N64 DK's Jungle Parkway
    MiiBalloon = 0x160,    ///< Floating balloon on Coconut Mall
    Windmill = 0x161,      ///< Rotating windmill on Moo Moo Meadows
    TownTreeDS = 0x163,    ///< Decorative trees on DS Delfino Square
    Hanabi = 0x16a,        ///< Fireworks on cutscene tracks
    LightHouse = 0x16d,    ///< The lighthouse on Daisy Circuit
    FlagA5 = 0x180,        ///< Starting line flag on Koopa Cape
    SentakuDS = 0x189,     ///< Hanging clothes on DS Delfino Square
    FlagB3 = 0x2c4,        ///< Bright shell flag on Koopa Cape
    FlagB4 = 0x2c7,        ///< Dark shell flag on Koopa Cape
    UtsuboDokan = 0x2d6,   ///< Eels on Koopa Cape
    EnvSnow = 0x2ef,       ///< Snow effect on DK Summit
};

/// @brief Checks if the object ID is blacklisted and should not be loaded in Kinoko
/// @param id The object ID to check against the blacklist
/// @return `true` if the object ID is blacklisted and should not be loaded, `false` otherwise.
[[nodiscard]] static constexpr bool IsObjectBlacklisted(u16 id) {
    BlacklistedObjectId objectId = static_cast<BlacklistedObjectId>(id);
    switch (objectId) {
    // Disabled collision
    case BlacklistedObjectId::Itembox:
        return true;

    // No collision
    case BlacklistedObjectId::CastleTree2:
    case BlacklistedObjectId::CastleFlower1:
    case BlacklistedObjectId::MarioTreeGC:
    case BlacklistedObjectId::DonkyTree1GC:
    case BlacklistedObjectId::DonkyTree2GC:
    case BlacklistedObjectId::PeachTreeGC:
    case BlacklistedObjectId::PeachHunsuiGC:
    case BlacklistedObjectId::GardenTreeDS:
    case BlacklistedObjectId::FlagA1:
    case BlacklistedObjectId::FlagA2:
    case BlacklistedObjectId::FlagB1:
    case BlacklistedObjectId::FlagB2:
    case BlacklistedObjectId::FlagA3:
    case BlacklistedObjectId::DKTreeA64:
    case BlacklistedObjectId::Windmill:
    case BlacklistedObjectId::TownTreeDS:
    case BlacklistedObjectId::Hanabi:
    case BlacklistedObjectId::LightHouse:
    case BlacklistedObjectId::FlagA5:
    case BlacklistedObjectId::SentakuDS:
    case BlacklistedObjectId::FlagB3:
    case BlacklistedObjectId::FlagB4:
    case BlacklistedObjectId::UtsuboDokan:
    case BlacklistedObjectId::EnvSnow:
        return true;

    default:
        return false;
    }
}

} // namespace Kinoko::Field
