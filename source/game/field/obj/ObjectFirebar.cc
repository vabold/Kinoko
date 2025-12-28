#include "ObjectFirebar.hh"

namespace Field {

/// @addr{0x807678F4}
ObjectFirebar::ObjectFirebar(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_angSpeed(static_cast<f32>(params.setting(1))) {
    s32 spokes = std::max<s32>(1, static_cast<s32>(params.setting(3)));
    s32 fireballCount = std::max<s32>(1, static_cast<s32>(params.setting(0)) * spokes);
    m_fireballs = std::span<ObjectFireball *>(new ObjectFireball *[fireballCount], fireballCount);

    f32 distScalar = 100.0f * static_cast<f32>(params.setting(2));
    f32 phase = 360.0f / spokes;

    for (s32 i = 0; i < fireballCount; ++i) {
        m_fireballs[i] = new ObjectFireball(params);
        m_fireballs[i]->load();

        f32 ring = 1.0f + static_cast<f32>(i / spokes);
        m_fireballs[i]->setDistance(ring * distScalar);
        m_fireballs[i]->setAngle(phase * (i % spokes));
    }

    EGG::Matrix34f mat;
    mat.makeR(m_rot);
    m_axis = mat.base(2);
    m_axis.normalise();
    m_initDir = m_axis.cross(RotateAxisAngle(F_PI / 2.0f, EGG::Vector3f::ex, m_axis));
    m_initDir.normalise();
}

/// @addr{0x807688AC}
ObjectFirebar::~ObjectFirebar() {
    delete[] m_fireballs.data();
}

/// @addr{0x80767DEC}
void ObjectFirebar::init() {
    m_degAngle = 0.0f;
}

/// @addr{0x80767E04}
void ObjectFirebar::calc() {
    m_degAngle += m_angSpeed / 60.0f;

    if (m_degAngle > 360.0f) {
        m_degAngle -= 360.0f;
    } else if (m_degAngle < 0.0f) {
        m_degAngle += 360.0f;
    }

    for (auto *&fireball : m_fireballs) {
        EGG::Vector3f dir = m_initDir * fireball->distance();
        fireball->setPos(
                m_pos + RotateAxisAngle((m_degAngle + fireball->angle()) * DEG2RAD, m_axis, dir));
    }
}

} // namespace Field
