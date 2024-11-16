#pragma once

#include "game/field/obj/ObjectDrivable.hh"

namespace Field {

class ObjectTuribashi final : public ObjectDrivable {
public:
    ObjectTuribashi(const System::MapdataGeoObj &params);
    ~ObjectTuribashi() override;

    /// @addr{0x80805AD8}
    void init() override {}

    /// @addr{0x80805C24}
    void calc() override {}

    /// @addr{0x8080650C}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x80806508}
    void createCollision() override {}

    /// @addr{0x80806504}
    void calcCollisionTransform() override {}

    /// @addr{0x808064E8}
    [[nodiscard]] f32 getCollisionRadius() const override {
        return RADIUS + 100.0f;
    }

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

    template <typename T>
        requires std::is_same_v<T, CollisionInfo> || std::is_same_v<T, CollisionInfoPartial>
    [[nodiscard]] bool checkSphereImpl(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f &v1,
            KCLTypeMask flags, T *pInfo, KCLTypeMask *pFlagsOut, u32 timeOffset, bool push);

    static constexpr f32 RADIUS = 7243.3198f; ///< Half of the bridge's length along the z-axis.
};

} // namespace Field
