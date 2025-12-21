#pragma once

#include "game/field/obj/ObjectDrivable.hh"

namespace Field {

/// @brief The base class for a conveyer belt which induces road velocity.
class ObjectBelt : public ObjectDrivable {
public:
    ObjectBelt(const System::MapdataGeoObj &params);
    ~ObjectBelt() override;

    /// @addr{0x807FD79C}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x807FD798}
    void createCollision() override {}

    /// @addr{0x807FD794}
    void calcCollisionTransform() override {}

    /// @addr{0x807FD784}
    [[nodiscard]] f32 getCollisionRadius() const override {
        return 25000.0f;
    }

    /// @addr{0x807FD750}
    [[nodiscard]] bool checkPointPartial(const EGG::Vector3f & /*pos*/,
            const EGG::Vector3f & /*prevPos*/, KCLTypeMask /*mask*/,
            CollisionInfoPartial * /*info*/, KCLTypeMask * /*maskOut*/) override {
        return false;
    }

    /// @addr{0x807FD758}
    [[nodiscard]] bool checkPointPartialPush(const EGG::Vector3f & /*pos*/,
            const EGG::Vector3f & /*prevPos*/, KCLTypeMask /*mask*/,
            CollisionInfoPartial * /*info*/, KCLTypeMask * /*maskOut*/) override {
        return false;
    }

    /// @addr{0x807FD760}
    [[nodiscard]] bool checkPointFull(const EGG::Vector3f & /*pos*/,
            const EGG::Vector3f & /*prevPos*/, KCLTypeMask /*mask*/, CollisionInfo * /*info*/,
            KCLTypeMask * /*maskOut*/) override {
        return false;
    }

    /// @addr{0x807FD768}
    [[nodiscard]] bool checkPointFullPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
            KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) override {
        return calcCollision(pos, prevPos, mask, info, maskOut, 0);
    }

    /// @addr{0x807FD728}
    [[nodiscard]] bool checkSpherePartial(f32 /*radius*/, const EGG::Vector3f & /*pos*/,
            const EGG::Vector3f & /*prevPos*/, KCLTypeMask /*mask*/,
            CollisionInfoPartial * /*info*/, KCLTypeMask * /*maskOut*/,
            u32 /*timeOffset*/) override {
        return false;
    }

    /// @addr{0x807FD730}
    [[nodiscard]] bool checkSpherePartialPush(f32 /*radius*/, const EGG::Vector3f & /*pos*/,
            const EGG::Vector3f & /*prevPos*/, KCLTypeMask /*mask*/,
            CollisionInfoPartial * /*info*/, KCLTypeMask * /*maskOut*/,
            u32 /*timeOffset*/) override {
        return false;
    }

    /// @addr{0x807FD738}
    [[nodiscard]] bool checkSphereFull(f32 /*radius*/, const EGG::Vector3f & /*pos*/,
            const EGG::Vector3f & /*prevPos*/, KCLTypeMask /*mask*/, CollisionInfo * /*info*/,
            KCLTypeMask * /*maskOut*/, u32 /*timeOffset*/) override {
        return false;
    }

    /// @addr{0x807FD740}
    [[nodiscard]] bool checkSphereFullPush(f32 /*radius*/, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) override {
        return calcCollision(pos, prevPos, mask, info, maskOut, timeOffset);
    }

    /// @addr{0x807FD6F4}
    [[nodiscard]] bool checkPointCachedPartial(const EGG::Vector3f & /*pos*/,
            const EGG::Vector3f & /*prevPos*/, KCLTypeMask /*mask*/,
            CollisionInfoPartial * /*info*/, KCLTypeMask * /*maskOut*/) override {
        return false;
    }

    /// @addr{0x807FD6FC}
    [[nodiscard]] bool checkPointCachedPartialPush(const EGG::Vector3f & /*pos*/,
            const EGG::Vector3f & /*prevPos*/, KCLTypeMask /*mask*/,
            CollisionInfoPartial * /*info*/, KCLTypeMask * /*maskOut*/) override {
        return false;
    }

    /// @addr{0x807FD704}
    [[nodiscard]] bool checkPointCachedFull(const EGG::Vector3f & /*pos*/,
            const EGG::Vector3f & /*prevPos*/, KCLTypeMask /*mask*/, CollisionInfo * /*info*/,
            KCLTypeMask * /*maskOut*/) override {
        return false;
    }

    /// @addr{0x807FD70C}
    [[nodiscard]] bool checkPointCachedFullPush(const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut) override {
        return calcCollision(pos, prevPos, mask, info, maskOut, 0);
    }

    /// @addr{0x807FD6CC}
    [[nodiscard]] bool checkSphereCachedPartial(f32 /*radius*/, const EGG::Vector3f & /*pos*/,
            const EGG::Vector3f & /*prevPos*/, KCLTypeMask /*mask*/,
            CollisionInfoPartial * /*info*/, KCLTypeMask * /*maskOut*/,
            u32 /*timeOffset*/) override {
        return false;
    }

    /// @addr{0x807FD6D4}
    [[nodiscard]] bool checkSphereCachedPartialPush(f32 /*radius*/, const EGG::Vector3f & /*pos*/,
            const EGG::Vector3f & /*prevPos*/, KCLTypeMask /*mask*/,
            CollisionInfoPartial * /*info*/, KCLTypeMask * /*maskOut*/,
            u32 /*timeOffset*/) override {
        return false;
    }

    /// @addr{0x807FD6DC}
    [[nodiscard]] bool checkSphereCachedFull(f32 /*radius*/, const EGG::Vector3f & /*pos*/,
            const EGG::Vector3f & /*prevPos*/, KCLTypeMask /*mask*/, CollisionInfo * /*info*/,
            KCLTypeMask * /*maskOut*/, u32 /*timeOffset*/) override {
        return false;
    }

    /// @addr{0x807FD6E4}
    [[nodiscard]] bool checkSphereCachedFullPush(f32 /*radius*/, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) override {
        return calcCollision(pos, prevPos, mask, info, maskOut, timeOffset);
    }

    [[nodiscard]] virtual EGG::Vector3f calcRoadVelocity(u32 variant, const EGG::Vector3f &pos,
            s32 timeOffset) const = 0;

    [[nodiscard]] virtual bool isMoving(u32 variant, const EGG::Vector3f &pos) const = 0;

protected:
    f32 m_roadVel;

private:
    [[nodiscard]] virtual bool calcCollision(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
            KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut, u32 timeOffset);
};

} // namespace Field
