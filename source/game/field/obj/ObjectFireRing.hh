#pragma once

#include "game/field/obj/ObjectFireball.hh"

namespace Kinoko::Field {

/// @brief A rotating circle of fireballs that are spaced out equally
/// @details The first param setting is the number of fireballs. The fourth param setting is the
/// radius of the circle. The second param setting is the angular speed of the fire ring. Fire rings
/// pulse inward and outward in a sinusoidal motion, with an amplitude defined by `0.1f` times param
/// setting 3.
class ObjectFireRing final : public ObjectCollidable {
public:
    ObjectFireRing(const System::MapdataGeoObj &params);
    ~ObjectFireRing() override;

    void init() override;
    void calc() override;
    void createCollision() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

private:
    owning_span<ObjectFireball *> m_fireballs; ///< Pointers to the underlying fireball objects
    const f32 m_angSpeed;                      ///< Angular speed of the fireball circle
    f32 m_currAngle;                           ///< Current rotation angle in degrees modulo 360
    EGG::Vector3f m_axis;                      ///< Axis of rotation
    EGG::Vector3f m_initDir;                   ///< Initial tangent direction
    const f32 m_pulseAmplitude;                ///< Causes the ring's radius to stretch in and out
    f32 m_phase;                               ///< Number of frames elapsed since initialization
};

} // namespace Kinoko::Field
