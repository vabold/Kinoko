#pragma once

#include <egg/core/BitFlag.hh>

namespace Kart {

/// @brief This is a union of the 5 bitfields contained within KartState
enum class eStatus {
    Accelerate = 0, ///< Accel button is pressed.
    Brake = 1,      ///< Brake button is pressed.
    /// @brief A "fake" button, normally set if you meet the speed requirement to hop.
    /// @warning When playing back a ghost, the game will register a hop regardless of whether
    /// or not the acceleration button is pressed. This can lead to "successful" synchronization
    /// of ghosts which could not have been created legitimately in the first place.
    DriftInput = 2,
    DriftManual = 3,                ///< Currently in a drift w/ manual.
    BeforeRespawn = 4,              ///< Set on respawn collision, cleared on position snap.
    Wall3Collision = 5,             ///< Set when colliding with wall KCL #COL_TYPE_WALL_2
    WallCollision = 6,              ///< Set if we are colliding with a wall.
    HopStart = 7,                   ///< Set if @ref m_bDriftInput was toggled on this frame.
    AccelerateStart = 8,            ///< Set if @ref m_bAccelerate was toggled on this frame.
    GroundStart = 9,                ///< Set first frame landing from airtime.
    VehicleBodyFloorCollision = 10, ///< Set if the vehicle body is colliding with the floor.
    AnyWheelCollision = 11,         ///< Set when any wheel is touching floor collision.
    AllWheelsCollision = 12,        ///< Set when all wheels are touching floor collision.
    StickLeft = 13,          ///< Set on left stick input. Mutually exclusive to @ref m_bStickRight.
    WallCollisionStart = 14, ///< Set if we have just started colliding with a wall.
    AirtimeOver20 = 15,      ///< Set after 20 frames of airtime, resets on landing.
    StickyRoad = 16,         ///< Like the rBC stairs
    TouchingGround = 18,     ///< Set when any part of the vehicle is colliding with floor KCL.
    Hop = 19,                ///< Set while we are in a drift hop. Clears when we land.
    Boost = 20,              ///< Set while in a boost.
    DisableAcceleration = 22,
    AirStart = 23,
    StickRight = 24,      ///< Set on right stick input. Mutually exclusive to @ref m_bStickLeft.
    LargeFlipHit = 25,    ///< Set when hitting an exploding object.
    MushroomBoost = 26,   ///< Set while we are in a mushroom boost.
    SlipdriftCharge = 27, ///< Currently in a drift w/ automatic.
    DriftAuto = 28,
    Wheelie = 29, ///< Set while we are in a wheelie (even during the countdown).
    JumpPad = 30,
    RampBoost = 31,

    InAction = 32,
    TriggerRespawn = 33,
    CannonStart = 35,
    InCannon = 36,
    TrickStart = 37,
    InATrick = 38,
    BoostOffroadInvincibility = 39, ///< Set if we should ignore offroad slowdown this frame.
    HalfPipeRamp = 41,              ///< Set while colliding with zipper KCL.
    OverZipper = 42,                ///< Set while mid-air from a zipper.
    JumpPadMushroomCollision = 43,
    ZipperInvisibleWall = 44,   ///< Set when colliding with invisible wall above a zipper.
    ZipperBoost = 45,           ///< Set when boosting after landing from a zipper.
    ZipperStick = 46,           ///< Set while mid-air and still influenced by the zipper.
    ZipperTrick = 47,           ///< Set while tricking mid-air from a zipper.
    DisableBackwardsAccel = 48, ///< Enforces a 20f delay when reversing after charging SSMT.
    RespawnKillY = 49,          ///< Set while respawning to cap external velocity at 0.
    Burnout = 50,               ///< Set during a burnout on race start.
    TrickRot = 54,
    JumpPadMushroomVelYInc = 55,
    ChargingSSMT = 57,      ///< Tracks whether we are charging a stand-still mini-turbo.
    RejectRoad = 59,        ///< Collision which causes a change in the player's pos and rot.
    RejectRoadTrigger = 60, ///< e.g. DK Summit ending, and Maple Treeway side walls.
    Trickable = 62,

    WheelieRot = 68,
    SkipWheelCalc = 69,
    JumpPadMushroomTrigger = 70,
    MovingWaterStickyRoad = 73,
    NoSparkInvisibleWall = 75,
    CollidingOffroad = 76,
    InRespawn = 77,
    AfterRespawn = 78,
    Crushed = 80,
    JumpPadFixedSpeed = 84,
    MovingWaterDecaySpeed = 85,
    JumpPadDisableYsusForce = 86,
    HalfpipeMidair = 87,

    UNK2 = 97,
    SomethingWallCollision = 99,
    SoftWallDrift = 100,
    HWG = 101, ///< Set when "Horizontal Wall Glitch" is active.
    AfterCannon = 102,
    ActionMidZipper = 103,  ///< Set when we enter an action while mid-air from a zipper.
    ChargeStartBoost = 104, ///< Like @ref m_bAccelerate but during countdown.
    MovingWaterVertical = 105,
    EndHalfPipe = 107,

    AutoDrift = 133, ///< True if auto transmission, false if manual.

    FlagMax, ///< Internal. Total number of bits in the status.
};
typedef EGG::TBitFlagExt<static_cast<size_t>(eStatus::FlagMax), eStatus> Status;

} // namespace Kart
