#pragma once

#include "game/field/obj/ObjectKCL.hh"

namespace Field {

class ObjectWLWallGC : public ObjectKCL {
public:
    ObjectWLWallGC(const System::MapdataGeoObj &params);
    ~ObjectWLWallGC() override;

    void init() override;
    void calc() override;

    /// @addr{0x8086C640}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    [[nodiscard]] const EGG::Matrix34f &getUpdatedMatrix(u32 timeOffset) override;

    /// @addr{0x8086C648}
    [[nodiscard]] f32 colRadiusAdditionalLength() const override {
        return (m_currPos - m_posOffset).length();
    }

    [[nodiscard]] bool checkCollision(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;
    [[nodiscard]] bool checkCollisionCached(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;

private:
    EGG::Vector3f interpolate(f32 t, const EGG::Vector3f &v0, const EGG::Vector3f &v1) const;

    s32 m_extendedDuration;
    u32 m_moveDuration;
    s32 m_startFrame;
    s32 m_hiddenDuration;
    s32 m_extendedFrame;
    s32 m_retractingFrame;
    s32 m_cycleDuration;
    EGG::Vector3f m_currPos;
    EGG::Vector3f m_posOffset;
    EGG::Matrix34f m_currTransform;
};

} // namespace Field
