#pragma once

#include "game/field/obj/ObjectBelt.hh"

namespace Field {

/// @brief The curved conveyer belt at the end of the factory on Toad's Factory.
/// @details This conveyer switches direction twice.
class ObjectBeltCurveA final : public ObjectBelt {
public:
    ObjectBeltCurveA(const System::MapdataGeoObj &params);
    ~ObjectBeltCurveA() override;

    [[nodiscard]] EGG::Vector3f calcRoadVelocity(u32 variant, const EGG::Vector3f &pos,
            s32 timeOffset) const override;

    /// @addr{0x807FCCA4}
    [[nodiscard]] bool isMoving(u32 variant, const EGG::Vector3f & /*pos*/) const override {
        return variant == 4 || variant == 5;
    }

private:
    [[nodiscard]] f32 calcDirSwitchVelocity(u32 t) const;

    bool m_startForward;
    s32 m_dirChange1Frame;
    s32 m_dirChange2Frame;
    EGG::Matrix34f m_initMat;
};

} // namespace Field
