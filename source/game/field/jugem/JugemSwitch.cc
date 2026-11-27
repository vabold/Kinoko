#include "JugemSwitch.hh"

#include "game/kart/KartObject.hh"
#include "game/kart/KartObjectManager.hh"

#include "game/system/RaceManager.hh"

namespace Kinoko::Field {

/// @addr{0x807259EC}
/// @brief Constructor
/// @details Initializes @ref m_isOn to `false`.
JugemSwitch::JugemSwitch() : m_isOn(false) {}

/// @addr{0x80725A0C}
/// @brief Default virtual destructor
JugemSwitch::~JugemSwitch() = default;

/// @addr{0x80725A4C}
/// @brief Initializes the internal state of the switch
void JugemSwitch::init() {
    m_isOn = false;
}

/// @addr{0x80725C1C}
/// @brief Default constructor
JugemSwitchReverse::JugemSwitchReverse() = default;

/// @addr{0x80725C3C}
/// @brief Default virtual destructor
JugemSwitchReverse::~JugemSwitchReverse() = default;

/// @addr{0x80725C7C}
/// @copybrief JugemSwitch::init()
/// @details Initializes @ref m_isOn to `false` and @ref m_activationPercent to `0.0f`.
void JugemSwitchReverse::init() {
    m_isOn = false;
    m_activationPercent = 0.0f;
}

/// @addr{0x80725C98}
/// @copybrief JugemSwitch::calc()
/// @details Checks if the player is driving the wrong way and updates the internal state of the
/// switch accordingly. If the player is driving the wrong way for 60 frames, Lakitu will toggle
/// on.
void JugemSwitchReverse::calc() {
    constexpr f32 ACTIVATION_FRAME_STEP = 1.0f / 60.0f;

    if (System::RaceManager::Instance()->player().drivingWrongWay()) {
        const auto &kartStatus = Kart::KartObjectManager::Instance()->object(0)->status();
        if (kartStatus.offBit(Kart::eStatus::InAction)) {
            m_activationPercent += ACTIVATION_FRAME_STEP;
        }

        if (m_activationPercent > 1.0f && kartStatus.offBit(Kart::eStatus::BeforeRespawn)) {
            m_activationPercent = 1.0f;
            m_isOn = true;
        }
    } else {
        m_activationPercent -= ACTIVATION_FRAME_STEP;
        if (m_activationPercent < 0.0f) {
            m_activationPercent = 0.0f;
            m_isOn = false;
        }
    }
}

} // namespace Kinoko::Field
