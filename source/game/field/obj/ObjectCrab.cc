#include "ObjectCrab.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/ObjectDirector.hh"

#include "game/system/RaceManager.hh"

namespace Field {

/// @addr{0x8088344C}
ObjectCrab::ObjectCrab(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_vel(static_cast<f32>(static_cast<s16>(params.setting(0)))),
      m_backwards(!!params.setting(1)), m_introCalc(false) {}

/// @addr{0x808837B8}
ObjectCrab::~ObjectCrab() = default;

/// @addr{0x80883844}
void ObjectCrab::init() {
    m_railInterpolator->init(0.0f, 0);

    calcCurRot(INIT_ROT);

    m_railInterpolator->setCurrVel(m_vel);
    m_pos = m_railInterpolator->curPos();
    m_flags.setBit(eFlags::Position);

    calcTransMat(m_curRot);

    m_stillDuration = 0;
    m_stillFrame = 0;
    m_still = false;
    m_state = State::Walking;
    m_statePhase = StatePhase::Start;
}

/// @addr{0x80883B98}
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
        m_pos = m_railInterpolator->curPos();
        m_flags.setBit(eFlags::Position);

        if (m_still) {
            m_statePhase = StatePhase::End;
        }

        return;
    } else {
        m_state = State::Still;
        m_statePhase = StatePhase::Start;

        calcState();
    }
}

bool ObjectCrab::calcRail() {
    if (m_still) {
        if (m_stillDuration <= ++m_stillFrame) {
            m_railInterpolator->setCurrVel(m_vel);
            m_still = false;
            return false;
        }

        return true;
    }

    auto status = m_railInterpolator->calc();
    if (status == RailInterpolator::Status::SegmentEnd ||
            status == RailInterpolator::Status::ChangingDirection) {
        u16 duration = m_railInterpolator->curPoint().setting[0];
        if (duration > 0) {
            m_railInterpolator->setCurrVel(0.0f);
            m_stillDuration = duration;
            m_stillFrame = 0;
            m_still = true;
        }
    }

    return true;
}

ObjectCrab::StateResult ObjectCrab::calcState() {
    if (m_state != State::Still) {
        return StateResult::Walking;
    }

    if (m_statePhase == StatePhase::Start) {
        m_pos = m_railInterpolator->curPos();
        m_flags.setBit(eFlags::Position);

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
        m_state = State::Walking;
        m_statePhase = StatePhase::Start;
    }

    return StateResult::BeginWalking;
}

void ObjectCrab::calcCurRot(const EGG::Vector3f &rot) {
    m_curRot = rot;
    m_curRot = m_backwards ? -m_curRot : m_curRot;
    m_curRot.y = m_railInterpolator->isMovementDirectionForward() ? m_curRot.y : -m_curRot.y;
}

void ObjectCrab::calcTransMat(const EGG::Vector3f &rot) {
    EGG::Matrix34f rotMat;
    rotMat.makeR(rot);
    rotMat = rotMat.multiplyTo(EGG::Matrix34f::ident);

    EGG::Matrix34f basis;
    basis.makeOrthonormalBasisLocal(m_railInterpolator->curTangentDir(), EGG::Vector3f::ey);

    m_transform = basis.multiplyTo(rotMat);
    m_transform.setBase(3, m_pos);
    m_flags.setBit(eFlags::Matrix);
}

} // namespace Field
