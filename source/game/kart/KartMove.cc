#include "KartMove.hh"

#include "game/kart/KartCollide.hh"
#include "game/kart/KartDynamics.hh"
#include "game/kart/KartJump.hh"
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

/// @brief Rate at which the kart's leaning rotation increases during the race
static constexpr f32 LEAN_ROT_INC_RACE = 0.1f;
/// @brief Max leaning rotation during the race
static constexpr f32 LEAN_ROT_CAP_RACE = 1.0f;
/// @brief Rate at which the kart's leaning rotation increases during the countdown
static constexpr f32 LEAN_ROT_INC_COUNTDOWN = 0.08f;
/// @brief Max leaning rotation during the countdown
static constexpr f32 LEAN_ROT_CAP_COUNTDOWN = 0.6f;

/// @addr{0x80577FC4}
KartMove::KartMove() : m_smoothedUp(EGG::Vector3f::ey), m_scale(1.0f, 1.0f, 1.0f) {
    m_totalScale = 1.0f;
    m_bPadBoost = false;
    m_bRampBoost = false;
    m_bPadJump = false;
    m_bSsmtCharged = false;
    m_bSsmtLeeway = false;
    m_bWallBounce = false;
    m_jump = nullptr;
}

/// @addr{0x80587B78}
KartMove::~KartMove() = default;

/// @addr{0x8057821C}
void KartMove::createSubsystems() {
    m_jump = new KartJump(this);
}

/// @stage All
/// @brief Each frame, looks at player input and kart stats. Saves turn-related info.
/// @addr{0x8057A8B4}
void KartMove::calcTurn() {
    m_realTurn = 0.0f;
    m_rawTurn = 0.0f;

    if (!state()->isHop() || m_hopStickX == 0) {
        m_rawTurn = -state()->stickX();
        if (state()->isAirtimeOver20()) {
            m_rawTurn *= 0.01f;
        }
    } else {
        m_rawTurn = static_cast<f32>(m_hopStickX);
    }

    f32 reactivity;
    if (state()->isDrifting()) {
        reactivity = param()->stats().driftReactivity;
    } else {
        reactivity = param()->stats().handlingReactivity;
    }

    m_weightedTurn = m_rawTurn * reactivity + m_weightedTurn * (1.0f - reactivity);
    m_weightedTurn = std::max(-1.0f, std::min(1.0f, m_weightedTurn));

    m_realTurn = m_weightedTurn;

    if (!state()->isDrifting()) {
        return;
    }

    m_realTurn = (m_weightedTurn + static_cast<f32>(m_hopStickX)) * 0.5f;
    m_realTurn = m_realTurn * 0.8f + 0.2f * static_cast<f32>(m_hopStickX);
    m_realTurn = std::max(-1.0f, std::min(1.0f, m_realTurn));
}

/// @addr{0x8057829C}
void KartMove::setTurnParams() {
    static constexpr std::array<DriftingParameters, 3> DRIFTING_PARAMS_ARRAY = {{
            {10.0f, 0.5f, 0.5f, 1.0f},
            {10.0f, 0.5f, 0.5f, 0.2f},
            {10.0f, 0.22f, 0.5f, 0.2f},
    }};

    init(false, false);
    m_dir = bodyFront();
    m_lastDir = m_dir;
    m_vel1Dir = m_dir;
    m_landingDir = m_dir;
    m_driftingParams = &DRIFTING_PARAMS_ARRAY[static_cast<u32>(param()->stats().driftType)];
}

/// @addr{0x805784D4}
void KartMove::init(bool b1, bool b2) {
    m_lastSpeed = 0.0f;
    m_baseSpeed = param()->stats().speed;
    m_softSpeedLimit = param()->stats().speed;
    m_speed = 0.0f;
    setKartSpeedLimit();
    m_acceleration = 0.0f;
    m_speedDragMultiplier = 1.0f;
    m_up = EGG::Vector3f::ey;
    m_smoothedUp = EGG::Vector3f::ey;
    m_vel1Dir = EGG::Vector3f::ez;
    m_lastDir = EGG::Vector3f::ez;
    m_dir = EGG::Vector3f::ez;
    m_landingDir = EGG::Vector3f::ez;
    m_dirDiff = EGG::Vector3f::zero;
    m_hasLandingDir = false;
    m_landingAngle = 0.0f;
    m_speedRatioCapped = 0.0f;
    m_kclSpeedFactor = 1.0f;
    m_kclRotFactor = 1.0f;
    m_kclWheelSpeedFactor = 1.0f;
    m_kclWheelRotFactor = 1.0f;

    if (!b2) {
        m_floorCollisionCount = 0;
    }

    m_hopStickX = 0;
    m_hopFrame = 0;
    m_hopDir = EGG::Vector3f::ez;
    m_divingRot = 0.0f;
    m_standStillBoostRot = 0.0f;
    m_driftState = DriftState::NotDrifting;
    m_mtCharge = 0;
    m_offroadInvincibility = 0;
    m_ssmtCharge = 0;
    m_ssmtLeewayTimer = 0;
    m_ssmtDisableAccelTimer = 0;
    m_nonZipperAirtime = 0;
    m_realTurn = 0.0f;
    m_weightedTurn = 0.0f;

    if (!b1) {
        m_scale.set(1.0f);
        m_totalScale = 1.0f;
        m_mushroomBoostTimer = 0;
    }

    m_jumpPadMinSpeed = 0.0f;
    m_jumpPadMaxSpeed = 0.0f;
    m_jumpPadProperties = nullptr;
    m_rampBoost = 0;

    m_hopVelY = 0.0f;
    m_hopPosY = 0.0f;
    m_hopGravity = 0.0f;
    m_drivingDirection = DrivingDirection::Forwards;
    m_bPadBoost = false;
    m_bRampBoost = false;
    m_bPadJump = false;
    m_bSsmtCharged = false;
    m_bSsmtLeeway = false;
    m_bWallBounce = false;
    m_jump->reset();
    m_rawTurn = 0.0f;
}

/// @addr{0x8058974C}
f32 KartMove::leanRot() const {
    return 0.0f;
}

/// @brief Initializes the kart's position and rotation. Calls tire suspension initializers.
/// @addr{0x80584044}
void KartMove::setInitialPhysicsValues(const EGG::Vector3f &position, const EGG::Vector3f &angles) {
    EGG::Quatf quaternion;
    quaternion.setRPY(angles * DEG2RAD);
    EGG::Vector3f newPos = position;
    Field::CourseColMgr::CollisionInfo info;
    Field::KCLTypeMask kcl_flags = KCL_NONE;

    bool bColliding = Field::CollisionDirector::Instance()->checkSphereFullPush(100.0f, newPos,
            EGG::Vector3f::inf, KCL_ANY, &info, &kcl_flags, 0);

    if (bColliding && (kcl_flags & KCL_TYPE_FLOOR)) {
        newPos = newPos + info.tangentOff + (info.floorNrm * -100.0f);
        newPos += info.floorNrm * bsp().initialYPos;
    }

    setPos(newPos);
    setRot(quaternion);

    sub()->initPhysicsValues();

    physics()->setPos(pos());
    physics()->setVelocity(dynamics()->velocity());

    m_landingDir = bodyFront();
    m_dir = bodyFront();
    m_up = bodyUp();
    dynamics()->setTop(m_up);

    for (u16 tireIdx = 0; tireIdx < suspCount(); ++tireIdx) {
        suspension(tireIdx)->setInitialState();
    }
}

/// @addr{0x8057B9AC}
void KartMove::setKartSpeedLimit() {
    constexpr f32 LIMIT = 120.0f;
    m_hardSpeedLimit = LIMIT;
}

