#pragma once

#include "game/field/obj/ObjectCollidable.hh"
#include "game/field/obj/ObjectDrivable.hh"
#include "game/field/obj/ObjectObakeManager.hh"

#include <vector>

class SavestateManager;

namespace Field {

class ObjectDrivableDirector : EGG::Disposer {
    friend class ::SavestateManager;

public:
    void init();
    void calc();
    void addObject(ObjectDrivable *obj);
    void createObakeManager(const System::MapdataGeoObj &params);

    [[nodiscard]] bool checkSpherePartial(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
            KCLTypeMask *maskOut, u32 timeOffset);
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
    void colNarScLocal(f32 radius, const EGG::Vector3f &pos, KCLTypeMask mask, u32 timeOffset);

    [[nodiscard]] ObjectObakeManager *obakeManager() const {
        return m_obakeManager;
    }

    static ObjectDrivableDirector *CreateInstance();
    static void DestroyInstance();

    [[nodiscard]] static ObjectDrivableDirector *Instance() {
        return s_instance;
    }

private:
    ObjectDrivableDirector();
    ~ObjectDrivableDirector() override;

    std::vector<ObjectDrivable *> m_objects;     ///< All objects live here
    std::vector<ObjectDrivable *> m_calcObjects; ///< Objects needing calc() live here too.
    ObjectObakeManager *m_obakeManager;          ///< Manages rGV2 blocks and spatial indexing.

    static ObjectDrivableDirector *s_instance;
};

} // namespace Field
