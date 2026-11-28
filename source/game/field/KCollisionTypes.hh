#pragma once

/// @file KCollisionTypes.hh
/// This file houses information about KCL types. KCL files define collision for Mario Kart Wii and
/// other games. They organize collision triangles using an octree, which allows for efficient
/// indexing. Each triangle is associated with a 16-bit flag, which determines the effect the
/// triangle has on the player colliding with it.
/// <center><table><caption id="kcl_flag_components">KCL Flag Components</caption>
/// <tr><th>15<th>14<th>13<th>12<th>11<th>10<th>9<th>8<th>7<th>6<th>5<th>4<th>3<th>2<th>1<th>0
/// <tr><td colspan="3"><center>Collision Effect</center><td colspan="2"><center>Wheel
/// Depth</center><td colspan="3"><center>BLIGHT Index</center><td
/// colspan="3"><center>Variant</center><td colspan="5"><center>@ref
/// Kinoko::Field::KColType</center>
/// </table></center>
/// - **@ref Kinoko::Field::KColType** - 0x00 to 0x1F. The main effect the triangle will have.
/// - **Variant** - 0 to 7. Determines the variant of the base type.
/// - **BLIGHT Index** - Ranges from 0 to 7. Has to do with lighting.
/// - **Wheel Depth** - 0 to 3. A higher value means player's wheel sinks further into the ground.
/// - **Collision effect** - Bitfield that makes surfaces trickable, reject road, and/or soft wall.

#include <egg/math/Vector.hh>