/// @stage All
/// @brief Each frame, calculates the kart's movement.
/// @addr{0x805788DC}
/// @details Calls various functions to handle drifts, hops, boosts.
/// Afterwards, calculates the kart's speed and rotation.
void KartMove::calc() {
    dynamics()->resetInternalVelocity();
    calcSsmtStart();
    calcTop();
    tryEndJumpPad();
    calcSpecialFloor();
    m_jump->calc();
    calcDirs();
    calcStickyRoad();
    calcOffroad();
    calcTurn();

    if (!state()->isAutoDrift()) {
        calcManualDrift();
    }

    calcWheelie();
    calcSsmt();
    calcBoost();
    calcMushroomBoost();
    calcOffroadInvincibility();
    calcVehicleSpeed();
    calcAcceleration();
    calcRotation();
}

/// @addr{0x8057D398}
void KartMove::calcTop() {
    f32 stabilizationFactor = 0.1f;
    m_hasLandingDir = false;
    EGG::Vector3f inputTop = state()->top();

    if (state()->isGroundStart() && m_nonZipperAirtime >= 3) {
        m_smoothedUp = inputTop;
        m_up = inputTop;
        m_landingDir = m_dir.perpInPlane(m_smoothedUp, true);
        m_dirDiff = m_landingDir.proj(m_landingDir);
        m_hasLandingDir = true;
    } else {
        if (state()->isHop() && m_hopPosY > 0.0f) {
            stabilizationFactor = m_driftingParams->stabilizationFactor;
        } else if (state()->isTouchingGround()) {
            if (state()->trickableTimer() > 0 && inputTop.dot(m_dir) > 0.0f && m_speed > 50.0f &&
                    collide()->isNotTrickable()) {
                inputTop = m_up;
            } else {
                m_up = inputTop;
            }

            f32 scalar = 0.8f;

            if (!state()->isBoost() && !state()->isWheelie()) {
                f32 topDotZ = 0.8f - 6.0f * (EGG::Mathf::abs(inputTop.dot(componentZAxis())));
                scalar = std::min(0.8f, std::max(0.3f, topDotZ));
            }

            m_smoothedUp += (inputTop - m_smoothedUp) * scalar;
            m_smoothedUp.normalise();

            f32 bodyDotFront = bodyFront().dot(m_smoothedUp);
            if (bodyDotFront < -0.1f) {
                stabilizationFactor += std::min(0.2f, EGG::Mathf::abs(bodyDotFront) * 0.5f);
            }

            if (collide()->isRampBoost()) {
                stabilizationFactor = 0.4f;
            }
        } else {
            calcAirtimeTop();
        }
    }

    dynamics()->setStabilizationFactor(stabilizationFactor);

    m_nonZipperAirtime = state()->airtime();
}

/// @addr{0x8057D888}
/// @brief Calculates rotation of the bike due to excessive airtime.
void KartMove::calcAirtimeTop() {
    if (!state()->isAirtimeOver20()) {
        return;
    }

    if (m_smoothedUp.y <= 0.99f) {
        m_smoothedUp += (EGG::Vector3f::ey - m_smoothedUp) * 0.03f;
        m_smoothedUp.normalise();
    } else {
        m_smoothedUp = EGG::Vector3f::ey;
    }

    if (m_up.y <= 0.99f) {
        m_up += (EGG::Vector3f::ey - m_up) * 0.03f;
        m_up.normalise();
    } else {
        m_up = EGG::Vector3f::ey;
    }
}

/// @stage 2
/// @brief Every frame, calculates any boost resulting from a boost panel.
/// @addr{0x80587590}
void KartMove::calcSpecialFloor() {
    const auto *raceMgr = System::RaceManager::Instance();
    if (!raceMgr->isStageReached(System::RaceManager::Stage::Race)) {
        return;
    }

    if (m_bPadBoost) {
        tryStartBoostPanel();
    }

    if (m_bRampBoost) {
        tryStartBoostRamp();
    }

    if (m_bPadJump) {
        tryStartJumpPad();
    }

    m_bPadBoost = false;
    m_bRampBoost = false;
    m_bPadJump = false;
}

/// @addr{0x8057A140}
void KartMove::calcDirs() {
    EGG::Vector3f right = dynamics()->mainRot().rotateVector(EGG::Vector3f::ex);
    EGG::Vector3f local_88 = right.cross(m_smoothedUp);
    local_88.normalise();

    if (!state()->isInATrick() &&
            (state()->isTouchingGround() || !state()->isRampBoost() ||
                    !m_jump->isBoostRampEnabled()) &&
            !state()->isJumpPad() && state()->airtime() <= 5) {
        if (state()->isHop()) {
            local_88 = m_hopDir;
        }

        EGG::Matrix34f mat;
        mat.setAxisRotation(m_landingAngle * DEG2RAD, m_smoothedUp);
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
    } else {
        m_vel1Dir = m_dir;
    }

    m_jump->tryStart(m_smoothedUp.cross(m_dir));

    if (m_hasLandingDir) {
        f32 dot = m_dir.dot(m_landingDir);
        EGG::Vector3f cross = m_dir.cross(m_landingDir);
        f32 crossDot = EGG::Mathf::sqrt(cross.dot());
        f32 angle = EGG::Mathf::atan2(crossDot, dot);
        angle = EGG::Mathf::abs(angle);

        f32 fVar4 = 1.0f;
        if (cross.dot(m_smoothedUp) < 0.0f) {
            fVar4 = -1.0f;
        }

        m_landingAngle += (angle * RAD2DEG) * fVar4;
    }

    if (m_landingAngle <= 0.0f) {
        if (m_landingAngle < 0.0f) {
            m_landingAngle = std::min(0.0f, m_landingAngle + 2.0f);
        }
    } else {
        m_landingAngle = std::max(0.0f, m_landingAngle - 2.0f);
    }
}

/// @addr{0x80583B88}
void KartMove::calcStickyRoad() {
    constexpr f32 STICKY_RADIUS = 200.0f;
    constexpr Field::KCLTypeMask STICKY_MASK = KCL_TYPE_BIT(COL_TYPE_STICKY_ROAD);

    if (!state()->isStickyRoad() || EGG::Mathf::abs(m_speed) <= 20.0f) {
        return;
    }

    EGG::Vector3f pos = dynamics()->pos();
    EGG::Vector3f vel = m_speed * m_vel1Dir;
    EGG::Vector3f down = -STICKY_RADIUS * componentYAxis();
    Field::CourseColMgr::CollisionInfo colInfo;
    colInfo.bbox.setZero();
    Field::KCLTypeMask kcl_flags = KCL_NONE;
    bool stickyRoad = false;

    for (size_t i = 0; i < 3; ++i) {
        EGG::Vector3f newPos = pos + vel;
        if (Field::CollisionDirector::Instance()->checkSphereFull(STICKY_RADIUS, newPos,
                    EGG::Vector3f::inf, STICKY_MASK, &colInfo, &kcl_flags, 0)) {
            m_vel1Dir = m_vel1Dir.perpInPlane(colInfo.floorNrm, true);
            stickyRoad = true;

            break;
        }
        vel *= 0.5f;
        pos += -STICKY_RADIUS * componentYAxis();
    }

    if (!stickyRoad) {
        state()->setStickyRoad(false);
    }
}

