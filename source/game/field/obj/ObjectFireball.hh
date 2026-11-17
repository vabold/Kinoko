#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

/// @brief An individual fireball within an @ref ObjectFirebar or @ref ObjectFireRing
/// @details The owning object sets the fireball's distance from the center of the owning object and
/// sets the angle of the fireball around the owning object's axis of rotation.
class ObjectFireball final : public ObjectCollidable {
public:
    /// @addr{0x80768650}
    /// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
    /// @param params The parameters used to initialize the object
    ObjectFireball(const System::MapdataGeoObj &params) : ObjectCollidable(params) {}

    /// @addr{0x807689AC}
    /// @brief Default virtual destructor
    ~ObjectFireball() = default;

    /// @addr{0x80768728}
    /// @copybrief ObjectBase::loadFlags()
    /// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
    [[nodiscard]] LoadFlags loadFlags() const override {
        return LoadFlags(eLoadFlags::Calc);
    }

    /// @addr{0x8076871C}
    /// @copybrief ObjectBase::getKclName()
    /// @return The model name of the fireball, `fireBPlane`
    [[nodiscard]] const char *getKclName() const override {
        return "fireBPlane";
    }

    /// @beginSetters

    /// @brief Sets the distance from the center of the parent object
    /// @param dist The distance from the center of the parent object
    void setDistance(f32 dist) {
        m_distance = dist;
    }

    /// @brief Sets the angle of the fireball around the axis of rotation of the parent object
    /// @param angle The fireball's angle in degrees around the parent object's axis of rotation
    void setAngle(f32 angle) {
        m_angle = angle;
    }

    /// @endSetters

    /// @beginGetters

    /// @brief Gets the distance from the center of the parent object
    /// @return The distance from the center of the parent object, @ref m_distance
    [[nodiscard]] f32 distance() const {
        return m_distance;
    }

    /// @brief Gets the angle of the fireball around the axis of rotation of the parent object
    /// @return The fireball's angle in degrees around the parent's axis of rotation, @ref m_angle
    [[nodiscard]] f32 angle() const {
        return m_angle;
    }

    /// @endGetters

private:
    f32 m_distance; ///< Distance from the center of the parent object
    f32 m_angle;    ///< Angle of the fireball about the axis of rotation in degrees
};

} // namespace Kinoko::Field
