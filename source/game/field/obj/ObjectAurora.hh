#pragma once

#include "game/field/obj/ObjectDrivable.hh"

namespace Field {

class ObjectAurora : public ObjectDrivable {
public:
    ObjectAurora(const System::MapdataGeoObj &params);
    ~ObjectAurora() override;

    void init() override;
    [[nodiscard]] u32 loadFlags() const override;
    void createCollision() override;
    void calcCollisionTransform() override;
    f32 getCollisionRadius() const override;

    [[nodiscard]] bool checkPointPartial(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
            KCLTypeMask flags, CollisionInfoPartial *pInfo, KCLTypeMask *pFlagsOut) override;
    [[nodiscard]] bool checkPointPartialPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
            KCLTypeMask flags, CollisionInfoPartial *pInfo, KCLTypeMask *pFlagsOut) override;
    [[nodiscard]] bool checkPointFull(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
            KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) override;
    [[nodiscard]] bool checkPointFullPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
            KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) override;
    [[nodiscard]] bool checkSpherePartial(f32 radius, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
            KCLTypeMask *pFlagsOut, u32 timeOffset) override;
    [[nodiscard]] bool checkSpherePartialPush(f32 radius, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
            KCLTypeMask *pFlagsOut, u32 timeOffset) override;
    [[nodiscard]] bool checkSphereFull(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f &v1,
            KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
            u32 timeOffset) override;
    [[nodiscard]] bool checkSphereFullPush(f32 radius, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo,
            KCLTypeMask *pFlagsOut, u32 timeOffset) override;
    [[nodiscard]] bool checkPointCachedPartial(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
            KCLTypeMask flags, CollisionInfoPartial *pInfo, KCLTypeMask *pFlagsOut) override;
    [[nodiscard]] bool checkPointCachedPartialPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
            KCLTypeMask flags, CollisionInfoPartial *pInfo, KCLTypeMask *pFlagsOut) override;
    [[nodiscard]] bool checkPointCachedFull(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
            KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) override;
    [[nodiscard]] bool checkPointCachedFullPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
            KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) override;
    [[nodiscard]] bool checkSphereCachedPartial(f32 radius, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
            KCLTypeMask *pFlagsOut, u32 timeOffset) override;
    [[nodiscard]] bool checkSphereCachedPartialPush(f32 radius, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
            KCLTypeMask *pFlagsOut, u32 timeOffset) override;
    [[nodiscard]] bool checkSphereCachedFull(f32 radius, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo,
            KCLTypeMask *pFlagsOut, u32 timeOffset) override;
    [[nodiscard]] bool checkSphereCachedFullPush(f32 radius, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo,
            KCLTypeMask *pFlagsOut, u32 timeOffset) override;

private:
    [[nodiscard]] bool checkSpherePartialImpl(f32 radius, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
            KCLTypeMask *pFlagsOut, u32 timeOffset);
    [[nodiscard]] bool checkSpherePartialPushImpl(f32 radius, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
            KCLTypeMask *pFlagsOut, u32 timeOffset);
    [[nodiscard]] bool checkSphereFullImpl(f32 radius, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo,
            KCLTypeMask *pFlagsOut, u32 timeOffset);
    [[nodiscard]] bool checkSphereFullPushImpl(f32 radius, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo,
            KCLTypeMask *pFlagsOut, u32 timeOffset);

    [[nodiscard]] bool checkSpherePartialImpl(f32 radius, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
            KCLTypeMask *pFlagsOut, u32 timeOffset, bool push);

    [[nodiscard]] bool checkSphereFullImpl(f32 radius, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo,
            KCLTypeMask *pFlagsOut, u32 timeOffset, bool push);

    [[nodiscard]] bool calcCollision(f32 radius, const EGG::Vector3f &vel, u32 time,
            EGG::Vector3f &v0, EGG::Vector3f &fnrm, f32 &dist);

    static constexpr EGG::Vector3f COLLISION_SIZE = EGG::Vector3f(2000.0f, 2000.0f, 15000.0f);
};

} // namespace Field
