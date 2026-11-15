#pragma once

#include "game/field/obj/ObjectBelt.hh"

namespace Kinoko::Field {

/// @brief The set of curved conveyor belts at the end of the factory on Toad's Factory
/// @details This conveyor switches direction twice.
class ObjectBeltCurveA final : public ObjectBelt {
public:
    ObjectBeltCurveA(const System::MapdataGeoObj &params);
    ~ObjectBeltCurveA() override;

private:
    [[nodiscard]] EGG::Vector3f calcRoadVelocity(u32 variant, const EGG::Vector3f &pos,
            u32 t) const override;
    [[nodiscard]] bool isMoving(u32 variant) const override;

    [[nodiscard]] f32 calcAngularSpeed(u32 t) const;
    [[nodiscard]] bool isMovingForward(u32 t) const;

    const bool m_startForward;   ///< Whether the belts start moving forward or backward
    const u16 m_dirChange1Frame; ///< Frame of first direction change
    const u16 m_dirChange2Frame; ///< Frame of second direction change
    EGG::Matrix34f m_initRt;     ///< The initial rotation matrix
};

} // namespace Kinoko::Field
