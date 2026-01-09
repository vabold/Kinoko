#include "KartScale.hh"

#include "game/kart/KartObjectManager.hh"

namespace Kart {

/// @addr{0x8056AD44}
KartScale::KartScale(const KartParam::Stats &stats) {
    reset();

    for (u16 i = 0; i < 4; ++i) {
        m_scaleTarget[i] = s_baseScaleTarget[i];

        switch (i) {
        case 1:
            m_scaleTarget[i] = stats.shrinkScale;
            break;
        case 2:
            m_scaleTarget[i] = stats.megaScale;
            break;
        default:
            break;
        }
    }
}

/// @addr{0x8056B5A8}
KartScale::~KartScale() = default;

/// @addr{0x8056AF10}
void KartScale::reset() {
    m_type = -1;
    m_sizeScale = EGG::Vector3f::unit;
    m_scaleTransformOffset.setZero();
    m_scaleTransformSlope.setZero();
    m_scaleAnmActive = false;
    m_anmFrame = 0.0f;
    m_crushState = CrushState::None;
    m_calcCrush = false;
    m_uncrushAnmFrame = 0.0f;
    m_pressScale = EGG::Vector3f::unit;
}

/// @addr{0x8056B218}
void KartScale::calc() {
    if (m_scaleAnmActive) {
        const Abstract::g3d::ResAnmChr *scaleAnm;
        if (m_type == 0) {
            scaleAnm = KartObjectManager::ThunderScaleUpAnmChr();
        } else if (m_type == 1) {
            scaleAnm = KartObjectManager::ThunderScaleDownAnmChr();
        } else {
            PANIC("Invalid scale type");
        }
        ASSERT(scaleAnm);

        auto anmResult = scaleAnm->getAnmResult(m_anmFrame, 0);
        m_sizeScale = m_scaleTransformOffset + m_scaleTransformSlope * anmResult.scale();

        m_anmFrame += 1.0f;
        if (m_anmFrame > scaleAnm->frameCount()) {
            m_scaleAnmActive = false;
            m_sizeScale.set(m_scaleTarget[m_type]);
            m_scaleTransformOffset.setZero();
            m_scaleTransformSlope.setZero();
        }
    }

    calcCrush();
}

/// @addr{0x8056B060}
void KartScale::startCrush() {
    m_crushState = CrushState::Crush;
    m_pressScale = EGG::Vector3f(1.0f, 1.0f, 1.0f);
    m_uncrushAnmFrame = 0.0f;
    m_calcCrush = true;
}

/// @addr{0x8056B094}
void KartScale::endCrush() {
    m_crushState = CrushState::Uncrush;
    m_pressScale = EGG::Vector3f(1.0f, CRUSH_SCALE, 1.0f);
    m_uncrushAnmFrame = 0.0f;
    m_calcCrush = true;
}

/// @addr{0x8056AFB4}
void KartScale::startShrink(s32 unk) {
    m_type = unk > 0 ? 2 : 1;
    m_anmFrame = 0.0f;
    m_scaleAnmActive = true;
    f32 tmp = m_scaleTarget[m_type];
    m_scaleTransformSlope = (EGG::Vector3f(tmp, tmp, tmp) - m_sizeScale) /
            (s_baseScaleTarget[m_type] - s_baseScaleStart[m_type]);
    m_scaleTransformOffset = m_sizeScale - m_scaleTransformSlope * s_baseScaleStart[m_type];
}

/// @addr{0x8056B168}
void KartScale::endShrink(s32 unk) {
    m_type = unk > 0 ? 3 : 0;
    m_anmFrame = 0.0f;
    m_scaleAnmActive = true;
    f32 tmp = m_scaleTarget[m_type];
    m_scaleTransformSlope = (EGG::Vector3f(tmp, tmp, tmp) - m_sizeScale) /
            (s_baseScaleTarget[m_type] - s_baseScaleStart[m_type]);
    m_scaleTransformOffset = m_sizeScale - m_scaleTransformSlope * s_baseScaleStart[m_type];
}

/// @addr{0x8056B45C}
void KartScale::calcCrush() {
    constexpr f32 SCALE_SPEED = 0.2f;

    if (!m_calcCrush || m_crushState == CrushState::None) {
        return;
    }

    if (m_crushState == CrushState::Crush) {
        m_pressScale.y -= SCALE_SPEED;
        if (m_pressScale.y < CRUSH_SCALE) {
            m_pressScale.y = CRUSH_SCALE;
            m_calcCrush = false;
        }
    } else {
        m_pressScale = getAnmScale(m_uncrushAnmFrame);

        const auto *scaleAnm = KartObjectManager::PressScaleUpAnmChr();
        ASSERT(scaleAnm);

        if (++m_uncrushAnmFrame > static_cast<f32>(scaleAnm->frameCount())) {
            m_calcCrush = false;
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
