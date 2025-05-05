#include "KartAction.hh"

#include "game/kart/KartMove.hh"
#include "game/kart/KartPhysics.hh"
#include "game/kart/KartState.hh"

#include <egg/math/Math.hh>

namespace Kart {

/// @addr{0x805672CC}
KartAction::KartAction()
    : m_currentAction(Action::None), m_hitDepth(EGG::Vector3f::zero), m_onStart(nullptr),
      m_onCalc(nullptr), m_onEnd(nullptr), m_actionParams(nullptr), m_rotationParams(nullptr),
      m_priority(0) {}

/// @addr{0x8056A1A8}
KartAction::~KartAction() = default;

/// @addr{0x8056739C}
void KartAction::init() {
    m_currentAction = Action::None;
    m_flags.makeAllZero();
}

/// @addr{0x805673B0}
void KartAction::calc() {
    if (m_currentAction == Action::None || !m_onCalc) {
        return;
    }

    if (calcCurrentAction()) {
        calcEndAction(false);
    }
}

/// @addr{0x80567CE4}
void KartAction::calcVehicleSpeed() {
    move()->setSpeed(m_actionParams->calcSpeedMult * move()->speed());
}

/// @addr{0x805675DC}
/// @brief Starts an action.
/// @param action The action to start.
/// @return Whether or not the action was started.
bool KartAction::start(Action action) {
    ASSERT(action != Action::None);

    if (state()->isInRespawn() || state()->isAfterRespawn() || state()->isBeforeRespawn() ||
            state()->isInCannon()) {
        return false;
    }

    size_t actionIdx = static_cast<size_t>(action);
    if (m_currentAction != Action::None && s_actionParams[actionIdx].priority <= m_priority) {
        return false;
    }

    calcEndAction(true);
    m_currentAction = action;
    m_actionParams = &s_actionParams[actionIdx];
    m_priority = m_actionParams->priority;
    m_onStart = s_onStart[actionIdx];
    m_onCalc = s_onCalc[actionIdx];
    m_onEnd = s_onEnd[actionIdx];
    state()->setInAction(true);
    m_frame = 0;
    m_flags.makeAllZero();
    m_up = move()->up();
    move()->clear();

    applyStartSpeed();
    (this->*m_onStart)();
    return true;
}

/// @addr{0x80567D3C}
/// @brief Initializes rotation parameters.
/// @warning The parameter is supposed to be an enum, but we discard it.
/// This results in the arguments being off-by-one. Beware of zero!
/// @param idx The index for the rotation parameters.
void KartAction::startRotation(size_t idx) {
    m_rotation = EGG::Quatf::ident;

    EGG::Vector3f dir = move()->dir();
    if (speed() < 0.0f) {
        dir = -dir;
    }

    m_rotationDirection = dir.cross(bodyFront()).dot(bodyUp()) > 0.0f ? 1.0f : -1.0f;
    setRotation(idx);
    m_flags.setBit(eFlags::Rotating);
}

void KartAction::setHitDepth(const EGG::Vector3f &hitDepth) {
    m_hitDepth = hitDepth;
}

const KartAction::Flags &KartAction::flags() const {
    return m_flags;
}

/// @addr{0x80567B98}
void KartAction::end() {
    state()->setInAction(false);
    dynamics()->setForceUpright(true);

    m_currentAction = Action::None;
    m_priority = 0;
    m_flags.makeAllZero();
}

/// @addr{0x80567A54}
/// @brief Executes a frame of the current action.
/// @return Whether or not the action should end.
bool KartAction::calcCurrentAction() {
    ++m_frame;
    return (this->*m_onCalc)();
}

/// @addr{0x80567A88}
void KartAction::calcEndAction(bool endArg) {
    if (m_currentAction == Action::None) {
        return;
    }

    if (m_onEnd) {
        (this->*m_onEnd)(endArg);
        end();
    }
}

/// @addr{0x80569DFC}
bool KartAction::calcRotation() {
    if (!m_rotationParams) {
        return false;
    }

    // Slow the rotation down as we approach the end of the spinout
    if (m_currentAngle > m_finalAngle * m_rotationParams->slowdownThreshold) {
        m_angleIncrement *= m_multiplier;
        if (m_rotationParams->minAngleIncrement > m_angleIncrement) {
            m_angleIncrement = m_rotationParams->minAngleIncrement;
        }

        m_multiplier -= m_multiplierDecrement;
        if (m_rotationParams->minMultiplier > m_multiplier) {
            m_multiplier = m_rotationParams->minMultiplier;
        }
    }

    m_currentAngle += m_angleIncrement;
    if (m_finalAngle < m_currentAngle) {
        m_currentAngle = m_finalAngle;
        return true;
    }

    return false;
}

/// @addr{0x80569E9C}
void KartAction::calcUp() {
    m_up += (move()->up() - m_up) * 0.3f;
    m_up.normalise();
}

/// @addr{0x80567C68}
void KartAction::applyStartSpeed() {
    move()->setSpeed(m_actionParams->startSpeedMult * move()->speed());
    if (m_actionParams->startSpeedMult == 0.0f) {
        move()->clearDrift();
    }
}

/// @addr{0x80569DB4}
void KartAction::setRotation(size_t idx) {
    ASSERT(idx - 1 < s_rotationParams.size());
    m_rotationParams = &s_rotationParams[--idx];

    m_finalAngle = m_rotationParams->finalAngle;
    m_angleIncrement = m_rotationParams->initialAngleIncrement;
    m_multiplierDecrement = m_rotationParams->initialMultiplierDecrement;
    m_currentAngle = 0.0f;
    m_multiplier = 1.0f;
}

/* ================================ *
 *     START FUNCTIONS
 * ================================ */

void KartAction::startStub() {}

/// @addr{0x80567FB4}
void KartAction::startAction1() {
    startRotation(2);
}

/// @addr{0x80568000}
void KartAction::startAction9() {
    startRotation(2);
}

/* ================================ *
 *     CALC FUNCTIONS
 * ================================ */

bool KartAction::calcStub() {
    return false;
}

/// @addr{0x80568204}
bool KartAction::calcAction1() {
    calcUp();
    bool finished = calcRotation();

    m_rotation.setAxisRotation(DEG2RAD * (m_currentAngle * m_rotationDirection), m_up);
    physics()->composeExtraRot(m_rotation);
    return finished;
}

/* ================================ *
 *     END FUNCTIONS
 * ================================ */

void KartAction::endStub(bool /*arg*/) {}

/// @addr{0x8056837C}
void KartAction::endAction1(bool arg) {
    if (arg) {
        physics()->composeDecayingExtraRot(m_rotation);
    }
}

/* ================================ *
 *     ACTION TABLES
 * ================================ */

const std::array<KartAction::ActionParams, KartAction::MAX_ACTION> KartAction::s_actionParams = {{
        {0.98f, 0.98f, 1},
        {0.98f, 0.98f, 2},
        {0.96f, 0.96f, 4},
        {0.0f, 0.96f, 4},
        {0.0f, 0.98f, 4},
        {0.0f, 0.96f, 4},
        {0.0f, 0.96f, 4},
        {0.0f, 0.0f, 6},
        {0.0f, 0.99f, 6},
        {0.98f, 0.98f, 3},
        {0.98f, 0.98f, 3},
        {1.0f, 1.0f, 5},
        {0.0f, 0.0f, 3},
        {0.0f, 0.0f, 3},
        {0.0f, 0.0f, 3},
        {0.98f, 0.98f, 3},
        {0.0f, 0.0f, 3},
        {0.98f, 0.98f, 3},
}};

const std::array<KartAction::RotationParams, 5> KartAction::s_rotationParams = {{
        {10.0f, 1.5f, 0.9f, 0.005f, 0.6f, 360.0f},
        {11.0f, 1.5f, 0.9f, 0.0028f, 0.7f, 720.0f},
        {11.0f, 1.5f, 0.9f, 0.0028f, 0.8f, 1080.0f},
        {7.0f, 1.5f, 0.9f, 0.005f, 0.6f, 450.0f},
        {9.0f, 1.5f, 0.9f, 0.0028f, 0.7f, 810.0f},
}};

const std::array<KartAction::StartActionFunc, KartAction::MAX_ACTION> KartAction::s_onStart = {{
        &KartAction::startStub,
        &KartAction::startAction1,
        &KartAction::startStub,
        &KartAction::startStub,
        &KartAction::startStub,
        &KartAction::startStub,
        &KartAction::startStub,
        &KartAction::startStub,
        &KartAction::startStub,
        &KartAction::startAction9,
        &KartAction::startStub,
        &KartAction::startStub,
        &KartAction::startStub,
        &KartAction::startStub,
        &KartAction::startStub,
        &KartAction::startStub,
        &KartAction::startStub,
        &KartAction::startStub,
}};

const std::array<KartAction::CalcActionFunc, KartAction::MAX_ACTION> KartAction::s_onCalc = {{
        &KartAction::calcStub,
        &KartAction::calcAction1,
        &KartAction::calcStub,
        &KartAction::calcStub,
        &KartAction::calcStub,
        &KartAction::calcStub,
        &KartAction::calcStub,
        &KartAction::calcStub,
        &KartAction::calcStub,
        &KartAction::calcAction1,
        &KartAction::calcStub,
        &KartAction::calcStub,
        &KartAction::calcStub,
        &KartAction::calcStub,
        &KartAction::calcStub,
        &KartAction::calcStub,
        &KartAction::calcStub,
        &KartAction::calcStub,
}};

const std::array<KartAction::EndActionFunc, KartAction::MAX_ACTION> KartAction::s_onEnd = {{
        &KartAction::endStub,
        &KartAction::endAction1,
        &KartAction::endStub,
        &KartAction::endStub,
        &KartAction::endStub,
        &KartAction::endStub,
        &KartAction::endStub,
        &KartAction::endStub,
        &KartAction::endStub,
        &KartAction::endAction1,
        &KartAction::endStub,
        &KartAction::endStub,
        &KartAction::endStub,
        &KartAction::endStub,
        &KartAction::endStub,
        &KartAction::endStub,
        &KartAction::endStub,
        &KartAction::endStub,
}};

} // namespace Kart
