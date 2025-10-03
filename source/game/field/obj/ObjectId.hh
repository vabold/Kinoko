#pragma once

#include <Common.hh>

namespace Field {

enum class ObjectId {
    None = 0x0,
    Itembox = 0x65,
    DummyPole = 0x066,
    Woodbox = 0x70,
    SunDS = 0x72,
    WLWallGC = 0xcb,
    CarA1 = 0xcc,
    KartTruck = 0xd0,
    CarBody = 0xd1,
    W_Woodbox = 0xd3,
    ItemboxLine = 0xd5,
    Boble = 0xdd,
    Seagull = 0xe3,
    CarA2 = 0xe7,
    CarA3 = 0xe8,
    DokanSFC = 0x12e,
    CastleTree1c = 0x130,
    MarioTreeGCc = 0x134,
    PeachTreeGCc = 0x138,
    ObakeBlockSFCc = 0x13c,
    WLDokanGC = 0x13f,
    MarioGo64c = 0x140,
    KinokoT1 = 0x142,
    PalmTree = 0x145,
    Parasol = 0x146,
    HeyhoTreeGBAc = 0x14a,
    GardenTreeDSc = 0x151,
    DKtreeA64c = 0x158,
    DKTreeB64c = 0x15a,
    TownTreeDsc = 0x15b,
    OilSFC = 0x15d,
    ParasolR = 0x16e,
    ObakeBlock2SFCc = 0x16f,
    ObakeBlock3SFCc = 0x170,
    Kuribo = 0x191,
    Choropu = 0x192,
    Cow = 0x193,
    WLFirebarGC = 0x195,
    Sanbo = 0x199,
    Choropu2 = 0x19a,
    Press = 0x19d,
    PressSoko = 0x19e,
    WLFireRingGC = 0x1a1,
    PakkunDokan = 0x1a2,
    FireSnake = 0x1a4,
    PuchiPakkun = 0x1aa,
    KinokoUd = 0x1f5,
    KinokoBend = 0x1f6,
    BulldozerL = 0x1f8,
    BulldozerR = 0x1f9,
    KinokoNm = 0x1fa,
    Crane = 0x1fb,
    TwistedWay = 0x1fe,
    TownBridge = 0x1ff,
    Turibashi = 0x202,
    Aurora = 0x204,
    Sandcone = 0x209,
    Ami = 0x20e,
    Mdush = 0x217,
    BeltEasy = 0x25a,
    BeltCrossing = 0x25b,
    BeltCurveA = 0x25c,
    Escalator = 0x25e,
    EscalatorGroup = 0x260,
};

enum class BlacklistedObjectId {
    Itembox = 0x65,
    CastleTree2 = 0x131,
    CastleFlower1 = 0x132,
    MarioTreeGC = 0x133,
    DonkyTree1GC = 0x135,
    DonkyTree2GC = 0x136,
    PeachTreeGC = 0x137,
    PeachHunsuiGC = 0x141,
    GardenTreeDS = 0x150,
    FlagA1 = 0x152,
    FlagA2 = 0x153,
    FlagB1 = 0x154,
    FlagB2 = 0x155,
    FlagA3 = 0x156,
    DKTreeA64 = 0x157,
    DKTreeB64 = 0x159,
    MiiBalloon = 0x160,
    Windmill = 0x161,
    TownTreeDS = 0x163,
    Hanabi = 0x16a,
    LightHouse = 0x16d,
    FlagA5 = 0x180,
    SentakuDS = 0x189,
    FlagB3 = 0x2c4,
    FlagB4 = 0x2c7,
    UtsuboDokan = 0x2d6,
    EnvSnow = 0x2ef,
};

static constexpr bool IsObjectBlacklisted(u16 id) {
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

} // namespace Field
