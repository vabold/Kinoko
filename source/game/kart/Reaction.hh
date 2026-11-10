#pragma once

namespace Kinoko::Kart {

/// @brief Represents the different reactions that can occur when a kart collides with an object.
/// @details @ref Field::ObjectCollidable objects can specify a reaction in their `onCollision`
/// function in order to determine how the kart should respond to the collision.
enum class Reaction {
    None = 0,                        ///< No reaction occurs, the kart continues as normal
    UNK_3 = 3,                       ///< Unknown
    UNK_4 = 4,                       ///< Unknown
    UNK_5 = 5,                       ///< Unknown
    UNK_7 = 7,                       ///< Unknown
    WallAllSpeed = 8,                ///< The kart collides with a wall and is knocked back
    SpinAllSpeed = 9,                ///< The kart spins out once but keeps its items
    SpinSomeSpeed = 10,              ///< The kart spins out twice but keeps its items
    FireSpin = 11,                   ///< The kart spins out due to a fireball collision
    ClipThroughSomeSpeed = 12,       ///< Unused in Kinoko
    SmallLaunch = 13,                ///< The kart launches forward into the air
    KnockbackSomeSpeedLoseItem = 14, ///< Launches into the air away from collision and flips once
    LaunchSpinLoseItem = 15,    ///< The kart launches into the air, flips once, and loses its items
    KnockbackBumpLoseItem = 16, ///< Launches into the air away from the collision and flips twice
    LongCrushLoseItem = 17,     ///< The kart is crushed for 8 seconds and loses its items
    SmallBump = 18,             ///< The kart is bumped away by the colliding object
    BigBump = 19,               ///< Unused in Kinoko
    SpinShrink = 20,            ///< The kart spins out and shrinks in size temporarily
    HighLaunchLoseItem = 21,    ///< The kart launches high into the air, and loses its items
    SpinHitSomeSpeed = 22,   ///< Decays to SpinSomeSpeed or WallAllSpeed based on the kart's speed
    WeakWall = 23,           ///< The kart's speed drops by 82% due to a weak wall
    Wall = 24,               ///< The kart will slowdown due to offroad collision
    LaunchSpin = 25,         ///< The kart launches sideways and flips once
    WallSpark = 26,          ///< The kart is colliding with Wall3
    RubberWall = 27,         ///< Elastic collision sending the kart away from the colliding object
    Wall2 = 28,              ///< Unused in Kinoko
    UntrickableJumpPad = 29, ///< The kart bounces into the air like a jump pad but cannot trick
    ShortCrushLoseItem = 30, ///< The kart is crushed for 4 seconds and loses its items
    CrushRespawn = 31,       ///< The kart is crushed and respawns (unused in Kinoko)
    ExplosionLoseItem = 32,  ///< The kart loses its items and is knocked upwards
};

} // namespace Kinoko::Kart
