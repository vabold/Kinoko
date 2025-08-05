#pragma once

#include "game/field/CourseColMgr.hh"

#include <egg/math/Vector.hh>

#define COLLISION_ARR_LENGTH 0x40

class SavestateManager;

/// @brief Pertains to collision.
namespace Field {

/// @brief Manages the caching of colliding KCL triangles and exposes queries for collision checks.
/// @addr{0x809C2F44}
/// @nosubgrouping
class CollisionDirector : EGG::Disposer {
    friend class ::SavestateManager;

public:
    struct CollisionEntry {
        KCLTypeMask typeMask;
        u16 attribute;
        f32 dist;
    };

    void checkCourseColNarrScLocal(f32 radius, const EGG::Vector3f &pos, KCLTypeMask mask,
            u32 timeOffset);

    [[nodiscard]] bool checkSphereFull(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f &v1,
            KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut, u32 timeOffset);
    [[nodiscard]] bool checkSphereFullPush(f32 radius, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo,
            KCLTypeMask *pFlagsOut, u32 timeOffset);

    [[nodiscard]] bool checkSphereCachedPartial(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask typeMask, CollisionInfoPartial *info,
            KCLTypeMask *typeMaskOut, u32 timeOffset);
    [[nodiscard]] bool checkSphereCachedPartialPush(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask typeMask, CollisionInfoPartial *info,
            KCLTypeMask *typeMaskOut, u32 timeOffset);
    [[nodiscard]] bool checkSphereCachedFullPush(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask typeMask, CollisionInfo *info,
            KCLTypeMask *typeMaskOut, u32 timeOffset);

    void resetCollisionEntries(KCLTypeMask *ptr);
    void pushCollisionEntry(f32 dist, KCLTypeMask *typeMask, KCLTypeMask kclTypeBit, u16 attribute);

    /// @addr{0x807BDB5C}
    /// @todo Attributes should be represented as a bitfield. We can use a union. We will accomplish
    /// this in a future PR.
    void setCurrentCollisionVariant(u16 attribute) {
        ASSERT(m_collisionEntryCount > 0);
        u16 &entryAttr = m_entries[m_collisionEntryCount - 1].attribute;
        entryAttr = (entryAttr & 0xff1f) | (attribute << 5);
    }

    /// @addr{0x807BDBC4}
    /// @todo Attributes should be represented as a bitfield. We can use a union. We will accomplish
    /// this in a future PR.
    void setCurrentCollisionTrickable(bool trickable) {
        ASSERT(m_collisionEntryCount > 0);
        u16 &entryAttr = m_entries[m_collisionEntryCount - 1].attribute;
        entryAttr &= 0xdfff;

        if (trickable) {
            entryAttr |= (1 << 0xd);
        }
    }

    bool findClosestCollisionEntry(KCLTypeMask *typeMask, KCLTypeMask type);

    /// @beginGetters
    [[nodiscard]] const CollisionEntry *closestCollisionEntry() const {
        return m_closestCollisionEntry;
    }
    /// @endGetters

    static CollisionDirector *CreateInstance();
    static void DestroyInstance();

    [[nodiscard]] static CollisionDirector *Instance() {
        return s_instance;
    }

private:
    CollisionDirector();
    ~CollisionDirector() override;

    const CollisionEntry *m_closestCollisionEntry;
    std::array<CollisionEntry, COLLISION_ARR_LENGTH> m_entries;
    size_t m_collisionEntryCount;

    static CollisionDirector *s_instance; ///< @addr{0x809C2F44}
};

} // namespace Field
