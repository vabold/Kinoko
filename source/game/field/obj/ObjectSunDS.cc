#include "ObjectSunDS.hh"

#include "game/system/RaceManager.hh"

namespace Field {

/// @addr{0x806DDDD8}
ObjectSunDS::ObjectSunDS(const System::MapdataGeoObj &params)
    : ObjectProjectileLauncher(params), StateManager(this),
      m_revolutionSpeed(static_cast<f32>(params.setting(0))),
      m_startFrame(static_cast<s32>(params.setting(1))) {}

/// @addr{0x806DDF68}
ObjectSunDS::~ObjectSunDS() = default;

/// @addr{0x806DE03C}
void ObjectSunDS::calc() {
    if (System::RaceManager::Instance()->timer() < static_cast<u32>(m_startFrame)) {
        return;
    }

    if (m_railInterpolator->calc() == RailInterpolator::Status::SegmentEnd) {
        m_railInterpolator->setT(0.0f);

        u16 setting = m_railInterpolator->curPoint().setting[0];
        if (setting != 0) {
            m_stillDuration = setting;
            m_nextStateId = 0;
        }
    }

    StateManager::calc();

    m_pos = m_railInterpolator->curPos();
    m_flags.setBit(eFlags::Position);
}

/// @addr{0x806DE598}
s16 ObjectSunDS::launchPointIdx() {
    constexpr u16 THROW_DELAY = 30;

    if (m_currentStateId != 0 || THROW_DELAY != m_currentFrame) {
        return -1;
    }

    return m_railInterpolator->curPointIdx();
}

} // namespace Field
