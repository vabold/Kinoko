#include "ObjectFirebar.hh"

namespace Kinoko::Field {

/// @addr{0x807678F4}
ObjectFirebar::ObjectFirebar(const System::MapdataGeoObj &params) : ObjectCollidable(params) {
    m_spokes = std::max<u32>(1, params.setting(3));
    size_t fireballCount = std::max<u32>(1, params.setting(0) * m_spokes);
    m_angSpeed = static_cast<f32>(static_cast<s16>(params.setting(1)));

    m_fireballs = owning_span<ObjectFireball *>(fireballCount);

    for (size_t i = 0; i < fireballCount; ++i) {
        m_fireballs[i] = EGG::egg_new<ObjectFireball>(params);
        m_fireballs[i]->load();

        f32 ring = 1.0f + static_cast<f32>(i / m_spokes);
        m_fireballs[i]->setDistance(
                ring * (100.0f * static_cast<f32>(static_cast<s16>(params.setting(2)))));
        m_fireballs[i]->setAngle((360.0f / m_spokes) * (i % m_spokes));
    }

    EGG::Matrix34f mat;
    mat.makeR(rot());
    m_axis = mat.base(2);
    m_axis.normalise();
    m_initDir = m_axis.cross(RotateAxisAngle(F_PI / 2.0f, EGG::Vector3f::ex, m_axis));
    m_initDir.normalise();
}

/// @addr{0x807688AC}
ObjectFirebar::~ObjectFirebar() = default;

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

    // Every fireball on the same spoke shares the same angle. For Kinoko, we can leverage this fact
    // to introduce a small performance improvement by building each spoke's rotation matrix once
    // instead of calling RotateAxisAngle() once per fireball.
    size_t spokeCount = std::min<size_t>(m_spokes, m_fireballs.size());
    for (size_t spoke = 0; spoke < spokeCount; ++spoke) {
        EGG::Matrix34f mat;
        mat.setBase(3, EGG::Vector3f::zero);
        mat.setAxisRotation((m_degAngle + m_fireballs[spoke]->angle()) * DEG2RAD, m_axis);

        for (size_t i = spoke; i < m_fireballs.size(); i += m_spokes) {
            auto *fireball = m_fireballs[i];
            fireball->setPos(pos() + mat.ps_multVector(m_initDir * fireball->distance()));
        }
    }
}

} // namespace Kinoko::Field
