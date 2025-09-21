#pragma once

#include "game/field/CourseColMgr.hh"

#include <egg/math/Vector.hh>

#define COLLISION_ARR_LENGTH 0x40

namespace Host {

class Context;

} // namespace Host

/// @brief Pertains to collision.
namespace Field {

/// @brief Manages the caching of colliding KCL triangles and exposes queries for collision checks.
/// @addr{0x809C2F44}
/// @nosubgrouping
class CollisionDirector : EGG::Disposer {
    friend class Host::Context;

public:
    enum class eCollisionAttribute {
        Trickable = 13
    };
    typedef EGG::TBitFlag<u16, eCollisionAttribute> CollisionAttribute;

    struct CollisionEntry {
        KCLTypeMask typeMask;
        CollisionAttribute attribute;
        f32 dist;
        
        // Credit: em-eight/mkw
        /// Extracts the "Variant" portion of the KCL flag. It's the 3 bits before the "Bast Type".
        u16 attributeVariant() const { 
            return (attribute >> 5) & 7;
        }

        u16 attributeBaseType() const {
            return attribute & 0x1F;
        }

        void setVariant(u16 variant) {
            u16 current = static_cast<u16>(attribute);
            attribute = static_cast<CollisionAttribute>(
                (current & ~0xE0) | ((variant & 7) << 5)
            );
        }
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
    void setCurrentCollisionVariant(u16 attribute) {
        ASSERT(m_collisionEntryCount > 0);
        m_entries[m_collisionEntryCount - 1].setVariant(attribute);
    }

    /// @addr{0x807BDBC4}
    void setCurrentCollisionTrickable(bool trickable) {
        ASSERT(m_collisionEntryCount > 0);
        CollisionEntry &entry = m_entries[m_collisionEntryCount - 1];
        trickable ? entry.attribute.setBit(eCollisionAttribute::Trickable)
                  : entry.attribute.resetBit(eCollisionAttribute::Trickable);
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
