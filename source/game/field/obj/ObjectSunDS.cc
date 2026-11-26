#include "ObjectSunDS.hh"

#include "game/field/RailInterpolator.hh"

#include "game/system/RaceManager.hh"
#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

/// @addr{0x806DDDD8}
/// @copybrief ObjectProjectileLauncher::ObjectProjectileLauncher(const System::MapdataGeoObj &)
/// @details Sets @ref m_revolutionSpeed based on param setting 1 and @ref m_startFrame based on
/// param setting 2.
ObjectSunDS::ObjectSunDS(const System::MapdataGeoObj &params)
    : ObjectProjectileLauncher(params),
      StateManager(this, STATE_ENTRIES),
      m_revolutionSpeed(static_cast<f32>(params.setting(0))),
      m_startFrame(static_cast<s32>(params.setting(1))) {}

/// @addr{0x806DDF68}
/// @brief Default virtual destructor
ObjectSunDS::~ObjectSunDS() = default;

/// @addr{0x806DDFD4}
/// @copybrief ObjectBase::init()
/// @details Initializes @ref m_stillDuration to zero.
void ObjectSunDS::init() {
    m_stillDuration = 0;
}

/// @addr{0x806DE03C}
/// @copybrief ObjectBase::calc()
/// @details Early returns if @ref m_startFrame frames have not elapsed in the race yet. Updates
/// the sun's rail position, checking to see if the sun should stop. Evaluates the sun's state
/// machine. Finally, updates the sun's position based on the rail interpolator's current
/// position.
void ObjectSunDS::calc() {
    if (System::RaceManager::Instance()->timer() < static_cast<u32>(m_startFrame)) {
        return;
    }

    calcRail();
    StateManager::calc();
    calcPos();
}

/// @addr{0x806DE614}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectSunDS::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x806DE598}
/// @copybrief ObjectProjectileLauncher::launchPointIdx()
/// @return The index of the rail point from which to launch a @ref ObjectFireSnake, or `-1` if no
/// @ref ObjectFireSnake should be launched this frame.
/// @details If the sun is currently in the still state and the sun has been stationary for exactly
/// 30 frames, then returns the rail interpolator's current point index. Otherwise, returns `-1`.
s16 ObjectSunDS::launchPointIdx() const {
    constexpr u16 THROW_DELAY = 30;

    if (m_currentStateId != 0 || THROW_DELAY != m_currentFrame) {
        return -1;
    }

    return m_railInterpolator->curPointIdx();
}

/// @addr{0x806DE1C8}
/// @brief Runs once when the sun has reached a point along its rail designated as a stop point
/// @details Sets the rail interpolator's speed to zero.
void ObjectSunDS::enterStill() {
    m_railInterpolator->setSpeed(0.0f);
}

/// @addr{0x806DE408}
/// @brief Runs once when the sun begins revolving around the course again
/// @details Sets the rail interpolator's speed to @ref m_revolutionSpeed.
void ObjectSunDS::enterRevolving() {
    m_railInterpolator->setSpeed(m_revolutionSpeed);
}

/// @addr{0x806DE1E4}
/// @brief Runs every frame that the sun is stationary
/// @details If the sun has been stationary for @ref m_stillDuration frames, then transitions
/// the sun to the revolving state.
void ObjectSunDS::calcStill() {
    if (m_currentFrame >= m_stillDuration) {
        m_nextStateId = 1;
    }
}

/// @addr{0x806DE458}
/// @brief Updates the rail interpolator and checks to see if the sun should stop
/// @details Updates the rail interpolator. If the interpolator has reached the end of a rail
/// segment, then snaps the rail interpolator to the start of the new rail segment and calls
/// @ref checkStop() to see if the sun should transition to the still state.
void ObjectSunDS::calcRail() {
    if (m_railInterpolator->calc() == RailInterpolator::Status::SegmentEnd) {
        m_railInterpolator->setT(0.0f);
        checkStop();
    }
}

/// @addr{0x806DE4E4}
/// @brief Updates the sun's position based on the current rail interpolation
void ObjectSunDS::calcPos() {
    setPos(m_railInterpolator->curPos());
}

/// @addr{0x806DE568}
/// @brief Checks if the current rail point is a stop point
/// @details Stop points are identified based on the rail point's first setting being non-zero.
/// This setting specifies the number of frames the sun should remain stationary for. If this
/// setting is non-zero, sets @ref m_stillDuration based on the setting's value and transitions
/// the sun to the still state.
void ObjectSunDS::checkStop() {
    u16 setting = m_railInterpolator->curPoint().setting[0];
    if (setting != 0) {
        m_stillDuration = setting;
        m_nextStateId = 0;
    }
}

} // namespace Kinoko::Field