/// @stage 2
/// @brief Each frame, computes rotation and speed scalars from the floor KCL.
/// @addr{0x8057C3D4}
void KartMove::calcOffroad() {
    if (state()->isBoostOffroadInvincibility()) {
        m_kclSpeedFactor = 1.0f;
        m_kclRotFactor = param()->stats().kclRot[0];
    } else {
        bool anyWheel = state()->isAnyWheelCollision();
        if (anyWheel) {
            m_kclSpeedFactor = m_kclWheelSpeedFactor;
            m_floorCollisionCount = m_floorCollisionCount != 0 ? m_floorCollisionCount : 1;
            m_kclRotFactor = m_kclWheelRotFactor / static_cast<f32>(m_floorCollisionCount);
        }

        if (state()->isVehicleBodyFloorCollision()) {
            const CollisionData &colData = collisionData();
            if (anyWheel) {
                if (colData.speedFactor < m_kclWheelSpeedFactor) {
                    m_kclSpeedFactor = colData.speedFactor;
                }
                m_kclRotFactor = (m_kclWheelRotFactor + colData.rotFactor) /
                        static_cast<f32>(m_floorCollisionCount + 1);
            } else {
                m_kclSpeedFactor = colData.speedFactor;
                m_kclRotFactor = colData.rotFactor;
            }
        }
    }
}

/// @addr{0x80582694}
void KartMove::calcBoost() {
    if (m_boost.calc()) {
        state()->setAccelerate(true);
    } else {
        state()->setBoost(false);
    }

    calcRampBoost();
}

/// @addr{0x80582804}
void KartMove::calcRampBoost() {
    if (!state()->isRampBoost()) {
        return;
    }

    state()->setAccelerate(true);
    if (--m_rampBoost < 1) {
        m_rampBoost = 0;
        state()->setRampBoost(false);
    }
}

/// @addr{Inlined in 0x805828CC}
/// @stage 2
/// @brief Computes the current cooldown duration between braking and reversing.
void KartMove::calcDisableBackwardsAccel() {
    if (!state()->isDisableBackwardsAccel()) {
        return;
    }

    if (--m_ssmtDisableAccelTimer < 0 || (!m_bSsmtLeeway && !state()->isBrake())) {
        state()->setDisableBackwardsAccel(false);
        m_ssmtDisableAccelTimer = 0;
    }
}

/// @addr{0x805828CC}
/// @stage 2
/// @brief Calculates standstill mini-turbo components, if applicable.
void KartMove::calcSsmt() {
    constexpr s16 MAX_SSMT_CHARGE = 75;
    constexpr s16 SSMT_BOOST_FRAMES = 30;
    constexpr s16 LEEWAY_FRAMES = 1;
    constexpr s16 DISABLE_ACCEL_FRAMES = 20;

    calcDisableBackwardsAccel();

    if (state()->isChargingSsmt()) {
        if (++m_ssmtCharge > MAX_SSMT_CHARGE) {
            m_ssmtCharge = MAX_SSMT_CHARGE;
            m_bSsmtCharged = true;
            m_ssmtLeewayTimer = 0;
        }

        return;
    }

    m_ssmtCharge = 0;

    if (!m_bSsmtCharged) {
        return;
    }

    if (m_bSsmtLeeway) {
        if (--m_ssmtLeewayTimer < 0) {
            m_ssmtLeewayTimer = 0;
            m_bSsmtCharged = false;
            m_bSsmtLeeway = false;
            m_ssmtDisableAccelTimer = DISABLE_ACCEL_FRAMES;
            state()->setDisableBackwardsAccel(true);
        } else {
            if (!state()->isAccelerate() && !state()->isBrake()) {
                activateBoost(KartBoost::Type::AllMt, SSMT_BOOST_FRAMES);
                m_ssmtLeewayTimer = 0;
                m_bSsmtCharged = false;
                m_bSsmtLeeway = false;
            }
        }
    } else {
        if (state()->isAccelerate() && !state()->isBrake()) {
            activateBoost(KartBoost::Type::AllMt, SSMT_BOOST_FRAMES);
            m_ssmtLeewayTimer = 0;
            m_bSsmtCharged = false;
            m_bSsmtLeeway = false;
        } else {
            m_ssmtLeewayTimer = LEEWAY_FRAMES;
            m_bSsmtLeeway = true;
            state()->setDisableBackwardsAccel(true);
            m_ssmtDisableAccelTimer = LEEWAY_FRAMES;
        }
    }
}

/// @stage 2
/// @addr{0x8057E804}
/// @brief Each frame, checks for hop or slipdrift. Computes drift direction based on player input.
/// @return Whether or not we are hopping or slipdrifting.
bool KartMove::calcPreDrift() {
    if (!state()->isTouchingGround() && !state()->isHop() && !state()->isDriftManual()) {
        if (state()->isStickLeft() || state()->isStickRight()) {
            if (!state()->isDriftInput()) {
                state()->setSlipdriftCharge(false);
            } else if (!state()->isSlipdriftCharge()) {
                if (m_hopStickX == 0) {
                    if (state()->isStickRight()) {
                        m_hopStickX = -1;
                    } else if (state()->isStickLeft()) {
                        m_hopStickX = 1;
                    }
                    state()->setSlipdriftCharge(true);
                    onHop();
                }
            }
        }
    }

    if (state()->isHop()) {
        if (m_hopStickX == 0) {
            if (state()->isStickRight()) {
                m_hopStickX = -1;
            } else if (state()->isStickLeft()) {
                m_hopStickX = 1;
            }
        }
        if (m_hopFrame < 3) {
            ++m_hopFrame;
        }
    } else if (state()->isSlipdriftCharge()) {
        m_hopFrame = 0;
    }

    return state()->isHop() || state()->isSlipdriftCharge();
}

/// @stage All
/// @brief Clears drift state. Called when touching ground and drift is canceled.
/// @addr{0x8057EA50}
void KartMove::resetDriftManual() {
    m_hopStickX = 0;
    m_hopFrame = 0;
    state()->setHop(false);
    state()->setDriftManual(false);
    m_driftState = DriftState::NotDrifting;
    m_mtCharge = 0;
}

/// @stage 2
/// @brief Each frame, handles hopping, drifting, and mini-turbos.
/// @addr{0x8057DC44}
void KartMove::calcManualDrift() {
    bool isHopping = calcPreDrift();

    // TODO: Is this backwards/inverted?
    if (((!state()->isHop() || m_hopFrame < 3) && !state()->isSlipdriftCharge()) ||
            !state()->isTouchingGround()) {
        if (canHop()) {
            hop();
            isHopping = true;
        }
    } else {
        startManualDrift();
        isHopping = false;
    }

    if (!state()->isDriftManual()) {
        if (!isHopping && state()->isTouchingGround()) {
            resetDriftManual();
        }
    } else {
        if (!state()->isDriftInput() || !state()->isAccelerate()) {
            releaseMt();
            resetDriftManual();
        } else {
            controlOutsideDriftAngle();
        }
    }
}

/// @stage 2
/// @brief Called when the player lands from a drift hop, or to start a slipdrift.
/// @addr{0x8057E3F4}
void KartMove::startManualDrift() {
    state()->setHop(false);
    state()->setSlipdriftCharge(false);

    if (!state()->isDriftInput()) {
        return;
    }

    if (getAppliedHopStickX() == 0) {
        return;
    }

    state()->setDriftManual(true);
    state()->setHop(false);
    m_driftState = DriftState::ChargingMt;
}

/// @stage 2
/// @brief Stops charging a mini-turbo, and applies boost if charged.
/// @addr{0x80582F9C}
void KartMove::releaseMt() {
    if (m_driftState == DriftState::ChargingMt) {
        m_driftState = DriftState::NotDrifting;
        return;
    }

    u16 mtLength = param()->stats().miniTurbo;

    if (m_driftState == DriftState::ChargedSmt) {
        K_PANIC("Not implemented yet");
    }

    activateBoost(KartBoost::Type::AllMt, mtLength);

    m_driftState = DriftState::NotDrifting;
}

