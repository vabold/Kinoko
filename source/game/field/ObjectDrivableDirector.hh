#pragma once

#include "game/field/KCollisionTypes.hh"
#include "game/field/obj/ObjectCollidable.hh"
#include "game/field/obj/ObjectDrivable.hh"

#include <egg/core/Disposer.hh>

#include <vector>

namespace Field {

class ObjectDrivableDirector : public EGG::Disposer {
public:
    void init();
    void calc();
    void addObject(ObjectDrivable *obj);

    bool checkSphereFull(f32 radius, f32 scale, const EGG::Vector3f &v0, const EGG::Vector3f &v1,
            KCLTypeMask mask, CollisionInfo *colInfo, KCLTypeMask *maskOut,
            u32 start);
    bool checkSphereFullPush(f32 radius, f32 scale, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask mask, CollisionInfo *colInfo,
            KCLTypeMask *maskOut, u32 start);
    bool checkSphereCachedPartial(f32 radius, f32 scale, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask mask, CollisionInfo *colInfo,
            KCLTypeMask *maskOut, u32 start);
    bool checkSphereCachedPartialPush(f32 radius, f32 scale, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask mask, CollisionInfo *colInfo,
            KCLTypeMask *maskOut, u32 start);
    bool checkSphereCachedFullPush(f32 radius, f32 scale, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask mask, CollisionInfo *colInfo,
            KCLTypeMask *maskOut, u32 start);
    void colNarScLocal(f32 scale, f32 radius, const EGG::Vector3f &pos, KCLTypeMask mask,
            bool bScaledUp);

    static ObjectDrivableDirector *CreateInstance();
    static void DestroyInstance();
    static ObjectDrivableDirector *Instance();

private:
    ObjectDrivableDirector();
    ~ObjectDrivableDirector() override;

    std::vector<ObjectDrivable *> m_objects; ///< All objects live here
    std::vector<ObjectBase *> m_calcObjects; ///< Objects needing calc() live here too.

    static ObjectDrivableDirector *s_instance;
};

} // namespace Field
