#include "KartScale.hh"

#include "game/kart/KartObjectManager.hh"

namespace Kart {

/// @addr{0x8056AD44}
KartScale::KartScale() {
    reset();
}

/// @addr{0x8056B5A8}
KartScale::~KartScale() = default;

/// @addr{0x8056AF10}
void KartScale::reset() {
    m_pressState = PressState::None;
    m_calcPress = false;
    m_pressUpAnmFrame = 0.0f;
    m_currScale = EGG::Vector3f(1.0f, 1.0f, 1.0f);
}

/// @addr{0x8056B218}
void KartScale::calc() {
    calcPress();
}

/// @addr{0x8056B060}
void KartScale::startPressDown() {
    m_pressState = PressState::Down;
    m_currScale = EGG::Vector3f(1.0f, 1.0f, 1.0f);
    m_pressUpAnmFrame = 0.0f;
    m_calcPress = true;
}

/// @addr{0x8056B094}
void KartScale::startPressUp() {
    m_pressState = PressState::Up;
    m_currScale = EGG::Vector3f(1.0f, CRUSH_SCALE, 1.0f);
    m_pressUpAnmFrame = 0.0f;
    m_calcPress = true;
}

/// @addr{0x8056B45C}
void KartScale::calcPress() {
    constexpr f32 SCALE_SPEED = 0.2f;

    if (!m_calcPress || m_pressState == PressState::None) {
        return;
    }

    if (m_pressState == PressState::Down) {
        m_currScale.y -= SCALE_SPEED;
        if (m_currScale.y < CRUSH_SCALE) {
            m_currScale.y = CRUSH_SCALE;
            m_calcPress = false;
        }
    } else {
        m_currScale = getAnmScale(m_pressUpAnmFrame);

        const auto *scaleAnm = KartObjectManager::PressScaleUpAnmChr();
        ASSERT(scaleAnm);

        if (++m_pressUpAnmFrame > static_cast<f32>(scaleAnm->frameCount())) {
            m_calcPress = false;
        }
    }
}

/// @addr{0x8056ACF4}
EGG::Vector3f KartScale::getAnmScale(f32 frame) const {
    const auto *scaleAnm = KartObjectManager::PressScaleUpAnmChr();
    ASSERT(scaleAnm);
    return scaleAnm->getAnmResult(frame, 0).scale();
}

} // namespace Kart