/// @stage 2
/// @brief Every frame, handles mini-turbo charging and outside drifting bike rotation.
/// @addr{0x8057EAB8}
void KartMove::controlOutsideDriftAngle() {
    if (state()->airtime() > 5) {
        return;
    }

    if (param()->stats().driftType != KartParam::Stats::DriftType::Inside_Drift_Bike) {
        K_PANIC("Not implemented yet!");
    }

    calcMtCharge();
}

/// @stage 1+
/// @brief Every frame, calculates kart rotation based on player input.
/// @addr{0x8057C69C}
void KartMove::calcRotation() {
    f32 turn;
    bool drifting = state()->isDrifting();

    if (drifting) {
        turn = param()->stats().driftManualTightness;
    } else {
        turn = param()->stats().handlingManualTightness;
    }

    bool forwards = true;
    if (state()->isBrake() && m_speed <= 0.0f) {
        forwards = false;
    }

    turn *= m_realTurn;
    if (state()->isChargingSsmt()) {
        turn = m_realTurn * 0.04f;
    } else {
        if (state()->isHop() && m_hopPosY > 0.0f) {
            turn *= 1.4f;
        }

        if (!drifting) {
            bool noTurn = false;
            if (!state()->isWallCollision() && EGG::Mathf::abs(m_speed) < 1.0f) {
                if (!(state()->isHop() && m_hopPosY > 0.0f)) {
                    turn = 0.0f;
                    noTurn = true;
                }
            }
            if (forwards && !noTurn) {
                if (m_speed >= 20.0f) {
                    turn *= 0.5f;
                    if (m_speed < 70.0f) {
                        turn += (1.0f - (m_speed - 20.0f) / 50.0f) * turn;
                    }
                } else {
                    turn = (turn * 0.4f) + (m_speed / 20.0f) * (turn * 0.6f);
                }
            }
        }

        if (!forwards) {
            turn = -turn;
        }
    }

    if (!state()->isTouchingGround()) {
        if (state()->isRampBoost() && m_jump->isBoostRampEnabled()) {
            turn = 0.0f;
        } else {
            u32 airtime = state()->airtime();
            if (airtime >= 70) {
                turn = 0.0f;
            } else if (airtime >= 30) {
                turn = std::max(0.0f, turn * (1.0f - (airtime - 30) * 0.025f));
            }
        }
    }

    calcVehicleRotation(turn);
}

/// @stage 2
/// @brief Every frame, computes speed based on acceleration and any active boosts.
/// @addr{0x8057AB68}
void KartMove::calcVehicleSpeed() {
    const auto *raceMgr = System::RaceManager::Instance();
    if (raceMgr->isStageReached(System::RaceManager::Stage::Race)) {
        f32 speedFix = dynamics()->speedFix();
        if ((state()->isWallCollisionStart() || state()->wallBonkTimer() == 0 ||
                    EGG::Mathf::abs(speedFix) >= 3.0f) &&
                !state()->isDriftManual()) {
            m_speed += speedFix;
        }
    }

    if (m_speed < -20.0f) {
        m_speed += 0.5f;
    }

    m_acceleration = 0.0f;
    m_speedDragMultiplier = 1.0f;

    if (!state()->isTouchingGround() || state()->isChargingSsmt()) {
        if (state()->isRampBoost() && state()->airtime() < 4) {
            m_acceleration = 7.0f;
        } else {
            if (state()->isJumpPad() && !state()->isAccelerate()) {
                m_speedDragMultiplier = 0.99f;
            } else {
                if (state()->airtime() > 5) {
                    m_speedDragMultiplier = 0.999f;
                }
            }
            m_speed *= m_speedDragMultiplier;
        }

    } else if (state()->isBoost()) {
        m_acceleration = m_boost.acceleration();
    } else {
        if (!state()->isJumpPad() && !state()->isRampBoost()) {
            if (state()->isAccelerate()) {
                m_acceleration = calcVehicleAcceleration();
            } else {
                if (!state()->isBrake() || state()->isDisableBackwardsAccel()) {
                    m_speed *= m_speed > 0.0f ? 0.98f : 0.95f;
                } else if (m_drivingDirection == DrivingDirection::Braking) {
                    m_acceleration = -1.5f;
                } else if (m_drivingDirection == DrivingDirection::WaitingForBackwards) {
                    if (++m_backwardsAllowCounter > 15) {
                        m_drivingDirection = DrivingDirection::Backwards;
                    }
                } else if (m_drivingDirection == DrivingDirection::Backwards) {
                    m_acceleration = -2.0f;
                }
            }

            if (!state()->isBoost() && !state()->isDriftManual() && !state()->isAutoDrift()) {
                const auto &stats = param()->stats();

                f32 x = 1.0f - EGG::Mathf::abs(m_weightedTurn) * m_speedRatioCapped;
                m_speed *= stats.turningSpeed + (1.0f - stats.turningSpeed) * x;
            }
        } else {
            m_acceleration = 7.0f;
        }
    }
}

/// @stage 2
/// @brief Every frame, computes acceleration based off the character/vehicle stats.
/// @addr{0x8057B868}
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

/// @stage 2
/// @brief Every frame, applies acceleration to the kart's internal velocity.
/// @addr{0x8057B9BC}
void KartMove::calcAcceleration() {
    constexpr f32 ROTATION_SCALAR_NORMAL = 0.5f;
    constexpr f32 ROTATION_SCALAR_MIDAIR = 0.2f;
    constexpr f32 ROTATION_SCALAR_BOOST_RAMP = 4.0f;

    m_lastSpeed = m_speed;

    if (m_acceleration < 0.0f) {
        if (m_speed < -20.0f) {
            m_acceleration = 0.0f;
        } else {
            if (m_speed + m_acceleration <= -20.0f) {
                m_acceleration = -20.0f - m_speed;
            }
        }
    }

    m_speed += m_acceleration;

    if (state()->isChargingSsmt()) {
        m_speed *= 0.8f;
    } else {
        if (m_drivingDirection == DrivingDirection::Braking && m_speed < 0.0f) {
            m_speed = 0.0f;
            m_drivingDirection = DrivingDirection::WaitingForBackwards;
            m_backwardsAllowCounter = 0;
        }
    }

    f32 dVar17 = state()->isJumpPad() ? m_jumpPadMaxSpeed : m_baseSpeed;
    dVar17 *= (m_boost.multiplier() + getWheelieSoftSpeedLimitBonus()) * m_kclSpeedFactor;
    dVar17 = std::max(dVar17, m_boost.speedLimit() * m_kclSpeedFactor);

    if (state()->isRampBoost()) {
        dVar17 = std::max(dVar17, 100.0f);
    }

    m_lastDir = (m_speed > 0.0f) ? 1.0f * m_dir : -1.0f * m_dir;

    f32 local_c8 = 1.0f;
    dVar17 *= calcWallCollisionSpeedFactor(local_c8);

    if (!state()->isWallCollision()) {
        m_softSpeedLimit = std::max(m_softSpeedLimit - 3.0f, dVar17);
    } else {
        m_softSpeedLimit = dVar17;
    }

    m_softSpeedLimit = std::min(m_hardSpeedLimit, m_softSpeedLimit);

    m_speed = std::min(m_softSpeedLimit, std::max(-m_softSpeedLimit, m_speed));

    if (state()->isJumpPad()) {
        m_speed = std::max(m_speed, m_jumpPadMinSpeed);
    }

    calcWallCollisionStart(local_c8);

    m_speedRatioCapped = std::min(1.0f, EGG::Mathf::abs(m_speed / m_baseSpeed));

    EGG::Vector3f crossVec = m_smoothedUp.cross(m_dir);
    if (m_speed < 0.0f) {
        crossVec = -crossVec;
    }

    f32 rotationScalar = ROTATION_SCALAR_NORMAL;
    if (collide()->isRampBoost()) {
        rotationScalar = ROTATION_SCALAR_BOOST_RAMP;
    } else if (!state()->isTouchingGround()) {
        rotationScalar = ROTATION_SCALAR_MIDAIR;
    }

    EGG::Matrix34f local_90;
    local_90.setAxisRotation(rotationScalar * DEG2RAD, crossVec);
    m_vel1Dir = local_90.multVector33(m_vel1Dir);
    m_processedSpeed = m_speed;
    EGG::Vector3f nextSpeed = m_speed * m_vel1Dir;
    dynamics()->setIntVel(dynamics()->intVel() + nextSpeed);

    if (state()->isTouchingGround() && !state()->isDriftManual() && !state()->isHop()) {
        if (state()->isBrake()) {
            if (m_drivingDirection == DrivingDirection::Forwards) {
                m_drivingDirection = m_processedSpeed > 5.0f ? DrivingDirection::Braking :
                                                               DrivingDirection::Backwards;
            }
        } else {
            if (m_processedSpeed >= 0.0f) {
                m_drivingDirection = DrivingDirection::Forwards;
            }
        }
    } else {
        m_drivingDirection = DrivingDirection::Forwards;
    }
}

