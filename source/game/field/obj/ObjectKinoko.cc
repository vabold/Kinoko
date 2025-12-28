#include "ObjectKinoko.hh"

namespace Field {

/// @addr{0x8080761C}
ObjectKinoko::ObjectKinoko(const System::MapdataGeoObj &params)
    : ObjectKCL(params), m_objPos(m_pos), m_objRot(m_rot) {
    m_type = static_cast<KinokoType>(params.setting(0));

    m_restFrame = 0;
    m_pulseFrame = 1; // (-1*-1) % PULSE_DURATION;
    m_pulseFalloff = 0.1f;
}

/// @adrr{0x80807A54}
ObjectKinoko::~ObjectKinoko() = default;

/// @addr{0x8080782C}
void ObjectKinoko::calc() {
    constexpr s16 REST_DURATION = 10;
    constexpr s16 PULSE_DURATION = 40;
    constexpr f32 PULSE_SCALE = 0.0008f;
    constexpr f32 PULSE_FREQ = 6.0f * F_PI / 40.0f;

    if (m_restFrame == 0) {
        ++m_pulseFrame;
    }
    if (m_pulseFrame == PULSE_DURATION) {
        ++m_restFrame;
    }
    if (m_restFrame > REST_DURATION) {
        m_restFrame = 0;
    }
    if (m_pulseFrame > PULSE_DURATION) {
        m_pulseFrame = 0;
    }

    m_pulseFalloff = PULSE_SCALE * static_cast<f32>(PULSE_DURATION - m_pulseFrame);
    m_flags.setBit(eFlags::Scale);
    m_scale.set(
            m_pulseFalloff * EGG::Mathf::sin(PULSE_FREQ * static_cast<f32>(m_pulseFrame)) + 1.0f);

    calcOscillation();
}

/// @addr{0x80807950}
ObjectKinokoUd::ObjectKinokoUd(const System::MapdataGeoObj &params) : ObjectKinoko(params) {
    m_waitFrame = 0;
    m_oscFrame = params.setting(3);
    m_waitDuration = params.setting(4);
    m_amplitude = params.setting(1);
    m_period = std::max<s16>(params.setting(2), 2);
    m_angFreq = F_TAU / static_cast<f32>(m_period);
}

/// @addr{0x80807E1C}
ObjectKinokoUd::~ObjectKinokoUd() = default;

/// @addr{0x80807A54}
void ObjectKinokoUd::calcOscillation() {
    m_flags.setBit(eFlags::Position);
    m_pos.y = m_objPos.y +
            static_cast<f32>(m_amplitude) *
                    (EGG::Mathf::cos(m_angFreq * static_cast<f32>(m_oscFrame)) + 1.0f) * 0.5f;

    if (m_waitFrame == 0) {
        ++m_oscFrame;
    }
    if (m_oscFrame == (m_period / 2)) {
        ++m_waitFrame;
    }
    if (m_waitFrame > m_waitDuration) {
        m_waitFrame = 0;
    }
    if (m_oscFrame > m_period) {
        m_oscFrame = 0;
    }
}

/// @addr{0x80807B7C}
ObjectKinokoBend::ObjectKinokoBend(const System::MapdataGeoObj &params) : ObjectKinoko(params) {
    m_currentFrame = params.setting(3);
    m_amplitude = static_cast<f32>(params.setting(1)) * DEG2RAD;
    m_period = std::max<s16>(params.setting(2), 2);
    m_angFreq = F_TAU / static_cast<f32>(m_period);
}

/// @addr{0x80807DB4}
ObjectKinokoBend::~ObjectKinokoBend() = default;

/// @addr{0x80807C98}
void ObjectKinokoBend::calcOscillation() {
    const f32 s = EGG::Mathf::sin(m_angFreq * static_cast<f32>(m_currentFrame));
    EGG::Vector3f rot = m_objRot + (EGG::Vector3f::ez * s) * m_amplitude;

    calcTransform();

    m_flags.setBit(eFlags::Rotation);
    m_rot = m_transform.multVector33(rot);

    if (++m_currentFrame >= m_period) {
        m_currentFrame = 0;
    }
}

ObjectKinokoNm::ObjectKinokoNm(const System::MapdataGeoObj &params) : ObjectKCL(params) {
    m_type = static_cast<KinokoType>(params.setting(0));
}

/// @addr{0x80827A9C}
ObjectKinokoNm::~ObjectKinokoNm() = default;

} // namespace Field
