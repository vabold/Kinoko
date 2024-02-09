#include "KartMove.hh"

#include "game/kart/KartDynamics.hh"
#include "game/kart/KartParam.hh"
#include "game/kart/KartPhysics.hh"
#include "game/kart/KartState.hh"
#include "game/kart/KartSub.hh"
#include "game/kart/KartSuspension.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/KCollisionTypes.hh"

#include "game/system/RaceManager.hh"

#include <egg/math/Math.hh>
#include <egg/math/Quat.hh>

namespace Kart {

constexpr f32 LEAN_ROT_INC_COUNTDOWN = 0.08f;
constexpr f32 LEAN_ROT_CAP_COUNTDOWN = 0.6f;

KartMove::KartMove()
    : m_smoothedUp(EGG::Vector3f::ey), m_scale(1.0f, 1.0f, 1.0f), m_totalScale(1.0f) {}

void KartMove::calcTurn() {
    m_realTurn = 0.0f;
    m_rawTurn = 0.0f;

    m_rawTurn = -state()->stickX();

    f32 reactivity = param()->stats().handlingReactivity;

    m_weightedTurn = m_rawTurn * reactivity + m_weightedTurn * (1.0f - reactivity);
    m_weightedTurn = std::max(-1.0f, std::min(1.0f, m_weightedTurn));

    m_realTurn = m_weightedTurn;
}

void KartMove::setTurnParams() {
    init(false, false);
    m_dir = bodyFront();
    m_vel1Dir = m_dir;
}

void KartMove::init(bool b1, bool b2) {
    setKartSpeedLimit();
    m_up = EGG::Vector3f::ey;
    m_smoothedUp = EGG::Vector3f::ey;
    m_vel1Dir = EGG::Vector3f::ez;
    m_dir = EGG::Vector3f::ez;
    m_speedRatioCapped = 0.0f;
    m_kclRotFactor = 1.0f;
    m_kclWheelRotFactor = 1.0f;

    if (!b2) {
        m_floorCollisionCount = 0;
    }

    m_standStillBoostRot = 0.0f;
    m_realTurn = 0.0f;
    m_weightedTurn = 0.0f;

    if (!b1) {
        m_scale.set(1.0f);
        m_totalScale = 1.0f;
    }

    m_rawTurn = 0.0f;
}

f32 KartMove::leanRot() const {
    return 0.0f;
}

void KartMove::setInitialPhysicsValues(const EGG::Vector3f &position, const EGG::Vector3f &angles) {
    EGG::Quatf quaternion;
    quaternion.setRPY(angles * DEG2RAD);
    EGG::Vector3f newPos = position;
    Field::CourseColMgr::CollisionInfo info;
    Field::KCLTypeMask kcl_flags = KCL_NONE;

    bool bColliding = Field::CollisionDirector::Instance()->checkSphereFullPush(100.0f, newPos,
            EGG::Vector3f::inf, KCL_ANY, &info, &kcl_flags, 0);

    if (bColliding && (kcl_flags & KCL_TYPE_FLOOR)) {
        newPos += info.tangentOff + (info.floorNrm * -100.0f);
        newPos += info.floorNrm * bsp().initialYPos;
    }

    setPos(newPos);
    setRot(quaternion);

    sub()->initPhysicsValues();

    physics()->setPos(pos());
    physics()->setVelocity(dynamics()->velocity());

    m_dir = bodyFront();

    for (u16 tireIdx = 0; tireIdx < suspCount(); ++tireIdx) {
        suspension(tireIdx)->setInitialState();
    }
}

void KartMove::setKartSpeedLimit() {
    constexpr f32 LIMIT = 120.0f;
    m_hardSpeedLimit = LIMIT;
}

void KartMove::calc() {
    dynamics()->resetInternalVelocity();
    calcTop();
    calcDirs();
    calcOffroad();
    calcTurn();
    calcWheelie();
    calcRotation();
}

void KartMove::calcTop() {
    m_up += state()->top();

    f32 topDotZ = 0.8f - 6.0f * (EGG::Mathf::abs(state()->top().dot(componentZAxis())));
    f32 scalar = std::min(0.8f, std::max(0.3f, topDotZ));

    if (state()->isGround()) {
        m_smoothedUp += (state()->top() - m_smoothedUp) * scalar;
        m_smoothedUp.normalise();
    }
}

void KartMove::calcDirs() {
    EGG::Vector3f right = dynamics()->mainRot().rotateVector(EGG::Vector3f::ex);
    EGG::Vector3f local_88 = right.cross(m_smoothedUp);
    local_88.normalise();
    EGG::Matrix34f mat;
    mat.setAxisRotation(0.0f, m_smoothedUp);
    EGG::Vector3f local_b8 = mat.multVector(local_88);
    local_b8 = local_b8.perpInPlane(m_smoothedUp, true);

    EGG::Vector3f dirDiff = local_b8 - m_dir;

    if (dirDiff.dot() <= FLT_EPSILON) {
        m_dir = local_b8;
        m_dirDiff.setZero();
    } else {
        EGG::Vector3f origDirCross = m_dir.cross(local_b8);
        m_dirDiff += m_kclRotFactor * dirDiff;
        m_dir += m_dirDiff;
        m_dir.normalise();
        m_dirDiff *= 0.1f;
        EGG::Vector3f newDirCross = m_dir.cross(local_b8);

        if (origDirCross.dot(newDirCross) < 0.0f) {
            m_dir = local_b8;
            m_dirDiff.setZero();
        }
    }

    m_vel1Dir = m_dir.perpInPlane(m_smoothedUp, true);
}

void KartMove::calcOffroad() {
    m_kclRotFactor = m_kclWheelRotFactor / static_cast<f32>(m_floorCollisionCount);
}

void KartMove::calcRotation() {
    calcVehicleRotation(0.0f);
}

void KartMove::setFloorCollisionCount(u16 count) {
    m_floorCollisionCount = count;
}

void KartMove::setKCLWheelRotFactor(f32 val) {
    m_kclWheelRotFactor = val;
}

const EGG::Vector3f &KartMove::scale() const {
    return m_scale;
}

f32 KartMove::hardSpeedLimit() const {
    return m_hardSpeedLimit;
}

const EGG::Vector3f &KartMove::smoothedUp() const {
    return m_smoothedUp;
}

f32 KartMove::totalScale() const {
    return m_totalScale;
}

const EGG::Vector3f &KartMove::dir() const {
    return m_dir;
}

u16 KartMove::floorCollisionCount() const {
    return m_floorCollisionCount;
}

KartMoveBike::KartMoveBike() : m_leanRot(0.0f) {}

KartMoveBike::~KartMoveBike() = default;

void KartMoveBike::startWheelie() {
    constexpr f32 MAX_WHEELIE_ROTATION = 0.07f;
    constexpr u16 WHEELIE_COOLDOWN = 0x14;

    state()->setWheelie(true);
    m_wheelieFrames = 0;
    m_maxWheelieRot = MAX_WHEELIE_ROTATION;
    m_wheelieCooldown = WHEELIE_COOLDOWN;
    m_wheelieRotDec = 0.0f;
}

void KartMoveBike::cancelWheelie() {
    state()->setWheelie(false);
    m_wheelieRotDec = 0.0f;
}

void KartMoveBike::calcVehicleRotation(f32 turn) {
    m_leanRotCap += 0.3f * (LEAN_ROT_CAP_COUNTDOWN - m_leanRotCap);
    m_leanRotInc += 0.3f * (LEAN_ROT_INC_COUNTDOWN - m_leanRotInc);

    f32 stickX = state()->stickX();
    f32 dVar15 = 0.0f;

    if (state()->isWheelie()) {
        m_leanRot *= 0.9f;
    } else {
        if (stickX <= 0.2f) {
            if (stickX >= -0.2f) {
                m_leanRot *= 0.9f;
            } else {
                m_leanRot -= m_leanRotInc;
                dVar15 = 1.0f;
            }
        } else {
            m_leanRot += m_leanRotInc;
            dVar15 = -1.0f;
        }
    }

    bool capped = false;
    if (-m_leanRotCap <= m_leanRot) {
        if (m_leanRotCap < m_leanRot) {
            m_leanRot = m_leanRotCap;
            capped = true;
        }
    } else {
        m_leanRot = -m_leanRotCap;
        capped = true;
    }

    if (!capped) {
        dynamics()->setExtVel(dynamics()->extVel() + componentXAxis() * dVar15);
    }

    calcStandstillBoostRot();

    dynamics()->setAngVel2(dynamics()->angVel2() +
            EGG::Vector3f(m_standStillBoostRot, turn * wheelieRotFactor(), m_leanRot * 0.05f));

    f32 scalar = std::min(1.0f, m_speedRatioCapped * 2.0f);

    EGG::Vector3f top = scalar * m_up + (1.0f - scalar) * EGG::Vector3f::ey;
    if (FLT_EPSILON < top.dot()) {
        top.normalise();
    }
    dynamics()->setTop_(top);
}

void KartMoveBike::setTurnParams() {
    KartMove::setTurnParams();

    m_leanRotInc = LEAN_ROT_INC_COUNTDOWN;
    m_leanRotCap = LEAN_ROT_CAP_COUNTDOWN;
}

void KartMoveBike::init(bool b1, bool b2) {
    KartMove::init(b1, b2);

    m_leanRot = 0.0f;
    m_leanRotCap = 0.0f;
    m_leanRotInc = 0.0f;
    m_wheelieRot = 0.0f;
    m_wheelieCooldown = 0;
}

void KartMoveBike::calcWheelie() {
    constexpr u32 FAILED_WHEELIE_FRAMES = 15;

    tryStartWheelie();
    --m_wheelieCooldown;
    m_wheelieCooldown = std::max<u16>(0, m_wheelieCooldown);

    // NOTE: This is inverted from Ghidra output
    if (state()->isWheelie()) {
        ++m_wheelieFrames;
        if (FAILED_WHEELIE_FRAMES <= m_wheelieFrames) {
            cancelWheelie();
        } else {
            m_wheelieRot += 0.01f;
            EGG::Vector3f angVel0 = dynamics()->angVel0();
            angVel0.x *= 0.9f;
            dynamics()->setAngVel0(angVel0);
        }
    } else if (0.0f < m_wheelieRot) {
        m_wheelieRotDec -= 0.001f;
        m_wheelieRotDec = std::max(-0.03f, m_wheelieRotDec);
        m_wheelieRot += m_wheelieRotDec;
    }

    m_wheelieRot = std::max(0.0f, std::min(m_wheelieRot, m_maxWheelieRot));

    f32 vel1DirUp = m_vel1Dir.dot(EGG::Vector3f::ey);

    if (m_wheelieRot > 0.0f) {
        if (vel1DirUp <= 0.5f || m_wheelieFrames < FAILED_WHEELIE_FRAMES) {
            EGG::Vector3f angVel2 = dynamics()->angVel2();
            angVel2.x -= m_wheelieRot * (1.0f - EGG::Mathf::abs(vel1DirUp));
            dynamics()->setAngVel2(angVel2);
        }
        state()->setWheelieRot(true);
    } else {
        state()->setWheelieRot(false);
    }
}

void KartMoveBike::calcStandstillBoostRot() {
    f32 next = 0.0f;

    if (state()->isGround()) {
        if (System::RaceManager::Instance()->stage() == System::RaceManager::Stage::Countdown) {
            next = 0.015f * -state()->startBoostCharge();
        }
    }

    m_standStillBoostRot = next;
}

f32 KartMoveBike::wheelieRotFactor() const {
    constexpr f32 WHEELIE_ROTATION_FACTOR = 0.2f;

    return state()->isWheelie() ? WHEELIE_ROTATION_FACTOR : 1.0f;
}

// Also handles cancelling wheelies
void KartMoveBike::tryStartWheelie() {
    bool dpadUp = inputs()->currentState().trickUp();

    if (!state()->isWheelie()) {
        if (dpadUp && state()->isGround()) {
            startWheelie();
        }
    }
}

f32 KartMoveBike::leanRot() const {
    return m_leanRot;
}

} // namespace Kart
