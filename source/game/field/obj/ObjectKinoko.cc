#include "ObjectKinoko.hh"

#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

/// @addr{0x8080761C}
/// @copybrief ObjectKCL::ObjectKCL(const System::MapdataGeoObj &)
/// @details Initializes @ref m_type based on param setting 1. Caches the mushroom's initial
/// position and rotation to @ref m_initPos and @ref m_initRot respectively. Initializes @ref
/// m_restFrame to 0, @ref m_pulseFrame to 1, and @ref m_pulseAmplitude to `0.1f`.
ObjectKinoko::ObjectKinoko(const System::MapdataGeoObj &params)
    : ObjectKCL(params),
      m_type(static_cast<KinokoType>(params.setting(0))),
      m_initPos(pos()),
      m_initRot(rot()) {
    m_restFrame = 0;
    m_pulseFrame = 1; // (-1*-1) % PULSE_DURATION;
    m_pulseAmplitude = 0.1f;
}

/// @addr{0x80807A14}
/// @brief Default virtual destructor
ObjectKinoko::~ObjectKinoko() = default;

/// @addr{0x8080782C}
/// @copybrief ObjectBase::calc()
/// @details Pulses the mushroom's scale using a sine wave whose @ref m_pulseAmplitude decays
/// linearly from `0.032f` to `0.0f` over a `40` frame duration. Each pulse is followed by a `10`
/// frame rest where the scale stays at `1.0f`.
///
/// If the mushroom is not idle, increments @ref m_pulseFrame. Once the pulse ends, increments
/// @ref m_restFrame. If the pulse or rest frames exceed their respective durations, they are
/// reset to 0. Computes @ref m_pulseAmplitude based on the updated @ref m_pulseFrame and sets the
/// scale accordingly. Finally, calls @ref calcOscillation() to update the mushroom's position and
/// orientation.
/// @note Because of how the counters reset, the scale is `1.0f` for `12` frames per cycle rather
/// than the expected `10` frames, giving a full cycle of 51 frames.
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

    m_pulseAmplitude = PULSE_SCALE * static_cast<f32>(PULSE_DURATION - m_pulseFrame);
    setScale(
            m_pulseAmplitude * EGG::Mathf::sin(PULSE_FREQ * static_cast<f32>(m_pulseFrame)) + 1.0f);
    calcOscillation();
}

/// @addr{0x80807DAC}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectKinoko::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x80807D8C}
/// @copybrief ObjectBase::getKclName()
/// @return The model name of the mushroom, `kinoko_r` for light, `kinoko_d_r` for dark.
const char *ObjectKinoko::getKclName() const {
    return m_type == KinokoType::Light ? "kinoko_r" : "kinoko_d_r";
}

/// @addr{0x80807950}
/// @copydoc ObjectKinoko::ObjectKinoko(const System::MapdataGeoObj &)
/// @details Initializes @ref m_period to param setting 3, with a minimum value of `2`. Sets @ref
/// m_waitDuration to param setting 5 and @ref m_amplitude to param setting 2. Computes @ref
/// m_angFreq as @ref F_TAU divided by @ref m_period. Finally, initializes @ref m_waitFrame to zero
/// and sets @ref m_oscFrame to param setting 4.
ObjectKinokoUd::ObjectKinokoUd(const System::MapdataGeoObj &params)
    : ObjectKinoko(params),
      m_period(std::max<u16>(params.setting(2), 2)),
      m_waitDuration(params.setting(4)),
      m_amplitude(params.setting(1)),
      m_angFreq(F_TAU / static_cast<f32>(m_period)) {
    m_waitFrame = 0;
    m_oscFrame = params.setting(3);
}

/// @addr{0x80807E1C}
/// @brief Default virtual destructor
ObjectKinokoUd::~ObjectKinokoUd() = default;

/// @addr{0x80807DFC}
/// @copybrief ObjectBase::getKclName()
/// @return The model name of the mushroom, `kinoko_r`
const char *ObjectKinokoUd::getKclName() const {
    return "kinoko_r";
}

/// @addr{0x80807DF8}
/// @copybrief ObjectKCL::calcScale()
/// @param timeOffset Optional time offset (unused)
/// @details No-op because these mushrooms maintain constant scale.
void ObjectKinokoUd::calcScale(u32 /*timeOffset*/) {}

/// @addr{0x80807A54}
/// @copybrief ObjectKinoko::calcOscillation()
/// @details Sinusoidal oscillation with a pause at the bottom of the cycle.
void ObjectKinokoUd::calcOscillation() {
    f32 posY = m_initPos.y +
            static_cast<f32>(m_amplitude) *
                    (EGG::Mathf::cos(m_angFreq * static_cast<f32>(m_oscFrame)) + 1.0f) * 0.5f;
    setPos(EGG::Vector3f(pos().x, posY, pos().z));

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
/// @copydoc ObjectKinoko::ObjectKinoko(const System::MapdataGeoObj &)
ObjectKinokoBend::ObjectKinokoBend(const System::MapdataGeoObj &params)
    : ObjectKinoko(params),
      m_period(std::max<u16>(params.setting(2), 2)),
      m_amplitude(static_cast<f32>(params.setting(1)) * DEG2RAD),
      m_angFreq(F_TAU / static_cast<f32>(m_period)) {
    m_currentFrame = params.setting(3);
}

/// @addr{0x80807DB4}
/// @brief Default virtual destructor
ObjectKinokoBend::~ObjectKinokoBend() = default;

/// @addr{0x80807D88}
/// @copybrief ObjectKCL::calcScale()
/// @param timeOffset Optional time offset (unused)
/// @details No-op because these mushrooms maintain constant scale.
void ObjectKinokoBend::calcScale(u32 /*timeOffset*/) {}

/// @addr{0x80807C98}
/// @copybrief ObjectKinoko::calcOscillation()
/// @details Applies an oscillating rotation that bends the mushroom.
void ObjectKinokoBend::calcOscillation() {
    const f32 s = EGG::Mathf::sin(m_angFreq * static_cast<f32>(m_currentFrame));
    EGG::Vector3f rot = m_initRot + (EGG::Vector3f::ez * s) * m_amplitude;

    calcTransform();
    setRot(transform().multVector33(rot));

    if (++m_currentFrame >= m_period) {
        m_currentFrame = 0;
    }
}

/// @addr{Inlined at 0x80821FE8}
/// @copybrief ObjectKCL::ObjectKCL(const System::MapdataGeoObj &)
/// @details Initializes @ref m_type based on param setting 1.
ObjectKinokoNm::ObjectKinokoNm(const System::MapdataGeoObj &params)
    : ObjectKCL(params), m_type(static_cast<KinokoType>(params.setting(0))) {}

/// @addr{0x80827A9C}
/// @brief Default virtual destructor
ObjectKinokoNm::~ObjectKinokoNm() = default;

/// @addr{0x80827A74}
/// @copybrief ObjectBase::getKclName()
/// @return The model name of the mushroom, `kinoko_g` for light, `kinoko_d_g` for dark.
const char *ObjectKinokoNm::getKclName() const {
    return m_type == KinokoType::Light ? "kinoko_g" : "kinoko_d_g";
}

} // namespace Kinoko::Field
