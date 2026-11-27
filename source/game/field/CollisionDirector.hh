#pragma once

#include "game/field/CollisionInfo.hh"

#include <egg/core/Heap.hh>

namespace Kinoko {

namespace Host {

class Context;

} // namespace Host

/// @brief Pertains to collision.
namespace Field {

class CourseColMgr;
class KColData;
class ObjectDrivableDirector;

/// @brief Manages the caching of colliding KCL triangles and exposes queries for collision checks
/// @details Stores up to 64 cached collision entries which can be used by classes like @ref
/// Kart::KartCollide to fetch tris without having to perform another collision query. Exposes
/// public interfaces for collision checks, including full vs. partial checks (whether resulting
/// collisions return just a subset of collision information), cached vs. uncached checks (whether
/// we can leverage the cache), and push variants (whether we want to add collision entries to the
/// cache). Also exposes functionality to find the closest colliding entry that matches a provided
/// @ref KCLTypeMask.
class CollisionDirector : EGG::Disposer {
    /// @brief Grants access to the singleton so a @ref Host::Context can restore the instance's
    /// state on context switch
    friend class Host::Context;

public:
    /// @brief Collision Entry Attribute fields
    /// @details Each KCL triangle is associated with a 16-bit attribute field which determines the
    /// effect of that triangle on the player that is colliding with it.\n\n
    /// Bit layout:
    /// | Bits 0-4 | Bits 5-7 | Bits 8-10 | Bits 11-12 | Bit 13    | Bit 14      | Bit 15        |
    /// |----------|----------|-----------|------------|-----------|-------------|---------------|
    /// | @ref KColType | Variant | (unused) | Intensity | Trickable | Reject road | Soft surface |
    /// - **@ref KColType**: (0-31) Represents the main category of effect (road, offroad, etc.)
    /// - **Variant**: (0-7) Represents variations in behavior that are specific to the base type
    /// - **Intensity**: (0-3) Defines how deep vehicle wheels "sink" into the road
    /// - **Trickable**: (0/1) Enables tricking on this surface
    /// - **Reject Road**: (0/1) The surface will try to redirect the player's direction
    /// - **Soft Surface**: (0/1) A barrel roll wall, used to prevent players from hanging on ledges
    enum class eCollisionAttribute {
        Trickable = 13,  ///< Whether the player can perform a trick on this surface
        RejectRoad = 14, ///< A surface that tries to push you back towards the road
        Soft = 15,       ///< Barrel roll walls
    };

    /// @brief A bitfield of @ref eCollisionAttribute that represents the attributes field of a
    /// colliding KCL triangle
    typedef EGG::TBitFlag<u16, eCollisionAttribute> CollisionAttribute;

    /// @brief Represents traits of a colliding KCL triangle
    /// @todo We should be able to get rid of typeMask as it is never accessed directly and can
    /// instead be identically retrieved via baseType() (it is always in parity)
    struct CollisionEntry {
        KCLTypeMask typeMask;         ///< The base type mask
        CollisionAttribute attribute; ///< The full 16-bit attribute field
        f32 dist;                     ///< How far the colliding body penetrates the tri

        /// @beginGetters

        /// @brief Gets the base type of the tri (bits 0-4)
        /// @return The @ref KColType of the tri as a `u16`
        [[nodiscard]] u16 baseType() const {
            return attribute & 0x1F;
        }

        /// @brief Gets the variant of the tri (bits 5-7)
        /// @return The variant of the tri
        [[nodiscard]] u16 variant() const {
            return (attribute >> 5) & 7;
        }

        /// @brief Gets the intensity of the wheel "sinking" effect (bits 11-12)
        /// @return The intensity of the wheel "sinking" effect
        [[nodiscard]] u16 intensity() const {
            return (attribute >> 11) & 3;
        }

        /// @endGetters

        /// @beginSetters

        /// @brief Updates the variant bits in the collision attribute field
        /// @param variant The new variant value to set
        void setVariant(u16 variant) {
            u16 current = static_cast<u16>(attribute);
            attribute = static_cast<CollisionAttribute>((current & ~0xE0) | ((variant & 7) << 5));
        }

        /// @endSetters
    };

    /// @beginPublicColCheckFuncs

    [[nodiscard]] bool checkSpherePartialPush(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut, u32 timeOffset);
    [[nodiscard]] bool checkSphereFull(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset);
    [[nodiscard]] bool checkSphereFullPush(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset);

