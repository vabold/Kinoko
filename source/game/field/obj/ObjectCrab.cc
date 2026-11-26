#include "ObjectCrab.hh"

#include "game/field/RailInterpolator.hh"

#include "game/system/RaceManager.hh"
#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

/// @addr{0x8088344C}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Sets @ref m_vel based on param setting 1 and sets @ref m_backwards to `true` when
/// param setting 2 is non-zero. Finally, initializes @ref m_introCalc to `false`.
ObjectCrab::ObjectCrab(const System::MapdataGeoObj &params)
    : ObjectCollidable(params),
      m_vel(static_cast<f32>(static_cast<s16>(params.setting(0)))),
      m_backwards(!!params.setting(1)),
      m_introCalc(false) {}

/// @addr{0x808837B8}
/// @brief Default virtual destructor
ObjectCrab::~ObjectCrab() = default;

/// @addr{0x80883844}
/// @copybrief ObjectBase::init()
/// @details Initializes the crab's rotation to @ref INIT_ROT. Updates the rail interpolator's
/// velocity to @ref m_vel and updates the crab's position to the rail interpolator's position.
/// Updates the crab's transform based off of @ref m_curRot. Finally, initializes the crab to the
/// walking state.
void ObjectCrab::init() {
    m_railInterpolator->init(0.0f, 0);

    calcCurRot(INIT_ROT);

    m_railInterpolator->setSpeed(m_vel);
    setPos(m_railInterpolator->curPos());
    calcTransMat(m_curRot);

    m_stillDuration = 0;
    m_stillFrame = 0;
    m_still = false;
    setState(State::Walking);
}

/// @addr{0x80883B98}
/// @copybrief ObjectBase::calc()
/// @details This class has unique behavior in that it runs the calc function only once during race
/// load-in, enforced via @ref m_introCalc. Calls @ref calcRail() to update the rail interpolator's
/// position and speed. Depending on the current state of the crab's walk, the crab's position will
/// be set to the rail interpolator's position.
void ObjectCrab::calc() {
    if (System::RaceManager::Instance()->timer() == 0 && m_introCalc) {
        return;
    }

    m_introCalc = true;

    if (!calcRail()) {
        return;
    }

    StateResult res = calcState();

    if (res == StateResult::Middle) {
        return;
    }

    if (res == StateResult::Walking) {
        if (m_statePhase == StatePhase::Start) {
            m_statePhase = StatePhase::Middle;
        }
    }

    if (m_statePhase == StatePhase::Middle) {
        setPos(m_railInterpolator->curPos());

        if (m_still) {
            m_statePhase = StatePhase::End;
        }

        return;
    } else {
        setState(State::Still);
        calcState();
    }
}

/// @addr{0x808864DC}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectCrab::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x80886438}
/// @brief Sets the provided state and resets the phase
/// @param state The new state to set for the crab
/// @details Updates @ref m_state to the provided state and resets @ref m_statePhase to @ref
/// StatePhase::Start.
void ObjectCrab::setState(State state) {
    m_state = state;
    m_statePhase = StatePhase::Start;
}

/// @brief Tries to move the crab along its rail, unless @ref m_still is `true`
/// @return `false` if the crab is starting to move this frame, `true` if the caller should proceed
/// with normal calc processing.
/// @details If the crab is currently still, checks to see if the crab has been still for @ref
/// m_stillDuration frames. If so, updates the rail interpolator's speed, clears @ref m_still and
/// returns false to indicate that we should skip @ref calc() evaluation until the next frame.
/// Otherwise, updates the rail interpolator. When traversing onto a new rail point, checks if the
/// point's settings designate that the crab should pause at that point for a set number of frames.
bool ObjectCrab::calcRail() {
    if (m_still) {
        if (m_stillDuration <= ++m_stillFrame) {
            m_railInterpolator->setSpeed(m_vel);
            m_still = false;
            return false;
        }

        return true;
    }

    auto status = m_railInterpolator->calc();
    if (status == RailInterpolator::Status::SegmentEnd ||
            status == RailInterpolator::Status::RailEnd) {
        u16 duration = m_railInterpolator->curPoint().setting[0];
        if (duration > 0) {
            m_railInterpolator->setSpeed(0.0f);
            m_stillDuration = duration;
            m_stillFrame = 0;
            m_still = true;
        }
    }

    return true;
}

/// @brief Manages the crab's still/pause state machine
/// @details If the crab is not still, then it returns @ref StateResult::Walking. Otherwise:
/// - Start phase -> Snaps the crab to the rail, resetting its rotation and advances to @ref
/// StatePhase::Middle.
/// - Middle phase -> Holds until the still duration has elapsed, then advances to @ref
/// StatePhase::End and returns @ref StateResult::Middle.
/// - End phase -> Resets the state to @ref State::Walking and returns @ref
/// StateResult::BeginWalking.
ObjectCrab::StateResult ObjectCrab::calcState() {
    if (m_state != State::Still) {
        return StateResult::Walking;
    }

    if (m_statePhase == StatePhase::Start) {
        setPos(m_railInterpolator->curPos());
        calcCurRot(INIT_ROT);
        calcTransMat(m_curRot);
        m_statePhase = StatePhase::Middle;
    }

    if (m_statePhase == StatePhase::Middle) {
        if (!m_still) {
            m_statePhase = StatePhase::End;
        }

        return StateResult::Middle;
    } else {
        setState(State::Walking);
    }

    return StateResult::BeginWalking;
}

/// @brief Updates the crab's rotation based off the provided rotation
/// @param rot The rotation to set for the crab.
/// @details Sets rotation by factoring in the crab's backwards setting and rail direction.
void ObjectCrab::calcCurRot(const EGG::Vector3f &rot) {
    m_curRot = rot;
    m_curRot = m_backwards ? -m_curRot : m_curRot;
    m_curRot.y = m_railInterpolator->isMovementDirectionForward() ? m_curRot.y : -m_curRot.y;
}

/// @brief Sets the transformation matrix based on the provided rotation and the rail's tangent
/// @param rot The rotation to apply to the crab's transformation matrix
/// @details The provided rotation is applied on top of the tangent-facing basis.
void ObjectCrab::calcTransMat(const EGG::Vector3f &rot) {
    EGG::Matrix34f rotMat;
    rotMat.makeR(rot);
    rotMat = rotMat.multiplyTo(EGG::Matrix34f::ident);

    EGG::Matrix34f mat;
    mat.makeOrthonormalBasisLocal(m_railInterpolator->curTangentDir(), EGG::Vector3f::ey);
    mat = mat.multiplyTo(rotMat);
    mat.setBase(3, pos());
    setTransform(mat);
}

} // namespace Kinoko::Field
