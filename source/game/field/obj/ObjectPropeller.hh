#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

class ObjectCollisionCylinder;

/// @brief Rotating Koopa shell lasers on Koopa Cape
/// @details The shell itself has cylindrical collision, and each of the three blades has its own
/// cylindrical collision object. The blades rotate around the shell's center point at the angular
/// speed defined by param setting 1.
class ObjectPropeller final : public ObjectCollidable {
public:
    ObjectPropeller(const System::MapdataGeoObj &params);
    ~ObjectPropeller() override;

    void init() override;
    void calc() override;
    void createCollision() override;
    void calcCollisionTransform() override;
    [[nodiscard]] LoadFlags loadFlags() const override;
    [[nodiscard]] f32 getCollisionRadius() const override;

    bool checkCollision(ObjectCollisionBase *lhs, EGG::Vector3f &dist) override;

private:
    void initAngVel();
    void calcAngleAndRot();

    f32 m_angVel;             ///< Angular speed of the propeller in degrees per frame
    f32 m_angle;              ///< Accumulated rotation angle of the propeller in degrees
    EGG::Vector3f m_axis;     ///< Forward direction, which is the axis of rotation for the blades
    EGG::Matrix34f m_initMat; ///< Initial rotation/translation matrix
    EGG::Matrix34f m_curRot;  ///< Current rotation matrix of the propeller
    std::array<ObjectCollisionCylinder *, 3> m_blades; ///< The collision objects for each blade
};

} // namespace Kinoko::Field
