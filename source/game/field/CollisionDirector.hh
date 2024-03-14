#pragma once

#include "game/field/CourseColMgr.hh"

#include <egg/math/Vector.hh>

#define COLLISION_ARR_LENGTH 0x40

namespace Field {

class CollisionDirector {
public:
    struct CollisionEntry {
        KCLTypeMask typeMask;
        u16 attribute;
        f32 dist;
    };

    void checkCourseColNarrScLocal(f32 radius, const EGG::Vector3f &pos, KCLTypeMask mask,
            u32 /*unused*/);

    bool checkSphereFullPush(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f &v1,
            KCLTypeMask flags, CourseColMgr::CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
            u32 /*start*/);

    bool checkSphereCachedPartialPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
            KCLTypeMask typeMask, CourseColMgr::CollisionInfo *colInfo, KCLTypeMask *typeMaskOut,
            f32 radius, u32 start);
    bool checkSphereCachedFullPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
            KCLTypeMask typeMask, CourseColMgr::CollisionInfo *colInfo, KCLTypeMask *typeMaskOut,
            f32 radius, u32 start);

    void resetCollisionEntries(KCLTypeMask *ptr);
    void pushCollisionEntry(f32 dist, KCLTypeMask *typeMask, KCLTypeMask kclTypeBit, u16 attribute);

    const CollisionEntry *closestCollisionEntry() const;
    bool findClosestCollisionEntry(KCLTypeMask *typeMask, KCLTypeMask type);

    static CollisionDirector *CreateInstance();
    static CollisionDirector *Instance();
    static void DestroyInstance();

private:
    CollisionDirector();
    ~CollisionDirector();

    const CollisionEntry *m_closestCollisionEntry;
    std::array<CollisionEntry, COLLISION_ARR_LENGTH> m_entries;
    size_t m_collisionEntryCount;

    static CollisionDirector *s_instance;
};

} // namespace Field
