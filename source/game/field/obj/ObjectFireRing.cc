#include "ObjectFireRing.hh"

#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

/// @addr{0x80767FF4}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Constructs and loads the required number of @ref ObjectFireball objects based on
/// param setting 1, sets @ref m_angSpeed based on param setting 2, and @ref m_pulseAmplitude
/// based on param setting 3 times `0.1f`. For each @ref ObjectFireball, computes and caches the
/// distance from the fire ring and angle about the ring's rotation axis. Finally, computes the
/// axis of rotation for the fire ring and its initial tangent direction.
ObjectFireRing::ObjectFireRing(const System::MapdataGeoObj &params)
    : ObjectCollidable(params),
      m_fireballs(std::max<u32>(1, params.setting(0))),
      m_angSpeed(static_cast<f32>(static_cast<s16>(params.setting(1)))),
      m_pulseAmplitude(0.1f * static_cast<f32>(params.setting(2))),
      m_phase(0.0f) {
    f32 distance = 100.0f * static_cast<f32>(params.setting(3));

    for (size_t i = 0; i < m_fireballs.size(); ++i) {
        m_fireballs[i] = EGG::egg_new<ObjectFireball>(params);
        m_fireballs[i]->load();
        m_fireballs[i]->setDistance(distance);
        m_fireballs[i]->setAngle(static_cast<f32>(i) * (360.0f / m_fireballs.size()));
    }

    EGG::Matrix34f mat;
    mat.makeR(rot());
    m_axis = mat.base(2);
    m_axis.normalise();
    m_initDir = m_axis.cross(RotateAxisAngle(HALF_PI, EGG::Vector3f::ex, m_axis));
    m_initDir.normalise();
}

/// @addr{0x8076892C}
/// @brief Default virtual destructor
ObjectFireRing::~ObjectFireRing() = default;

/// @addr{0x807683F0}
/// @copybrief ObjectBase::init()
/// @details Resets the fire ring's angle to zero.
void ObjectFireRing::init() {
    m_currAngle = 0.0f;
}

/// @addr{0x80768408}
/// @copybrief ObjectBase::calc()
/// @details Each frame, increments the phase of the fire ring's pulse by `1.0f`. Rotates the
/// ring based on @ref m_angSpeed. Updates the positions of all underlying fireballs accordingly,
/// factoring in the fire ring's sinusoidal pulsing.
void ObjectFireRing::calc() {
    m_phase += 1.0f;
    m_currAngle += m_angSpeed / 60.0f;

    if (m_currAngle > 360.0f) {
        m_currAngle -= 360.0f;
    } else if (m_currAngle < 0.0f) {
        m_currAngle += 360.0f;
    }

    f32 radius = m_pulseAmplitude * EGG::Mathf::sin(m_phase * DEG2RAD);

    for (auto *&fireball : m_fireballs) {
        EGG::Vector3f dir = m_initDir * fireball->distance() * (1.0f + radius);
        fireball->setPos(
                pos() + RotateAxisAngle((m_currAngle + fireball->angle()) * DEG2RAD, m_axis, dir));
    }
}

/// @addr{0x80768734}
/// @copybrief ObjectBase::createCollision()
/// @details Does not create any collision, since this is effectively a manager class
void ObjectFireRing::createCollision() {}

/// @addr{0x80768740}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectFireRing::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

} // namespace Kinoko::Field