/// @addr{0x8057B108}
/// @stage 2
/// @brief Every frame, computes a speed scalar if we are colliding with a wall.
f32 KartMove::calcWallCollisionSpeedFactor(f32 &f1) {
    if (!state()->isWallCollision()) {
        return 1.0f;
    }

    onWallCollision();

    EGG::Vector3f wallNrm = collisionData().wallNrm;
    if (wallNrm.y > 0.0f) {
        wallNrm.y = 0.0f;
        wallNrm.normalise();
    }

    f32 dot = m_lastDir.dot(wallNrm);

    if (dot < 0.0f) {
        f1 = std::max(0.0f, dot + 1.0f);
        return std::min(1.0f, f1 * 0.4f);
    }

    return 1.0f;
}

/// @addr{0x8057B2A0}
/// @stage 2
/// @brief If we started to collide with a wall this frame, applies rotation.
void KartMove::calcWallCollisionStart(f32 param_2) {
    m_bWallBounce = false;

    if (!state()->isWallCollisionStart()) {
        return;
    }

    m_dir = bodyFront();
    m_vel1Dir = m_dir;
    m_landingDir = m_dir;

    if (param_2 < 0.9f) {
        f32 speedDiff = m_lastSpeed - m_speed;

        if (speedDiff > 30.0f) {
            m_bWallBounce = true;
            const CollisionData &colData = collisionData();
            EGG::Vector3f newPos = colData.relPos + pos();
            f32 dot = -bodyUp().dot(colData.relPos) * 0.5f;
            EGG::Vector3f scaledUp = dot * bodyUp();
            newPos -= scaledUp;

            speedDiff = std::min(60.0f, speedDiff);
            EGG::Vector3f scaledWallNrm = speedDiff * colData.wallNrm;

            auto [proj, rej] = scaledWallNrm.projAndRej(m_vel1Dir);
            proj *= 0.3f;
            rej *= 0.9f;

            if (state()->isBoost()) {
                proj = EGG::Vector3f::zero;
                rej = EGG::Vector3f::zero;
            }

            if (bodyFront().dot(colData.wallNrm) > 0.0f) {
                proj = EGG::Vector3f::zero;
            }
            rej *= 0.9f;

            EGG::Vector3f projRejSum = proj + rej;
            f32 bumpDeviation =
                    state()->isTouchingGround() ? param()->stats().bumpDeviationLevel : 0.0f;

            dynamics()->applyWrenchScaled(newPos, projRejSum, bumpDeviation);
        }
    }
}

/// @stage 1+
/// @brief STAGE Computes the x-component of angular velocity based on the kart's speed.
/// @addr{0x8057D1D4}
void KartMove::calcStandstillBoostRot() {
    f32 next = 0.0f;
    f32 scalar = 1.0f;

    if (state()->isTouchingGround()) {
        if (System::RaceManager::Instance()->stage() == System::RaceManager::Stage::Countdown) {
            next = 0.015f * -state()->startBoostCharge();
        } else if (!state()->isChargingSsmt()) {
            if (!state()->isJumpPad() && !state()->isRampBoost() && !state()->isSoftWallDrift()) {
                f32 speedDiff = m_lastSpeed - m_speed;
                scalar = std::min(3.0f, std::max(speedDiff, -3.0f));

                if (state()->isMushroomBoost()) {
                    next = (scalar * 0.15f) * 0.25f;
                    if (state()->isWheelie()) {
                        next *= 0.5f;
                    }
                } else {
                    next = (scalar * 0.15f) * 0.08f;
                }
                scalar = m_driftingParams->boostRotFactor;
            }
        } else {
            constexpr s16 MAX_SSMT_CHARGE = 75;
            next = 0.015f * (-static_cast<f32>(m_ssmtCharge) / static_cast<f32>(MAX_SSMT_CHARGE));
        }
    }

    if (m_bWallBounce) {
        m_standStillBoostRot = isBike() ? next * 3.0f : next * 10.0f;
    } else {
        m_standStillBoostRot += scalar * (next - m_standStillBoostRot);
    }
}

/// @stage 2
/// @brief Responds to player input to handle up/down kart tilt mid-air.
/// @addr{0x805869DC}
void KartMove::calcDive() {
    constexpr f32 DIVE_LIMIT = 0.8f;

    m_divingRot *= 0.96f;

    if (state()->isTouchingGround()) {
        return;
    }

    f32 stickY = state()->stickY();

    if (state()->isInATrick() && m_jump->type() == TrickType::BikeSideStuntTrick) {
        stickY = std::min(1.0f, stickY + 0.4f);
    }

    u32 airtime = state()->airtime();

    if (airtime > 50) {
        if (EGG::Mathf::abs(stickY) < 0.1f) {
            m_divingRot += 0.05f * (-0.025f - m_divingRot);
        }
    } else {
        stickY *= (airtime / 50.0f);
    }

    m_divingRot = std::max(-DIVE_LIMIT, std::min(DIVE_LIMIT, m_divingRot + stickY * 0.005f));

    EGG::Vector3f angVel2 = dynamics()->angVel2();
    angVel2.x += m_divingRot;
    dynamics()->setAngVel2(angVel2);

    if (state()->airtime() < 50) {
        return;
    }

    EGG::Vector3f topRotated = dynamics()->mainRot().rotateVector(EGG::Vector3f::ey);
    EGG::Vector3f forwardRotated = dynamics()->mainRot().rotateVector(EGG::Vector3f::ez);
    f32 upDotTop = m_up.dot(topRotated);
    EGG::Vector3f upCrossTop = m_up.cross(topRotated);
    f32 crossNorm = EGG::Mathf::sqrt(upCrossTop.dot());
    f32 angle = EGG::Mathf::abs(EGG::Mathf::atan2(crossNorm, upDotTop));

    f32 fVar1 = angle * RAD2DEG - 20.0f;
    if (fVar1 <= 0.0f) {
        return;
    }

    f32 mult = std::min(1.0f, fVar1 / 20.0f);
    if (forwardRotated.y > 0.0f) {
        dynamics()->setGravity((1.0f - 0.2f * mult) * dynamics()->gravity());
    } else {
        dynamics()->setGravity((0.2f * mult + 1.0f) * dynamics()->gravity());
    }
}

