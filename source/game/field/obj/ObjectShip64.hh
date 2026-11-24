#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

class ObjectCollisionCylinder;

/// @brief Represents the ship on N64 DK's Jungle Parkway
/// @details Follows a rail path, interpolating its forward direction along the way to make turns
/// smooth. This class also creates a cylindrical collision object to represent the red paddle
/// wheel's collision shape.
class ObjectShip64 final : public ObjectCollidable {
public:
    ObjectShip64(const System::MapdataGeoObj &params);
    ~ObjectShip64() override;

    void init() override;
    void calc() override;
    void createCollision() override;
    void calcCollisionTransform() override;
    [[nodiscard]] LoadFlags loadFlags() const override;
    [[nodiscard]] const char *getResources() const override;
    [[nodiscard]] const char *getKclName() const override;

    bool checkCollision(ObjectCollisionBase *lhs, EGG::Vector3f &dist) override;

private:
    void calcTangent();

    EGG::Vector3f m_tangent;                         ///< Smoothed forward direction
    ObjectCollisionCylinder *m_paddleWheelCollision; ///< Collision of the rotating red wheel
};

} // namespace Kinoko::Field
