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

static constexpr f32 LEAN_ROT_INC_RACE = 0.1f;
static constexpr f32 LEAN_ROT_CAP_RACE = 1.0f;
static constexpr f32 LEAN_ROT_INC_COUNTDOWN = 0.08f;
static constexpr f32 LEAN_ROT_CAP_COUNTDOWN = 0.6f;

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
    m_lastSpeed = 0.0f;
    m_baseSpeed = param()->stats().speed;
    m_softSpeedLimit = param()->stats().speed;
    m_speed = 0.0f;
    setKartSpeedLimit();
    m_acceleration = 0.0f;
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

    if (m_boost.calc()) {
        state()->setAccelerate(true);
    } else {
        state()->setBoost(false);
    }

    calcVehicleSpeed();
    calcAcceleration();
    calcRotation();
}

void KartMove::calcTop() {
    m_up = state()->top();

    f32 topDotZ = 0.8f - 6.0f * (EGG::Mathf::abs(state()->top().dot(componentZAxis())));
    f32 scalar = std::min(0.8f, std::max(0.3f, topDotZ));

    if (state()->isTouchingGround()) {
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
    f32 turn = param()->stats().handlingManualTightness * m_realTurn;

    if (EGG::Mathf::abs(m_speed) < 1.0f) {
        turn = 0.0f;
    }

    if (m_speed >= 70.0f) {
        turn *= 0.5f;
    }

    calcVehicleRotation(turn);
}

void KartMove::calcVehicleSpeed() {
    const auto *raceMgr = System::RaceManager::Instance();
    if (raceMgr->isStageReached(System::RaceManager::Stage::Race)) {
        m_speed += dynamics()->speedFix();
    }

    m_acceleration = 0.0f;

    if (state()->isBoost()) {
        m_acceleration = m_boost.acceleration();
    } else {
        if (state()->isAccelerate()) {
            m_acceleration = calcVehicleAcceleration();
        }

        if (!state()->isBoost() && !state()->isAutoDrift()) {
            const auto &stats = param()->stats();

            f32 x = 1.0f - EGG::Mathf::abs(m_weightedTurn) * m_speedRatioCapped;
            m_speed *= stats.turningSpeed + (1.0f - stats.turningSpeed) * x;
        }
    }
}

f32 KartMove::calcVehicleAcceleration() const {
    f32 ratio = m_speed / m_softSpeedLimit;
    if (ratio < 0.0f) {
        return 1.0f;
    }

    std::vector<f32> as;
    std::vector<f32> ts;
    if (state()->isDrifting()) {
        const auto &as_arr = param()->stats().accelerationDriftA;
        const auto &ts_arr = param()->stats().accelerationDriftT;
        as = {as_arr.begin(), as_arr.end()};
        ts = {ts_arr.begin(), ts_arr.end()};
    } else {
        const auto &as_arr = param()->stats().accelerationStandardA;
        const auto &ts_arr = param()->stats().accelerationStandardT;
        as = {as_arr.begin(), as_arr.end()};
        ts = {ts_arr.begin(), ts_arr.end()};
    }

    size_t i = 0;
    f32 acceleration = 0.0f;
    f32 t_curr = 0.0f;
    for (; i < ts.size(); ++i) {
        if (ratio < ts[i]) {
            acceleration = as[i] + ((as[i + 1] - as[i]) / (ts[i] - t_curr)) * (ratio - t_curr);
            break;
        }

        t_curr = ts[i];
    }

    return i < ts.size() ? acceleration : as.back();
}

void KartMove::calcAcceleration() {
    m_lastSpeed = m_speed;
    m_speed += m_acceleration;

    f32 dVar17 = m_baseSpeed;
    f32 fVar1 = m_boost.multiplier() + getWheelieSoftSpeedLimitBonus();
    dVar17 *= fVar1;

    if (m_softSpeedLimit > dVar17) {
        // If not colliding with a wall
        m_softSpeedLimit = std::max(m_softSpeedLimit - 3.0f, dVar17);
    } else {
        m_softSpeedLimit = dVar17;
    }
    m_softSpeedLimit = std::min(m_hardSpeedLimit, m_softSpeedLimit);

    m_speed = std::min(m_softSpeedLimit, std::max(-m_softSpeedLimit, m_speed));

    m_speedRatioCapped = std::min(1.0f, EGG::Mathf::abs(m_speed / m_baseSpeed));

    EGG::Vector3f crossVec = m_smoothedUp.cross(m_dir);
    EGG::Matrix34f local_90;
    local_90.setAxisRotation(0.5f * DEG2RAD, crossVec);
    m_vel1Dir = local_90.multVector33(m_vel1Dir);
    EGG::Vector3f nextSpeed = m_speed * m_vel1Dir;
    dynamics()->setIntVel(dynamics()->intVel() + nextSpeed);
}

void KartMove::calcStandstillBoostRot() {
    f32 next = 0.0f;
    f32 scalar = 1.0f;

    if (state()->isTouchingGround()) {
        if (System::RaceManager::Instance()->stage() == System::RaceManager::Stage::Countdown) {
            next = 0.015f * -state()->startBoostCharge();
        } else {
            f32 speedDiff = m_lastSpeed - m_speed;
            if (-3.0f <= speedDiff) {
                scalar = std::min(3.0f, speedDiff);
            }

            next = (scalar * 0.15f) * 0.08f;
            scalar = 0.2f;
        }
    }

    m_standStillBoostRot += scalar * (next - m_standStillBoostRot);
}

f32 KartMove::getWheelieSoftSpeedLimitBonus() const {
    return 0.0f;
}

bool KartMove::canWheelie() const {
    return false;
}

void KartMove::applyStartBoost(s16 frames) {
    if (m_boost.activate(KartBoost::Type::AllMt, frames)) {
        state()->setBoost(true);
    }
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
    constexpr u16 WHEELIE_COOLDOWN = 20;

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
    f32 leanRotInc = LEAN_ROT_INC_RACE;
    f32 leanRotCap = LEAN_ROT_CAP_RACE;
    const auto *raceManager = System::RaceManager::Instance();

    if (!raceManager->isStageReached(System::RaceManager::Stage::Race) ||
            EGG::Mathf::abs(m_speed) < 50.0f) {
        leanRotInc = LEAN_ROT_INC_COUNTDOWN;
        leanRotCap = LEAN_ROT_CAP_COUNTDOWN;
    }

    m_leanRotCap += 0.3f * (leanRotCap - m_leanRotCap);
    m_leanRotInc += 0.3f * (leanRotInc - m_leanRotInc);

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

    if (System::RaceManager::Instance()->isStageReached(System::RaceManager::Stage::Race)) {
        m_leanRotInc = LEAN_ROT_INC_RACE;
        m_leanRotCap = LEAN_ROT_CAP_RACE;
    } else {
        m_leanRotInc = LEAN_ROT_INC_COUNTDOWN;
        m_leanRotCap = LEAN_ROT_CAP_COUNTDOWN;
    }
}

void KartMoveBike::init(bool b1, bool b2) {
    KartMove::init(b1, b2);

    m_leanRot = 0.0f;
    m_leanRotCap = 0.0f;
    m_leanRotInc = 0.0f;
    m_wheelieRot = 0.0f;
    m_wheelieCooldown = 0;
}

f32 KartMoveBike::getWheelieSoftSpeedLimitBonus() const {
    constexpr f32 WHEELIE_SPEED_BONUS = 0.15f;
    return state()->isWheelie() ? WHEELIE_SPEED_BONUS : 0.0f;
}

void KartMoveBike::calcWheelie() {
    constexpr u32 FAILED_WHEELIE_FRAMES = 15;
    constexpr u32 MAX_WHEELIE_FRAMES = 180;

    tryStartWheelie();
    --m_wheelieCooldown;
    m_wheelieCooldown = std::max<u16>(0, m_wheelieCooldown);

    if (state()->isWheelie()) {
        ++m_wheelieFrames;
        if (MAX_WHEELIE_FRAMES < m_wheelieFrames ||
                (!canWheelie() && FAILED_WHEELIE_FRAMES <= m_wheelieFrames)) {
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

f32 KartMoveBike::wheelieRotFactor() const {
    constexpr f32 WHEELIE_ROTATION_FACTOR = 0.2f;

    return state()->isWheelie() ? WHEELIE_ROTATION_FACTOR : 1.0f;
}

// Also handles cancelling wheelies
void KartMoveBike::tryStartWheelie() {
    bool dpadUp = inputs()->currentState().trickUp();

    if (!state()->isWheelie()) {
        if (dpadUp && state()->isTouchingGround()) {
            startWheelie();
        }
    }
}

f32 KartMoveBike::leanRot() const {
    return m_leanRot;
}

bool KartMoveBike::canWheelie() const {
    constexpr f32 WHEELIE_THRESHOLD = 0.3f;

    return m_speedRatioCapped >= WHEELIE_THRESHOLD && m_speed >= 0.0f;
}

} // namespace Kart