/// @addr{Inlined in 0x805788DC}
/// @stage 2
/// @brief Calculates whether we are starting a standstill mini-turbo.
void KartMove::calcSsmtStart() {
    if (EGG::Mathf::abs(m_speed) >= 10.0f || state()->isBoost() || state()->isRampBoost() ||
            !state()->isAccelerate() || !state()->isBrake()) {
        state()->setChargingSsmt(false);
        return;
    }

    state()->setChargingSsmt(true);
    state()->setHopStart(false);
    state()->setDriftInput(false);
}

/// @addr{0x80579968}
void KartMove::calcHopPhysics() {
    m_hopVelY = m_hopVelY * 0.998f + m_hopGravity;
    m_hopPosY += m_hopVelY;

    if (m_hopPosY < 0.0f) {
        m_hopPosY = 0.0f;
        m_hopVelY = 0.0f;
    }
}

/// @addr{0x8057CF0C}
/// @brief Every frame, calculates rotation, EV, and angular velocity for the kart.
void KartMove::calcVehicleRotation(f32 turn) {
    f32 tiltMagnitude = 0.0f;

    if (state()->isAnyWheelCollision()) {
        EGG::Vector3f front = componentZAxis();
        front = front.perpInPlane(m_up, true);
        EGG::Vector3f frontSpeed = velocity().rej(front).perpInPlane(m_up, false);
        f32 dot = frontSpeed.dot();

        f32 magnitude = tiltMagnitude;
        if (dot > FLT_EPSILON) {
            magnitude = front.length();

            if (front.z * frontSpeed.x - front.x * frontSpeed.z > 0.0f) {
                magnitude = -magnitude;
            }

            tiltMagnitude = -1.0f;
            if (-1.0f <= magnitude) {
                tiltMagnitude = std::min(1.0f, magnitude);
            }
        }
    } else if (!state()->isHop() || m_hopPosY <= 0.0f) {
        EGG::Vector3f angVel0 = dynamics()->angVel0();
        angVel0.z *= 0.98f;
        dynamics()->setAngVel0(angVel0);
    }

    f32 lean = EGG::Mathf::abs(m_weightedTurn) * (tiltMagnitude * param()->stats().tilt);

    calcStandstillBoostRot();

    EGG::Vector3f angVel0 = dynamics()->angVel0();
    angVel0.x += m_standStillBoostRot;
    angVel0.z += lean;
    dynamics()->setAngVel0(angVel0);

    EGG::Vector3f angVel2 = dynamics()->angVel2();
    angVel2.y += turn;
    dynamics()->setAngVel2(angVel2);
}

/// @stage 2
/// @brief Initializes hop information, resets upwards EV and clears upwards force.
/// @addr{0x8057DA5C}
void KartMove::hop() {
    state()->setHop(true);
    state()->setDriftManual(false);
    onHop();

    m_hopDir = dynamics()->mainRot().rotateVector(EGG::Vector3f::ez);
    m_driftState = DriftState::NotDrifting;
    m_mtCharge = 0;
    m_hopStickX = 0;
    m_hopFrame = 0;
    m_hopPosY = 0.0f;
    m_hopGravity = dynamics()->gravity();
    m_hopVelY = m_driftingParams->hopVelY;

    EGG::Vector3f extVel = dynamics()->extVel();
    extVel.y = 0.0f + m_hopVelY;
    dynamics()->setExtVel(extVel);

    EGG::Vector3f totalForce = dynamics()->totalForce();
    totalForce.y = 0.0f;
    dynamics()->setTotalForce(totalForce);
}

/// @stage 2
/// @brief Returns the % speed boost from wheelies. For karts, this is always 0.
/// @addr{0x8057C3C8}
f32 KartMove::getWheelieSoftSpeedLimitBonus() const {
    return 0.0f;
}

/// @addr{0x8058758C}
bool KartMove::canWheelie() const {
    return false;
}

/// @addr{0x8057DA18}
bool KartMove::canHop() const {
    if (!state()->isHopStart() || !state()->isTouchingGround()) {
        return false;
    }

    return true;
}

/// @addr{Inlined at 0x80587590}
void KartMove::tryStartBoostPanel() {
    constexpr s16 BOOST_PANEL_DURATION = 60;

    activateBoost(KartBoost::Type::MushroomAndBoostPanel, BOOST_PANEL_DURATION);
    setOffroadInvincibility(BOOST_PANEL_DURATION);
}

/// @stage 2
/// @brief Sets offroad invincibility and and enables the ramp boost bitfield flag.
/// @addr{Inlined at 0x80587590}
void KartMove::tryStartBoostRamp() {
    constexpr s16 BOOST_RAMP_DURATION = 60;

    state()->setRampBoost(true);
    m_rampBoost = BOOST_RAMP_DURATION;
    setOffroadInvincibility(BOOST_RAMP_DURATION);
}

/// @stage 2
/// @brief Applies calculations to start interacting with a @ref COL_TYPE_JUMP_PAD "jump pad".
/// @addr{0x8057FD18}
/// @details If applicable, updates @ref KartDynamics::m_extVel "external velocity"
void KartMove::tryStartJumpPad() {
    static constexpr std::array<JumpPadProperties, 8> JUMP_PAD_PROPERTIES = {{
            {50.0f, 50.0f, 35.0f},
            {50.0f, 50.0f, 47.0f},
            {59.0f, 59.0f, 30.0f},
            {73.0f, 73.0f, 45.0f},
            {73.0f, 73.0f, 53.0f},
            {55.0f, 55.0f, 35.0f},
            {56.0f, 56.0f, 50.0f},
    }};

    state()->setJumpPad(true);
    s32 jumpPadVariant = state()->jumpPadVariant();
    m_jumpPadProperties = &JUMP_PAD_PROPERTIES[jumpPadVariant];

    if (jumpPadVariant != 4) {
        EGG::Vector3f extVel = dynamics()->extVel();
        EGG::Vector3f totalForce = dynamics()->totalForce();

        extVel.y = m_jumpPadProperties->velY;
        totalForce.y = 0.0f;

        dynamics()->setExtVel(extVel);
        dynamics()->setTotalForce(totalForce);

        if (jumpPadVariant != 3) {
            EGG::Vector3f dir = m_dir;
            dir.y = 0.0f;
            dir.normalise();
            m_speed *= m_dir.dot(dir);
            m_dir = dir;
            m_vel1Dir = dir;
            state()->setJumpPadDisableYsusForce(true);
        }
    }

    m_jumpPadMinSpeed = m_jumpPadProperties->minSpeed;
    m_jumpPadMaxSpeed = m_jumpPadProperties->maxSpeed;
    m_speed = std::max(m_speed, m_jumpPadMinSpeed);
}

/// @addr{0x80582530}
void KartMove::tryEndJumpPad() {
    if (state()->isGroundStart()) {
        cancelJumpPad();
    }
}

/// @addr{0x80582DB4}
void KartMove::cancelJumpPad() {
    m_jumpPadMinSpeed = 0.0f;
    state()->setJumpPad(false);
}

/// @addr{0x8057F090}
void KartMove::activateBoost(KartBoost::Type type, s16 frames) {
    if (m_boost.activate(type, frames)) {
        state()->setBoost(true);
    }
}

/// @addr{0x8058212C}
void KartMove::applyStartBoost(s16 frames) {
    activateBoost(KartBoost::Type::AllMt, frames);
}

