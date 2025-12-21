#include "ObjectFlamePoleV.hh"

#include "game/system/RaceManager.hh"

namespace Field {

/// @addr{0x806C3AA4}
ObjectFlamePoleV::ObjectFlamePoleV(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), StateManager(this, STATE_ENTRIES),
      m_initDelay(static_cast<s32>(params.setting(1))),
      m_cycleDuration(static_cast<s32>(params.setting(0))),
      m_dormantFrames(static_cast<s32>(params.setting(3)) + 200),
      m_scaleFactor(params.setting(2) == 0 ? 5.0f : static_cast<f32>(params.setting(2))),
      m_initPosY(m_pos.y), m_isBig(strcmp(getName(), "FlamePole_v_big") == 0) {
    constexpr f32 M_D8 = 384.0f;
    constexpr f32 BIG_MULTIPLIER = 14.0f;

    if (m_isBig) {
        m_maxOffsetY = BIG_MULTIPLIER * M_D8 * m_scaleFactor;
    } else {
        m_maxOffsetY = M_D8 * m_scaleFactor;
    }
}

/// @addr{0x806C47B4}
ObjectFlamePoleV::~ObjectFlamePoleV() = default;

/// @addr{0x806C3E90}
void ObjectFlamePoleV::init() {
    constexpr EGG::Vector3f BIG_SCALE = EGG::Vector3f(13.0f, 14.0f, 13.0f);
    constexpr f32 RADIUS = 70.0f;

    m_nextStateId = 0;

    disableCollision();

    if (m_isBig) {
        m_scale = BIG_SCALE;
        m_flags.setBit(eFlags::Scale);
    } else if (0.0f != m_scaleFactor) {
        m_scale = EGG::Vector3f(m_scaleFactor, m_scaleFactor, m_scaleFactor);
        m_flags.setBit(eFlags::Scale);
    }

    resize(RADIUS * m_scale.y, 0.0f);

    m_currOffsetY = 0.0f;
    m_fallSpeed = m_maxOffsetY / FALL_FRAMES;
}

/// @addr{0x806C3FCC}
void ObjectFlamePoleV::calc() {
    if (System::RaceManager::Instance()->timer() <= static_cast<u32>(m_initDelay)) {
        return;
    }

    StateManager::calc();

    m_flags.setBit(eFlags::Position);
    m_pos.y = m_currOffsetY + (m_initPosY - m_maxOffsetY);
}

/// @addr{0x806C42A0}
void ObjectFlamePoleV::calcErupted() {
    constexpr f32 AMPLITUDE = BEFORE_ERUPT_FRAMES;

    f32 frame = static_cast<f32>(m_currentFrame);
    if (frame >= static_cast<f32>(m_cycleDuration) - BEFORE_ERUPT_FRAMES - ERUPT_FRAMES -
                    FALL_FRAMES - static_cast<f32>(m_dormantFrames)) {
        m_nextStateId = 3;
    }

    f32 sin = EGG::Mathf::SinFIdx(DEG2FIDX * (360.0f * frame / 30.0f));
    m_currOffsetY = m_maxOffsetY + AMPLITUDE * sin;
}

/// @addr{0x806C43E8}
void ObjectFlamePoleV::calcLowering() {
    if (m_isBig) {
        if (m_currOffsetY <= -300.0f) {
            m_nextStateId = 4;
        }
    } else {
        if (m_currOffsetY <= 0.0f) {
            m_nextStateId = 4;
        }
    }
    m_currOffsetY = m_loweringStartOffsetY - m_fallSpeed * static_cast<f32>(m_currentFrame);
}

}; // namespace Field
