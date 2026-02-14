#include "ObjectFireRing.hh"

namespace Field {

/// @addr{0x80767FF4}
ObjectFireRing::ObjectFireRing(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_angSpeed(static_cast<f32>(params.setting(1))), m_phase(0.0f) {
    s32 fireballCount = std::max<s32>(1, static_cast<s32>(params.setting(0)));
    m_fireballs = std::span<ObjectFireball *>(new ObjectFireball *[fireballCount], fireballCount);

    f32 distance = 100.0f * static_cast<f32>(params.setting(3));
    f32 phase = 360.0f / static_cast<f32>(fireballCount);

    for (s32 i = 0; i < fireballCount; ++i) {
        m_fireballs[i] = new ObjectFireball(params);
        m_fireballs[i]->load();
        m_fireballs[i]->setDistance(distance);
        m_fireballs[i]->setAngle(static_cast<f32>(i) * phase);
    }

    EGG::Matrix34f mat;
    mat.makeR(m_rot);
    m_axis = mat.base(2);
    m_axis.normalise();
    m_initDir = m_axis.cross(RotateAxisAngle(F_PI / 2.0f, EGG::Vector3f::ex, m_axis));
    m_initDir.normalise();
    m_radiusScale = 0.1f * static_cast<f32>(params.setting(2));
}

/// @addr{0x8076892C}
ObjectFireRing::~ObjectFireRing() {
    delete[] m_fireballs.data();
}

/// @addr{0x807683F0}
void ObjectFireRing::init() {
    m_degAngle = 0.0f;
}

/// @addr{0x80768408}
void ObjectFireRing::calc() {
    m_phase += 1.0f;
    m_degAngle += m_angSpeed / 60.0f;

    if (m_degAngle > 360.0f) {
        m_degAngle -= 360.0f;
    } else if (m_degAngle < 0.0f) {
        m_degAngle += 360.0f;
    }

    f32 radius = m_radiusScale * EGG::Mathf::sin(m_phase * DEG2RAD);

    for (auto *&fireball : m_fireballs) {
        EGG::Vector3f dir = m_initDir * fireball->distance() * (1.0f + radius);
        fireball->setPos(
                m_pos + RotateAxisAngle((m_degAngle + fireball->angle()) * DEG2RAD, m_axis, dir));
    }
}

} // namespace Field