/// @addr{0x8057F3D8}
void KartMove::activateMushroom() {
    constexpr s16 MUSHROOM_DURATION = 90;
    activateBoost(KartBoost::Type::MushroomAndBoostPanel, MUSHROOM_DURATION);

    m_mushroomBoostTimer = MUSHROOM_DURATION;
    state()->setMushroomBoost(true);
    setOffroadInvincibility(MUSHROOM_DURATION);
}

/// @stage 2
/// @brief Ignores offroad KCL collision for a set amount of time.
/// @addr{0x805824C8}
/// @param timer Framecount to ignore offroad
void KartMove::setOffroadInvincibility(s16 timer) {
    if (timer > m_offroadInvincibility) {
        m_offroadInvincibility = timer;
    }

    state()->setBoostOffroadInvincibility(true);
}

/// @stage 2
/// @brief Checks a timer to see if we are still ignoring offroad slowdown.
/// @addr{0x805824F0}
void KartMove::calcOffroadInvincibility() {
    if (!state()->isBoostOffroadInvincibility()) {
        return;
    }

    if (--m_offroadInvincibility > 0) {
        return;
    }

    state()->setBoostOffroadInvincibility(false);
}

/// @stage 2
/// @brief Checks a timer to see if we are still boosting from a mushroom.
void KartMove::calcMushroomBoost() {
    if (!state()->isMushroomBoost()) {
        return;
    }

    if (--m_mushroomBoostTimer > 0) {
        return;
    }

    state()->setMushroomBoost(false);
}

/// @addr{0x8057F7A8}
void KartMove::landTrick() {
    static constexpr std::array<s16, 3> KART_TRICK_BOOST_DURATION = {{
            40,
            70,
            85,
    }};
    static constexpr std::array<s16, 3> BIKE_TRICK_BOOST_DURATION = {{
            45,
            80,
            95,
    }};

    s16 duration;
    if (isBike()) {
        duration = BIKE_TRICK_BOOST_DURATION[static_cast<u32>(m_jump->variant())];
    } else {
        duration = KART_TRICK_BOOST_DURATION[static_cast<u32>(m_jump->variant())];
    }

    activateBoost(KartBoost::Type::TrickAndZipper, duration);
}

void KartMove::setDir(const EGG::Vector3f &v) {
    m_dir = v;
}

void KartMove::setVel1Dir(const EGG::Vector3f &v) {
    m_vel1Dir = v;
}

void KartMove::setFloorCollisionCount(u16 count) {
    m_floorCollisionCount = count;
}

void KartMove::setKCLWheelSpeedFactor(f32 val) {
    m_kclWheelSpeedFactor = val;
}

void KartMove::setKCLWheelRotFactor(f32 val) {
    m_kclWheelRotFactor = val;
}

void KartMove::setPadBoost(bool isSet) {
    m_bPadBoost = isSet;
}

void KartMove::setRampBoost(bool isSet) {
    m_bRampBoost = isSet;
}

void KartMove::setPadJump(bool isSet) {
    m_bPadJump = isSet;
}

/// @brief Factors in vehicle speed to retrieve our hop direction and magnitude.
/// @addr{0x8057EFF8}
/// @return 0.0f if we are too slow to drift, otherwise the hop direction.
s32 KartMove::getAppliedHopStickX() const {
    constexpr f32 MIN_DRIFT_THRESHOLD = 0.55f;

    if (MIN_DRIFT_THRESHOLD * m_baseSpeed > m_speed) {
        return 0;
    }

    return m_hopStickX;
}

f32 KartMove::softSpeedLimit() const {
    return m_softSpeedLimit;
}

f32 KartMove::speed() const {
    return m_speed;
}

