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
        return (m_initialPos - m_targetPos).length();
    }

    [[nodiscard]] bool checkCollision(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;
    [[nodiscard]] bool checkCollisionCached(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;

private:
    /// @addr{0x8086BF08}
    [[nodiscard]] u32 cycleFrame(s32 t) const {
        u32 time = t < m_startFrame ? 0 : t - m_startFrame;
        return time % m_cycleDuration;
    }

    u32 m_moveDuration;
    const s32 m_startFrame;
    s32 m_hiddenDuration;
    s32 m_extendedFrame;
    s32 m_retractingFrame;
    s32 m_cycleDuration;
    EGG::Vector3f m_initialPos;
    EGG::Vector3f m_targetPos;
    EGG::Matrix34f m_currTransform;
};

} // namespace Field