    [[nodiscard]] bool checkSphereCachedPartial(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut, u32 timeOffset);
    [[nodiscard]] bool checkSphereCachedPartialPush(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut, u32 timeOffset);
    [[nodiscard]] bool checkSphereCachedFullPush(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset);

    void checkCourseColNarrScLocal(f32 radius, const EGG::Vector3f &pos, KCLTypeMask mask,
            u32 timeOffset);

    /// @endPublicColCheckFuncs

    void resetCollisionEntries(KCLTypeMask *maskOut);
    void pushCollisionEntry(f32 dist, KCLTypeMask *typeMask, KCLTypeMask kclTypeBit,
            CollisionAttribute attribute);
    [[nodiscard]] const CollisionEntry *findClosestCollisionEntry(KCLTypeMask type) const;

    /// @beginSetters

    /// @addr{0x807BDB5C}
    /// @brief Updates the variant for the currently colliding tri
    /// @param variant The new variant to set for the currently colliding tri
    /// @details As an example, SNES Ghost Valley 2 blocks (managed by @ref ObjectObakeManager) are
    /// updated to have special wall variant 2 (bouncy wall) when colliding with the side of the
    /// block so that you bounce off of them when colliding at slow speeds.
    void setCurrentCollisionVariant(u16 variant) {
        ASSERT(m_collisionEntryCount > 0);
        m_entries[m_collisionEntryCount - 1].setVariant(variant);
    }

    /// @addr{0x807BDBC4}
    /// @brief Toggles whether the currently colliding tri is trickable
    /// @param trickable Whether the currently colliding tri should be trickable
    /// @details This is used by dynamic objects to determine whether the player can trick. For
    /// example, the geyser mounds at the end of Bowser's Castle (@ref ObjectFlamePoleFoot) are only
    /// trickable if their scale is larger than 2x.
    void setCurrentCollisionTrickable(bool trickable) {
        ASSERT(m_collisionEntryCount > 0);
        CollisionEntry &entry = m_entries[m_collisionEntryCount - 1];
        entry.attribute.changeBit(trickable, eCollisionAttribute::Trickable);
    }

    /// @endSetters

    /// @beginSingletonFuncs

    static CollisionDirector *CreateInstance();
    static void DestroyInstance();

    /// @brief Gets the singleton instance of the @ref CollisionDirector
    /// @return A pointer to the singleton instance of the @ref CollisionDirector
    [[nodiscard]] static CollisionDirector *Instance() {
        return s_instance;
    }

    /// @endSingletonFuncs

private:
    EGG_NEW_DELETE_FRIEND

    CollisionDirector();
    ~CollisionDirector() override;

    /// @brief Function pointer type for a @ref CourseColMgr sphere collision check
    /// @tparam T The collision info object type, either @ref CollisionInfo or @ref
    /// CollisionInfoPartial
    template <typename T>
    using CourseColCheckFunc = bool (CourseColMgr::*)(f32 scale, f32 radius, KColData *data,
            const EGG::Vector3f &pos, const EGG::Vector3f &prevPos, KCLTypeMask mask, T *info,
            KCLTypeMask *maskOut);

    /// @brief Function pointer type for an @ref ObjectDrivableDirector sphere collision check
    /// @tparam T The collision info object type, either @ref CollisionInfo or @ref
    /// CollisionInfoPartial
    template <typename T>
    using DrivableColCheckFunc = bool (ObjectDrivableDirector::*)(f32 radius,
            const EGG::Vector3f &pos, const EGG::Vector3f &prevPos, KCLTypeMask mask, T *info,
            KCLTypeMask *maskOut, u32 timeOffset);

    /// @beginPrivateColCheckFuncs

    template <typename T>
        requires std::is_same_v<T, CollisionInfo> || std::is_same_v<T, CollisionInfoPartial>
    [[nodiscard]] bool checkSphereImpl(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, T *info, KCLTypeMask *maskOut,
            u32 timeOffset, CourseColCheckFunc<T> courseCheckFunc,
            DrivableColCheckFunc<T> drivableCheckFunc, bool cached, bool push);

    /// @endPrivateColCheckFuncs

    /// @brief The maximum number of collision entries that can be cached
    static constexpr size_t COLLISION_ARR_LENGTH = 0x40;

    std::array<CollisionEntry, COLLISION_ARR_LENGTH> m_entries; ///< Array of all colliding tris
    size_t m_collisionEntryCount;                               ///< Number of valid colliding tris

    /// @addr{0x809C2F44}
    /// @brief The pointer to the singleton instance of the class
    static CollisionDirector *s_instance;
};

} // namespace Field

} // namespace Kinoko