namespace Kinoko::Field {

/// @brief A bitmask representing the @ref KColType of collision surfaces
typedef u32 KCLTypeMask;

/// @brief KCL attribute types
enum class KColType : u8 {
    Road = 0,                     ///< Default road
    SlipperyRoad = 1,             ///< Like Dry Dry Ruins
    WeakOffRoad = 2,              ///< Slight off-road slowdown
    OffRoad = 3,                  ///< Moderate off-road slowdown
    HeavyOffRoad = 4,             ///< Heavy off-road slowdown
    SlipperyRoad2 = 5,            ///< N64 Sherbet Land ice
    BoostPad = 6,                 ///< Boost panel
    BoostRamp = 7,                ///< Trickable. Variant affects boost duration
    JumpPad = 8,                  ///< Like GBA Shy Guy Beach
    ItemRoad = 9,                 ///< Road that is only solid for items
    SolidOOB = 0xa,               ///< Solid out-of-bounds trigger
    MovingWater = 0xb,            ///< Koopa Cape and DS Yoshi Falls
    Wall = 0xc,                   ///< Default wall
    InvisibleWall = 0xd,          ///< Solid wall that is (typically) invisible
    ItemWall = 0xe,               ///< Wall that is only solid for items
    Wall2 = 0xf,                  ///< Difference to @ref KColType::Wall is unknown
    FallBoundary = 0x10,          ///< Non-solid out-of-bounds trigger
    CannonTrigger = 0x11,         ///< Launches player to a destination "cannon point"
    ForceRecalculateRoute = 0x12, ///< Forces CPU pathing recalculation
    HalfpipeRamp = 0x13,          ///< Bowser's Castle half-pipe ramps
    PlayerWall = 0x14,            ///< Allows items to pass through but not players
    MovingRoad = 0x15,            ///< TF conveyor belts and CM escalators
    StickyRoad = 0x16,            ///< Player sticks if within 200 units (rBC stairs)
    Road2 = 0x17,                 ///< @ref KColType::Road but with more variants
    SoundTrigger = 0x18,          ///< Triggers for changing sound effects per-course
    WeakWall = 0x19,              ///< Pushes player away only at low speeds
    EffectTrigger = 0x1a,         ///< Activates various effects based on variant
    ItemStateModifier = 0x1b,     ///< Stops items when leaving curved TF conveyor belts
    HalfpipeInvisibleWall = 0x1c, ///< Invisible wall after a half-pipe jump, like in BC
    RotatingRoad = 0x1d,          ///< Chain Chomp Roulette
    SpecialWall = 0x1e,           ///< Various other wall types, determined by variant
    InvisibleWall2 = 0x1f,        ///< Identical to @ref KColType::InvisibleWall
};

/// @brief Computes the "Base Type" portion of the KCL flags. It's the lower 5 bits of the flag.
/// @param attribute The full 16-bit KCL flag
/// @return The @ref KColType portion of the flag
[[nodiscard]] constexpr KColType KCL_ATTRIBUTE_TYPE(u32 attribute) {
    return static_cast<KColType>(attribute & 0x1f);
}

/// @brief Converts a @ref KColType to its bit in a @ref KCLTypeMask. The game uses this to build
/// masks that represent multiple KCL types.
/// @param type The @ref KColType to convert
/// @return The type mask with only the bit for `type` set
[[nodiscard]] constexpr KCLTypeMask KCL_TYPE_BIT(KColType type) {
    return 1u << static_cast<u32>(type);
}

/// @brief Converts the full 16-bit KCL flag for a prism into the type mask bit for its base type
/// @param attribute The full 16-bit KCL flag
/// @return The type mask with only the bit for the flag's base type set
[[nodiscard]] constexpr KCLTypeMask KCL_ATTRIBUTE_TYPE_BIT(u32 attribute) {
    return KCL_TYPE_BIT(KCL_ATTRIBUTE_TYPE(attribute));
}

/// @brief Masks the soft wall bit within the "Collision Effect" field
constexpr u16 KCL_SOFT_WALL_MASK = 0x8000;

constexpr KCLTypeMask KCL_ANY = 0xffffffff;  ///< Represents all KCL type flags being set
constexpr KCLTypeMask KCL_NONE = 0x00000000; ///< Represents no KCL type flags being set

/// @brief Represents KCL which is only treated as a collision if the object is moving towards it
/// @details Value: 0x05070000
constexpr KCLTypeMask KCL_TYPE_DIRECTIONAL = KCL_TYPE_BIT(KColType::FallBoundary) |
        KCL_TYPE_BIT(KColType::SoundTrigger) | KCL_TYPE_BIT(KColType::ForceRecalculateRoute) |
        KCL_TYPE_BIT(KColType::EffectTrigger) | KCL_TYPE_BIT(KColType::CannonTrigger);
STATIC_ASSERT(KCL_TYPE_DIRECTIONAL == 0x05070000);

/// @brief Represents a tangible surface that the player or items can interact with
/// @details Value: 0xF0F8FFFF
constexpr KCLTypeMask KCL_TYPE_SOLID_SURFACE = ~KCL_TYPE_BIT(KColType::FallBoundary) &
        ~KCL_TYPE_BIT(KColType::CannonTrigger) & ~KCL_TYPE_BIT(KColType::ForceRecalculateRoute) &
        ~KCL_TYPE_BIT(KColType::SoundTrigger) & ~KCL_TYPE_BIT(KColType::WeakWall) &
        ~KCL_TYPE_BIT(KColType::EffectTrigger) & ~KCL_TYPE_BIT(KColType::ItemStateModifier);
STATIC_ASSERT(KCL_TYPE_SOLID_SURFACE == 0xF0F8FFFF);

/// @brief Represents surfaces that are drivable by players and/or items
/// @details Value: 0x20E80FFF
constexpr KCLTypeMask KCL_TYPE_FLOOR = KCL_TYPE_BIT(KColType::Road) |
        KCL_TYPE_BIT(KColType::SlipperyRoad) | KCL_TYPE_BIT(KColType::WeakOffRoad) |
        KCL_TYPE_BIT(KColType::OffRoad) | KCL_TYPE_BIT(KColType::HeavyOffRoad) |
        KCL_TYPE_BIT(KColType::SlipperyRoad2) | KCL_TYPE_BIT(KColType::BoostPad) |
        KCL_TYPE_BIT(KColType::BoostRamp) | KCL_TYPE_BIT(KColType::JumpPad) |
        KCL_TYPE_BIT(KColType::ItemRoad) | KCL_TYPE_BIT(KColType::SolidOOB) |
        KCL_TYPE_BIT(KColType::MovingWater) | KCL_TYPE_BIT(KColType::HalfpipeRamp) |
        KCL_TYPE_BIT(KColType::MovingRoad) | KCL_TYPE_BIT(KColType::StickyRoad) |
        KCL_TYPE_BIT(KColType::Road2) | KCL_TYPE_BIT(KColType::RotatingRoad);
STATIC_ASSERT(KCL_TYPE_FLOOR == 0x20E80FFF);

/// @brief Represents surfaces that are drivable by players but not items
/// @details Value: 0x20E80DFF
constexpr KCLTypeMask KCL_TYPE_DRIVER_FLOOR = KCL_TYPE_FLOOR & ~KCL_TYPE_BIT(KColType::ItemRoad);
STATIC_ASSERT(KCL_TYPE_DRIVER_FLOOR == 0x20E80DFF);

/// @brief Represents surfaces that act like walls for players and/or items
/// @details Value: 0xD010F000
constexpr KCLTypeMask KCL_TYPE_WALL = KCL_TYPE_BIT(KColType::Wall) |
        KCL_TYPE_BIT(KColType::InvisibleWall) | KCL_TYPE_BIT(KColType::ItemWall) |
        KCL_TYPE_BIT(KColType::Wall2) | KCL_TYPE_BIT(KColType::PlayerWall) |
        KCL_TYPE_BIT(KColType::HalfpipeInvisibleWall) | KCL_TYPE_BIT(KColType::SpecialWall) |
        KCL_TYPE_BIT(KColType::InvisibleWall2);
STATIC_ASSERT(KCL_TYPE_WALL == 0xD010F000);

/// @brief Represents surfaces that act like walls for players
/// @details Value: 0xC010B000
constexpr KCLTypeMask KCL_TYPE_DRIVER_WALL = KCL_TYPE_WALL & ~KCL_TYPE_BIT(KColType::ItemWall) &
        ~KCL_TYPE_BIT(KColType::HalfpipeInvisibleWall);
STATIC_ASSERT(KCL_TYPE_DRIVER_WALL == 0xC010B000);

/// @brief Represents @ref KCL_TYPE_DRIVER_WALL but without @ref KColType::InvisibleWall
/// @details Value: 0xC0109000
constexpr KCLTypeMask KCL_TYPE_DRIVER_WALL_NO_INVISIBLE_WALL =
        KCL_TYPE_DRIVER_WALL & ~KCL_TYPE_BIT(KColType::InvisibleWall);
STATIC_ASSERT(KCL_TYPE_DRIVER_WALL_NO_INVISIBLE_WALL == 0xC0109000);

/// @brief Represents @ref KCL_TYPE_DRIVER_WALL but without @ref KColType::InvisibleWall2
/// @details Value: 0x4010B000
constexpr KCLTypeMask KCL_TYPE_DRIVER_WALL_NO_INVISIBLE_WALL2 =
        KCL_TYPE_DRIVER_WALL & ~KCL_TYPE_BIT(KColType::InvisibleWall2);
STATIC_ASSERT(KCL_TYPE_DRIVER_WALL_NO_INVISIBLE_WALL2 == 0x4010B000);

/// @brief Represents all visible wall KCL types
/// @details Value: 0x4010D000
constexpr KCLTypeMask KCL_TYPE_VISIBLE_WALL =
        (KCL_TYPE_DRIVER_WALL_NO_INVISIBLE_WALL & ~KCL_TYPE_BIT(KColType::InvisibleWall2)) |
        KCL_TYPE_BIT(KColType::ItemWall);
STATIC_ASSERT(KCL_TYPE_VISIBLE_WALL == 0x4010D000);

/// @brief Represents surfaces that act like walls when objects collide with them
/// @details Value: 0x4000D000
constexpr KCLTypeMask KCL_TYPE_OBJECT_WALL = KCL_TYPE_BIT(KColType::SpecialWall) |
        KCL_TYPE_BIT(KColType::Wall2) | KCL_TYPE_BIT(KColType::ItemWall) |
        KCL_TYPE_BIT(KColType::Wall);
STATIC_ASSERT(KCL_TYPE_OBJECT_WALL == 0x4000D000);

/// @brief Represents all surfaces that vehicles can interact with
/// @details Value: 0xEFFFBDFF
constexpr KCLTypeMask KCL_TYPE_VEHICLE_INTERACTABLE = ~KCL_TYPE_BIT(KColType::ItemRoad) &
        ~KCL_TYPE_BIT(KColType::ItemWall) & ~KCL_TYPE_BIT(KColType::HalfpipeInvisibleWall);
STATIC_ASSERT(KCL_TYPE_VEHICLE_INTERACTABLE == 0xEFFFBDFF);

/// @brief Represents @ref KCL_TYPE_VEHICLE_INTERACTABLE but without triggers or fall boundaries
/// @details Value: 0xEAF8BDFF
constexpr KCLTypeMask KCL_TYPE_VEHICLE_COLLIDABLE = KCL_TYPE_VEHICLE_INTERACTABLE &
        ~KCL_TYPE_BIT(KColType::SoundTrigger) & ~KCL_TYPE_BIT(KColType::EffectTrigger) &
        ~KCL_TYPE_BIT(KColType::FallBoundary) & ~KCL_TYPE_BIT(KColType::CannonTrigger) &
        ~KCL_TYPE_BIT(KColType::ForceRecalculateRoute);
STATIC_ASSERT(KCL_TYPE_VEHICLE_COLLIDABLE == 0xEAF8BDFF);

/// @brief Represents the collision types that @ref Render::KartCamera checks for collision with
/// @details Value: 0x62F8BDFF
constexpr KCLTypeMask KCL_TYPE_CAMERA_COLLIDABLE = KCL_TYPE_VEHICLE_COLLIDABLE &
        ~KCL_TYPE_BIT(KColType::InvisibleWall2) & ~KCL_TYPE_BIT(KColType::ItemStateModifier);
STATIC_ASSERT(KCL_TYPE_CAMERA_COLLIDABLE == 0x62F8BDFF);

/// @brief Represents all KCL types whose collision is not conditional on the direction of impact
/// @details Value: 0xE0F8BDFF
constexpr KCLTypeMask KCL_TYPE_NON_DIRECTIONAL = KCL_TYPE_VEHICLE_COLLIDABLE &
        ~KCL_TYPE_BIT(KColType::ItemStateModifier) & ~KCL_TYPE_BIT(KColType::WeakWall);
STATIC_ASSERT(KCL_TYPE_NON_DIRECTIONAL == 0xE0F8BDFF);

/// @brief Represents the collision types that the kart's body can collide with
/// @details Value: 0xEAFABDFF
constexpr KCLTypeMask KCL_TYPE_BODY_COLLIDABLE =
        KCL_TYPE_VEHICLE_COLLIDABLE | KCL_TYPE_BIT(KColType::CannonTrigger);
STATIC_ASSERT(KCL_TYPE_BODY_COLLIDABLE == 0xEAFABDFF);

/// @brief Represents the collision types that a kart body hitbox flagged "walls only" can collide
/// with
/// @details Value: 0x4A109000
constexpr KCLTypeMask KCL_TYPE_BODY_WALLS_ONLY_COLLIDABLE = KCL_TYPE_BIT(KColType::Wall) |
        KCL_TYPE_BIT(KColType::Wall2) | KCL_TYPE_BIT(KColType::PlayerWall) |
        KCL_TYPE_BIT(KColType::SpecialWall) | KCL_TYPE_BIT(KColType::WeakWall) |
        KCL_TYPE_BIT(KColType::ItemStateModifier);
STATIC_ASSERT(KCL_TYPE_BODY_WALLS_ONLY_COLLIDABLE == 0x4A109000);

/// @brief Represents KCL types for which @ref Kart::KartReject can apply a rejection
/// @details Value: 0xB0E82DFF
constexpr KCLTypeMask KCL_TYPE_REJECTABLE = KCL_TYPE_DRIVER_FLOOR |
        KCL_TYPE_BIT(KColType::HalfpipeInvisibleWall) | KCL_TYPE_BIT(KColType::InvisibleWall) |
        KCL_TYPE_BIT(KColType::InvisibleWall2);
STATIC_ASSERT(KCL_TYPE_REJECTABLE == 0xB0E82DFF);

/// @brief Represents a tangible surface that objects can interact with
/// @details Value: 0x60E8DFFF
constexpr KCLTypeMask KCL_TYPE_OBJECT_SOLID_SURFACE = KCL_TYPE_FLOOR |
        KCL_TYPE_BIT(KColType::Wall) | KCL_TYPE_BIT(KColType::ItemWall) |
        KCL_TYPE_BIT(KColType::Wall2) | KCL_TYPE_BIT(KColType::SpecialWall);
STATIC_ASSERT(KCL_TYPE_OBJECT_SOLID_SURFACE == 0x60E8DFFF);

/// @brief Represents all KCL types that objects can collide with
/// @details Value: 0x64EBDFFF
constexpr KCLTypeMask KCL_TYPE_OBJECT_COLLIDABLE = KCL_TYPE_OBJECT_SOLID_SURFACE |
        KCL_TYPE_BIT(KColType::EffectTrigger) | KCL_TYPE_BIT(KColType::CannonTrigger) |
        KCL_TYPE_BIT(KColType::FallBoundary);
STATIC_ASSERT(KCL_TYPE_OBJECT_COLLIDABLE == 0x64EBDFFF);

/// @brief Represents all KCL types that objects can collide with, as well as
/// @ref KColType::ItemStateModifier
/// @details Value: 0x6CEBDFFF
constexpr KCLTypeMask KCL_TYPE_OBJECT_COLLIDABLE_MODIFIABLE =
        KCL_TYPE_OBJECT_COLLIDABLE | KCL_TYPE_BIT(KColType::ItemStateModifier);
STATIC_ASSERT(KCL_TYPE_OBJECT_COLLIDABLE_MODIFIABLE == 0x6CEBDFFF);

/// @brief Represents both @ref KColType::InvisibleWall and @ref KColType::InvisibleWall2
/// @details Value: 0x80002000
constexpr KCLTypeMask KCL_TYPE_INVISIBLE_WALL =
        KCL_TYPE_BIT(KColType::InvisibleWall) | KCL_TYPE_BIT(KColType::InvisibleWall2);
STATIC_ASSERT(KCL_TYPE_INVISIBLE_WALL == 0x80002000);

/// @brief Represents all types of invisible walls (@ref KColType::InvisibleWall,
/// @ref KColType::InvisibleWall2, and @ref KColType::HalfpipeInvisibleWall)
/// @details Value: 0x90002000
constexpr KCLTypeMask KCL_TYPE_ANY_INVISIBLE_WALL =
        KCL_TYPE_INVISIBLE_WALL | KCL_TYPE_BIT(KColType::HalfpipeInvisibleWall);
STATIC_ASSERT(KCL_TYPE_ANY_INVISIBLE_WALL == 0x90002000);

/// @brief The header of the KCL file format. It is 0x3C bytes long (for Mario Kart Wii).
struct KColHeader {
    u32 pos_data_offset;        ///< 0-indexed offset to array of position vectors
    u32 nrm_data_offset;        ///< 0-indexed offset to array of normal vectors
    u32 prism_data_offset;      ///< 1-indexed offset to array of prisms
    u32 block_data_offset;      ///< Offset to collision octree blocks
    f32 prism_thickness;        ///< Depth of a triangular prism along its normal vector
    EGG::Vector3f area_min_pos; ///< Smallest possible coordinate in the model
    u32 area_x_width_mask;      ///< The x dimension of the octree's bounding box
    u32 area_y_width_mask;      ///< The y dimension of the octree's bounding box
    u32 area_z_width_mask;      ///< The z dimension of the octree's bounding box
    u32 block_width_shift;      ///< Used to initialize octree navigation
    u32 area_x_blocks_shift;    ///< Used to initialize octree navigation
    u32 area_xy_blocks_shift;   ///< Used to initialize octree navigation
    f32 sphere_radius;          ///< Clamps the sphere we check collision against
};

STATIC_ASSERT(sizeof(KColHeader) == 0x3C);

} // namespace Kinoko::Field
