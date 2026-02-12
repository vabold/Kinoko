#pragma once

#include "game/field/obj/ObjectKCL.hh"

namespace Field {

class ObjectSandcone : public ObjectKCL {
public:
    ObjectSandcone(const System::MapdataGeoObj &params);
    ~ObjectSandcone() override;

    void init() override;

    /// @addr{0x806873Bc}
    void calc() override {
        setTransform(getUpdatedMatrix(0));
    }

    /// @addr{0x80687E14}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    [[nodiscard]] const EGG::Matrix34f &getUpdatedMatrix(u32 timeOffset) override;
    [[nodiscard]] bool checkCollision(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;
    [[nodiscard]] bool checkCollisionCached(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;

private:
    f32 m_flowRate;
    f32 m_finalHeightDelta;
    u16 m_startFrame;
    u16 m_duration;
    EGG::Matrix34f m_baseMtx;
    EGG::Matrix34f m_currentMtx;
    EGG::Vector3f m_finalPos; ///< Not in base game. Stores position of fully poured sandcone.
};

} // namespace Field
