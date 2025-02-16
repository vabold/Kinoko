#pragma once

#include "game/field/CourseColMgr.hh"

#include <egg/math/Vector.hh>

#define COLLISION_ARR_LENGTH 0x40

/// @brief Pertains to collision.
namespace Field {

struct CollisionInfo {
    EGG::BoundBox3f bbox;
    EGG::Vector3f tangentOff;
    EGG::Vector3f floorNrm;
    EGG::Vector3f wallNrm;
    EGG::Vector3f _3c;
    f32 floorDist;
    f32 wallDist;
    f32 _50;
    f32 perpendicularity;

    void updateFloor(f32 dist, const EGG::Vector3f &fnrm);
    void updateWall(f32 dist, const EGG::Vector3f &fnrm);
    void update(f32 now_dist, const EGG::Vector3f &offset, const EGG::Vector3f &fnrm,
            u32 kclAttributeTypeBit);
};

/// @brief Manages the caching of colliding KCL triangles and exposes queries for collision checks.
/// @addr{0x809C2F44}
/// @nosubgrouping
class CollisionDirector : EGG::Disposer {
public:
    struct CollisionEntry {
        KCLTypeMask typeMask;
        u16 attribute;
        f32 dist;
    };

    void checkCourseColNarrScLocal(f32 radius, const EGG::Vector3f &pos, KCLTypeMask mask,
            bool scaledUp);

    [[nodiscard]] bool checkSphereFull(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f &v1,
            KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut, u32 start);
    [[nodiscard]] bool checkSphereFullPush(f32 radius, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo,
            KCLTypeMask *pFlagsOut, u32 start);

    [[nodiscard]] bool checkSphereCachedPartial(const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask typeMask, CollisionInfo *colInfo,
            KCLTypeMask *typeMaskOut, f32 radius, u32 start);
    [[nodiscard]] bool checkSphereCachedPartialPush(const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask typeMask, CollisionInfo *colInfo,
            KCLTypeMask *typeMaskOut, f32 radius, u32 start);
    [[nodiscard]] bool checkSphereCachedFullPush(const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask typeMask, CollisionInfo *colInfo,
            KCLTypeMask *typeMaskOut, f32 radius, u32 start);

    void resetCollisionEntries(KCLTypeMask *ptr);
    void pushCollisionEntry(f32 dist, KCLTypeMask *typeMask, KCLTypeMask kclTypeBit, u16 attribute);
    void setCurrentCollisionVariant(u16 attribute);
    void setCurrentCollisionTrickable(bool trickable);

    bool findClosestCollisionEntry(KCLTypeMask *typeMask, KCLTypeMask type);

    /// @beginGetters
    [[nodiscard]] const CollisionEntry *closestCollisionEntry() const;
    /// @endGetters

    static CollisionDirector *CreateInstance();
    [[nodiscard]] static CollisionDirector *Instance();
    static void DestroyInstance();

private:
    CollisionDirector();
    ~CollisionDirector() override;

    const CollisionEntry *m_closestCollisionEntry;
    std::array<CollisionEntry, COLLISION_ARR_LENGTH> m_entries;
    size_t m_collisionEntryCount;

    static CollisionDirector *s_instance; ///< @addr{0x809C2F44}
};

} // namespace Field