f32 KartMove::acceleration() const {
    return m_acceleration;
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

const EGG::Vector3f &KartMove::up() const {
    return m_up;
}

f32 KartMove::totalScale() const {
    return m_totalScale;
}

const EGG::Vector3f &KartMove::dir() const {
    return m_dir;
}

const EGG::Vector3f &KartMove::vel1Dir() const {
    return m_vel1Dir;
}

f32 KartMove::speedRatioCapped() const {
    return m_speedRatioCapped;
}

u16 KartMove::floorCollisionCount() const {
    return m_floorCollisionCount;
}

s32 KartMove::hopStickX() const {
    return m_hopStickX;
}

KartJump *KartMove::jump() const {
    return m_jump;
}

/// @addr{0x80587B30}
KartMoveBike::KartMoveBike() : m_leanRot(0.0f) {}

/// @addr{0x80589704}
KartMoveBike::~KartMoveBike() = default;

/// @brief STAGE 1+ - Sets the wheelie bit flag and some wheelie-related variables.
/// @addr{0x80588350}
void KartMoveBike::startWheelie() {
    constexpr f32 MAX_WHEELIE_ROTATION = 0.07f;
    constexpr u16 WHEELIE_COOLDOWN = 20;

    state()->setWheelie(true);
    m_wheelieFrames = 0;
    m_maxWheelieRot = MAX_WHEELIE_ROTATION;
    m_wheelieCooldown = WHEELIE_COOLDOWN;
    m_wheelieRotDec = 0.0f;
}

/// @addr{0x805883C4}
/// @stage 1+
/// @brief Clears the wheelie bit flag and resets the rotation decrement.
void KartMoveBike::cancelWheelie() {
    state()->setWheelie(false);
    m_wheelieRotDec = 0.0f;
}

/// @addr{0x80587BB8}
void KartMoveBike::createSubsystems() {
    m_jump = new KartJumpBike(this);
}

/// @stage All
/// @brief Every frame, calculates rotation, EV, and angular velocity for the bike.
/// @addr{0x80587D68}
void KartMoveBike::calcVehicleRotation(f32 turn) {
    f32 leanRotInc = m_turningParams->leanRotIncRace;
    f32 leanRotCap = m_turningParams->leanRotCapRace;
    const auto *raceManager = System::RaceManager::Instance();

    if (!state()->isChargingSsmt()) {
        if (!raceManager->isStageReached(System::RaceManager::Stage::Race) ||
                EGG::Mathf::abs(m_speed) < 5.0f) {
            leanRotInc = m_turningParams->leanRotIncCountdown;
            leanRotCap = m_turningParams->leanRotCapCountdown;
        }
    } else {
        leanRotInc = m_turningParams->leanRotIncSSMT;
        leanRotCap = m_turningParams->leanRotCapSSMT;
    }

    m_leanRotCap += 0.3f * (leanRotCap - m_leanRotCap);
    m_leanRotInc += 0.3f * (leanRotInc - m_leanRotInc);

    f32 stickX = state()->stickX();
    f32 extVelXFactor = 0.0f;
    f32 leanRotMin = -m_leanRotCap;
    f32 leanRotMax = m_leanRotCap;

    if (state()->isWheelie() || state()->isAirtimeOver20() || state()->isSoftWallDrift() ||
            state()->isSomethingWallCollision()) {
        m_leanRot *= m_turningParams->leanRotDecayFactor;
    } else if (!state()->isDrifting()) {
        if (stickX <= 0.2f) {
            if (stickX >= -0.2f) {
                m_leanRot *= m_turningParams->leanRotDecayFactor;
            } else {
                m_leanRot -= m_leanRotInc;
                extVelXFactor = m_turningParams->leanRotShallowFactor;
            }
        } else {
            m_leanRot += m_leanRotInc;
            extVelXFactor = -m_turningParams->leanRotShallowFactor;
        }
    } else {
        leanRotMax = m_turningParams->leanRotMaxDrift;
        leanRotMin = m_turningParams->leanRotMinDrift;

        if (m_hopStickX == 1) {
            leanRotMin = -leanRotMax;
            leanRotMax = -m_turningParams->leanRotMinDrift;
        }
        if (m_hopStickX == -1) {
            if (stickX == 0.0f) {
                m_leanRot += (0.5f - m_leanRot) * 0.05f;
            } else {
                m_leanRot += m_turningParams->driftStickXFactor * stickX;
                extVelXFactor = -m_turningParams->leanRotShallowFactor * stickX;
            }
        } else if (stickX == 0.0f) {
            m_leanRot += (-0.5f - m_leanRot) * 0.05f;
        } else {
            m_leanRot += m_turningParams->driftStickXFactor * stickX;
            extVelXFactor = -m_turningParams->leanRotShallowFactor * stickX;
        }
    }

    bool capped = false;
    if (leanRotMin <= m_leanRot) {
        if (leanRotMax < m_leanRot) {
            m_leanRot = leanRotMax;
            capped = true;
        }
    } else {
        m_leanRot = leanRotMin;
        capped = true;
    }

    if (!capped) {
        dynamics()->setExtVel(dynamics()->extVel() + componentXAxis() * extVelXFactor);
    }

    f32 leanRotScalar = state()->isDrifting() ? 0.065f : 0.05f;

    calcStandstillBoostRot();

    dynamics()->setAngVel2(dynamics()->angVel2() +
            EGG::Vector3f(m_standStillBoostRot, turn * wheelieRotFactor(),
                    m_leanRot * leanRotScalar));

    calcDive();

    f32 scalar = (m_speed >= 0.0f) ? m_speedRatioCapped * 2.0f : 0.0f;
    scalar = std::min(1.0f, scalar);

    EGG::Vector3f top = scalar * m_up + (1.0f - scalar) * EGG::Vector3f::ey;
    if (FLT_EPSILON < top.dot()) {
        top.normalise();
    }
    dynamics()->setTop_(top);
}

/// @brief On init, sets the bike's lean rotation cap and increment.
/// @addr{0x80587C54}
/// In addition to setting the lean rotation cap and increment on init,
/// this function also gets called when falling out-of-bounds.
void KartMoveBike::setTurnParams() {
    static constexpr std::array<TurningParameters, 2> TURNING_PARAMS_ARRAY = {{
            {0.8f, 0.08f, 1.0f, 0.1f, 1.2f, 0.8f, 0.08f, 0.6f, 0.15f, 1.6f, 0.9f, 180},
            {1.0f, 0.1f, 1.0f, 0.05f, 1.5f, 0.7f, 0.08f, 0.6f, 0.15f, 1.3f, 0.9f, 180},
    }};

    KartMove::setTurnParams();

    m_turningParams = &TURNING_PARAMS_ARRAY[1];

    if (System::RaceManager::Instance()->isStageReached(System::RaceManager::Stage::Race)) {
        m_leanRotInc = LEAN_ROT_INC_RACE;
        m_leanRotCap = LEAN_ROT_CAP_RACE;
    } else {
        m_leanRotInc = LEAN_ROT_INC_COUNTDOWN;
        m_leanRotCap = LEAN_ROT_CAP_COUNTDOWN;
    }
}

/// @addr{0x80587D00}
void KartMoveBike::init(bool b1, bool b2) {
    KartMove::init(b1, b2);

    m_leanRot = 0.0f;
    m_leanRotCap = 0.0f;
    m_leanRotInc = 0.0f;
    m_wheelieRot = 0.0f;
    m_maxWheelieRot = 0.0f;
    m_wheelieCooldown = 0;
}

/// @stage 2
/// @brief Returns what % to raise the speed cap when wheeling.
/// @addr{0x80588324}
f32 KartMoveBike::getWheelieSoftSpeedLimitBonus() const {
    constexpr f32 WHEELIE_SPEED_BONUS = 0.15f;
    return state()->isWheelie() ? WHEELIE_SPEED_BONUS : 0.0f;
}

/// @brief STAGE 1+ - Every frame, checks player input for wheelies and computes wheelie rotation.
/// @addr{0x805883F4}
void KartMoveBike::calcWheelie() {
    constexpr u32 FAILED_WHEELIE_FRAMES = 15;

    tryStartWheelie();
    m_wheelieCooldown = std::max(0, m_wheelieCooldown - 1);

    if (state()->isWheelie()) {
        ++m_wheelieFrames;
        if (m_turningParams->maxWheelieFrames < m_wheelieFrames ||
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
        } else {
            cancelWheelie();
        }

        state()->setWheelieRot(true);
    } else {
        state()->setWheelieRot(false);
    }
}

/// @stage 2
/// @brief Virtual function that just cancels wheelies when you hop.
/// @addr{0x80588B30}
/// @todo This function may be called without actually hopping (slipdrift), in which case we should
/// rename this function.
void KartMoveBike::onHop() {
    if (state()->isAutoDrift()) {
        return;
    }

    cancelWheelie();
}

/// @stage 2
/// @brief Called when you collide with a wall. All it does for bikes is cancel wheelies.
void KartMoveBike::onWallCollision() {
    cancelWheelie();
}

/// @stage 2
/// @brief Every frame during a drift, calculates MT charge based on player input.
/// @addr{0x80588888}
void KartMoveBike::calcMtCharge() {
    constexpr u16 MAX_MT_CHARGE = 270;
    constexpr u16 BASE_MT_CHARGE = 2;
    constexpr f32 BONUS_CHARGE_STICK_THRESHOLD = 0.4f;
    constexpr u16 EXTRA_MT_CHARGE = 3;

    if (m_driftState != DriftState::ChargingMt) {
        return;
    }

    m_mtCharge += BASE_MT_CHARGE;

    f32 stickX = state()->stickX();
    if (-BONUS_CHARGE_STICK_THRESHOLD <= stickX) {
        if (BONUS_CHARGE_STICK_THRESHOLD < stickX && m_hopStickX == -1) {
            m_mtCharge += EXTRA_MT_CHARGE;
        }
    } else if (m_hopStickX != -1) {
        m_mtCharge += EXTRA_MT_CHARGE;
    }

    if (MAX_MT_CHARGE < m_mtCharge) {
        m_mtCharge = MAX_MT_CHARGE;
        m_driftState = DriftState::ChargedMt;
    }
}

/// @addr{0x80588860}
f32 KartMoveBike::wheelieRotFactor() const {
    constexpr f32 WHEELIE_ROTATION_FACTOR = 0.2f;

    return state()->isWheelie() ? WHEELIE_ROTATION_FACTOR : 1.0f;
}

/// @brief STAGE 1+ - Every frame, checks player input to see if we should start or stop a wheelie.
/// @addr{0x80588798}
void KartMoveBike::tryStartWheelie() {
    constexpr s16 COOLDOWN_FRAMES = 20;
    bool dpadUp = inputs()->currentState().trickUp();

    if (!state()->isWheelie()) {
        if (dpadUp && state()->isTouchingGround()) {
            if (state()->isDriftManual() || state()->isWallCollision() || state()->isHop() ||
                    state()->isDriftAuto()) {
                return;
            }

            if (m_wheelieCooldown > 0) {
                return;
            }

            startWheelie();
        }
    } else if (inputs()->currentState().trickDown() && m_wheelieCooldown <= 0) {
        cancelWheelie();
        m_wheelieCooldown = COOLDOWN_FRAMES;
    }
}

/// @addr{0x805896BC}
f32 KartMoveBike::leanRot() const {
    return m_leanRot;
}

/// @brief Checks if the kart is going fast enough to wheelie.
/// @addr{0x80588FE0}
bool KartMoveBike::canWheelie() const {
    constexpr f32 WHEELIE_THRESHOLD = 0.3f;

    return m_speedRatioCapped >= WHEELIE_THRESHOLD && m_speed >= 0.0f;
}

} // namespace Kart
