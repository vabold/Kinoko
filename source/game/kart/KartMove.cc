#include "KartMove.hh"

#include "game/kart/KartCollide.hh"
#include "game/kart/KartDynamics.hh"
#include "game/kart/KartJump.hh"
#include "game/kart/KartParam.hh"
#include "game/kart/KartPhysics.hh"
#include "game/kart/KartScale.hh"
#include "game/kart/KartSub.hh"
#include "game/kart/KartSuspension.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/KCollisionTypes.hh"

#include "game/item/ItemDirector.hh"
#include "game/item/KartItem.hh"

#include "game/system/CourseMap.hh"
#include "game/system/RaceManager.hh"
#include "game/system/map/MapdataCannonPoint.hh"
#include "game/system/map/MapdataJugemPoint.hh"

#include <egg/math/Math.hh>
#include <egg/math/Quat.hh>

namespace Kart {

struct CannonParameter {
    f32 speed;
    f32 height;
    f32 decelFactor;
    f32 endDecel;
};

static constexpr std::array<CannonParameter, 3> CANNON_PARAMETERS = {{
        {500.0f, 0.0f, 6000.0f, -1.0f},
        {500.0f, 5000.0f, 6000.0f, -1.0f},
        {120.0f, 2000.0f, 1000.0f, 45.0f},
}};

/// @addr{0x80577FC4}
KartMove::KartMove() : m_smoothedUp(EGG::Vector3f::ey), m_scale(1.0f, 1.0f, 1.0f) {
    m_totalScale = 1.0f;
    m_hitboxScale = 1.0f;
    m_padType.makeAllZero();
    m_flags.makeAllZero();
    m_jump = nullptr;
}

/// @addr{0x80587B78}
KartMove::~KartMove() {
    delete m_jump;
    delete m_halfPipe;
    delete m_kartScale;
}

/// @addr{0x8057821C}
void KartMove::createSubsystems() {
    m_jump = new KartJump(this);
    m_halfPipe = new KartHalfPipe();
    m_kartScale = new KartScale();
}

/// @stage All
/// @brief Each frame, looks at player input and kart stats. Saves turn-related info.
/// @addr{0x8057A8B4}
void KartMove::calcTurn() {
    m_realTurn = 0.0f;
    m_rawTurn = 0.0f;

    if (state()->isInAction() || state()->isCannonStart() || state()->isInCannon() ||
            state()->isOverZipper()) {
        return;
    }

    if (state()->isBeforeRespawn()) {
        return;
    }

    if (!state()->isHop() || m_hopStickX == 0) {
        m_rawTurn = -state()->stickX();
        if (state()->isJumpPadMushroomCollision()) {
            m_rawTurn *= 0.35f;
        } else if (state()->isAirtimeOver20()) {
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
    m_outsideDriftLastDir = m_dir;
    m_driftingParams = &DRIFTING_PARAMS_ARRAY[static_cast<u32>(param()->stats().driftType)];
    m_kartScale->reset();
}

/// @addr{0x805784D4}
void KartMove::init(bool b1, bool b2) {
    m_lastSpeed = 0.0f;
    m_baseSpeed = param()->stats().speed;
    m_jumpPadSoftSpeedLimit = m_softSpeedLimit = param()->stats().speed;
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
    m_outsideDriftAngle = 0.0f;
    m_landingAngle = 0.0f;
    m_outsideDriftLastDir = EGG::Vector3f::ez;
    m_speedRatio = 0.0f;
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
    m_hopUp = EGG::Vector3f::ey;
    m_hopDir = EGG::Vector3f::ez;
    m_divingRot = 0.0f;
    m_standStillBoostRot = 0.0f;
    m_driftState = DriftState::NotDrifting;
    m_smtCharge = 0;
    m_mtCharge = 0;
    m_outsideDriftBonus = 0.0f;
    m_boost.reset();
    m_zipperBoostTimer = 0;
    m_zipperBoostMax = 0;
    m_reject.reset();
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
        m_hitboxScale = 1.0f;
        m_mushroomBoostTimer = 0;
        m_crushTimer = 0;
    }

    m_jumpPadMinSpeed = 0.0f;
    m_jumpPadMaxSpeed = 0.0f;
    m_jumpPadBoostMultiplier = 0.0f;
    m_jumpPadProperties = nullptr;
    m_rampBoost = 0;
    m_autoDriftAngle = 0.0f;
    m_autoDriftStartFrameCounter = 0;

    m_cannonEntryOfsLength = 0.0f;
    m_cannonEntryPos.setZero();
    m_cannonEntryOfs.setZero();
    m_cannonOrthog.setZero();
    m_cannonProgress.setZero();

    m_hopVelY = 0.0f;
    m_hopPosY = 0.0f;
    m_hopGravity = 0.0f;
    m_timeInRespawn = 0;
    m_respawnPreLandTimer = 0;
    m_respawnPostLandTimer = 0;
    m_respawnTimer = 0;
    m_bumpTimer = 0;
    m_drivingDirection = DrivingDirection::Forwards;
    m_padType.makeAllZero();
    m_flags.makeAllZero();
    m_jump->reset();
    m_halfPipe->reset();
    m_rawTurn = 0.0f;
}

/// @addr{0x8058348C}
void KartMove::clear() {
    if (state()->isOverZipper()) {
        state()->setActionMidZipper(true);
    }

    clearBoost();
    clearJumpPad();
    clearRampBoost();
    clearZipperBoost();
    clearSsmt();
    clearOffroadInvincibility();
    m_halfPipe->end(false);
    m_jump->end();
    clearRejectRoad();
}

/// @brief Initializes the kart's position and rotation. Calls tire suspension initializers.
/// @addr{0x80584044}
void KartMove::setInitialPhysicsValues(const EGG::Vector3f &position, const EGG::Vector3f &angles) {
    EGG::Quatf quaternion = EGG::Quatf::FromRPY(angles * DEG2RAD);
    EGG::Vector3f newPos = position;
    Field::CollisionInfo info;
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

/// @stage All
/// @brief Each frame, calculates the kart's movement.
/// @addr{0x805788DC}
/// @details Calls various functions to handle drifts, hops, boosts.
/// Afterwards, calculates the kart's speed and rotation.
void KartMove::calc() {
    if (state()->isInRespawn()) {
        calcInRespawn();
        return;
    }

    dynamics()->resetInternalVelocity();
    m_burnout.calc();
    calcSsmtStart();
    m_halfPipe->calc();
    calcTop();
    tryEndJumpPad();
    calcRespawnBoost();
    calcSpecialFloor();
    m_jump->calc();

    m_bumpTimer = std::max(m_bumpTimer - 1, 0);

    calcAutoDrift();
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
    calcZipperBoost();
    calcCrushed();
    calcScale();

    if (state()->isInCannon()) {
        calcCannon();
    }

    calcOffroadInvincibility();
    calcVehicleSpeed();
    calcAcceleration();
    calcRotation();
}

/// @addr{0x80584334}
void KartMove::calcRespawnStart() {
    constexpr float RESPAWN_HEIGHT = 700.0f;

    const auto *jugemPoint = System::RaceManager::Instance()->jugemPoint();
    const EGG::Vector3f &jugemPos = jugemPoint->pos();
    const EGG::Vector3f &jugemRot = jugemPoint->rot();

    EGG::Vector3f respawnPos = jugemPos;
    respawnPos.y += RESPAWN_HEIGHT;
    EGG::Vector3f respawnRot = EGG::Vector3f(0.0f, jugemRot.y, 0.0f);

    setInitialPhysicsValues(respawnPos, respawnRot);

    Item::ItemDirector::Instance()->kartItem(0).clear();

    state()->setTriggerRespawn(false);
    state()->setInRespawn(true);
}

/// @addr{0x80579A50}
void KartMove::calcInRespawn() {
    constexpr f32 LAKITU_VELOCITY = 1.5f;
    constexpr u16 RESPAWN_DURATION = 110;

    if (!state()->isInRespawn()) {
        return;
    }

    EGG::Vector3f newPos = pos();
    newPos.y -= LAKITU_VELOCITY;
    dynamics()->setPos(newPos);
    dynamics()->setNoGravity(true);

    if (++m_timeInRespawn > RESPAWN_DURATION) {
        state()->setInRespawn(false);
        state()->setAfterRespawn(true);
        state()->setRespawnKillY(true);
        m_timeInRespawn = 0;
        m_flags.setBit(eFlags::Respawned);
        dynamics()->setNoGravity(false);
    }
}

/// @addr{0x80581C90}
void KartMove::calcRespawnBoost() {
    constexpr s16 RESPAWN_BOOST_DURATION = 30;
    constexpr s16 RESPAWN_BOOST_INPUT_LENIENCY = 4;

    if (state()->isAfterRespawn()) {
        if (state()->isTouchingGround()) {
            if (m_respawnPreLandTimer > 0) {
                if (!state()->isBeforeRespawn() && !state()->isInAction()) {
                    activateBoost(KartBoost::Type::AllMt, RESPAWN_BOOST_DURATION);
                    m_respawnTimer = RESPAWN_BOOST_DURATION;
                }
            } else {
                m_respawnPostLandTimer = RESPAWN_BOOST_INPUT_LENIENCY;
            }

            state()->setAfterRespawn(false);
            m_flags.resetBit(eFlags::Respawned);
        }

        m_respawnPreLandTimer = std::max(0, m_respawnPreLandTimer - 1);

        if (m_flags.onBit(eFlags::Respawned) && state()->isAccelerateStart()) {
            m_respawnPreLandTimer = RESPAWN_BOOST_INPUT_LENIENCY;
            m_flags.resetBit(eFlags::Respawned);
        }
    } else {
        if (m_respawnPostLandTimer > 0) {
            if (state()->isAccelerateStart()) {
                if (!state()->isBeforeRespawn() && !state()->isInAction()) {
                    activateBoost(KartBoost::Type::AllMt, RESPAWN_BOOST_DURATION);
                    m_respawnTimer = RESPAWN_BOOST_DURATION;
                }

                m_respawnPostLandTimer = 0;
            }

            m_respawnPostLandTimer = std::max(0, m_respawnPostLandTimer - 1);
        } else {
            state()->setRespawnKillY(false);
        }
    }

    m_respawnTimer = std::max(0, m_respawnTimer - 1);
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
            if ((m_flags.onBit(eFlags::TrickableSurface) || state()->trickableTimer() > 0) &&
                    inputTop.dot(m_dir) > 0.0f && m_speed > 50.0f &&
                    collide()->surfaceFlags().onBit(KartCollide::eSurfaceFlags::NotTrickable)) {
                inputTop = m_up;
            } else {
                m_up = inputTop;
            }

            f32 scalar = 0.8f;

            if (state()->isHalfPipeRamp() ||
                    (!state()->isBoost() && !state()->isRampBoost() && !state()->isWheelie() &&
                            !state()->isOverZipper() &&
                            (!state()->isZipperBoost() || m_zipperBoostTimer > 15))) {
                f32 topDotZ = 0.8f - 6.0f * (EGG::Mathf::abs(inputTop.dot(componentZAxis())));
                scalar = std::min(0.8f, std::max(0.3f, topDotZ));
            }

            m_smoothedUp += (inputTop - m_smoothedUp) * scalar;
            m_smoothedUp.normalise();

            f32 bodyDotFront = bodyFront().dot(m_smoothedUp);

            if (bodyDotFront < -0.1f) {
                stabilizationFactor += std::min(0.2f, EGG::Mathf::abs(bodyDotFront) * 0.5f);
            }

            if (collide()->surfaceFlags().onBit(KartCollide::eSurfaceFlags::BoostRamp)) {
                stabilizationFactor = 0.4f;
            }
        } else {
            calcAirtimeTop();
        }
    }

    dynamics()->setStabilizationFactor(stabilizationFactor);

    m_nonZipperAirtime = state()->isOverZipper() ? 0 : state()->airtime();
    m_flags.changeBit(collide()->surfaceFlags().onBit(KartCollide::eSurfaceFlags::Trickable),
            eFlags::TrickableSurface);
}

/// @addr{0x8057D888}
/// @brief Calculates rotation of the bike due to excessive airtime.
void KartMove::calcAirtimeTop() {
    if (state()->isOverZipper() || !state()->isAirtimeOver20()) {
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

    if (m_padType.onBit(ePadType::BoostPanel)) {
        tryStartBoostPanel();
    }

    if (m_padType.onBit(ePadType::BoostRamp)) {
        tryStartBoostRamp();
    }

    if (m_padType.onBit(ePadType::JumpPad)) {
        tryStartJumpPad();
    }

    m_padType.makeAllZero();
}

/// @addr{0x8057A140}
void KartMove::calcDirs() {
    EGG::Vector3f right = dynamics()->mainRot().rotateVector(EGG::Vector3f::ex);
    EGG::Vector3f local_88 = right.cross(m_smoothedUp);
    local_88.normalise();
    m_flags.setBit(eFlags::LaunchBoost);

    if (!state()->isInATrick() && !state()->isOverZipper() &&
            (((state()->isTouchingGround() || !state()->isRampBoost() ||
                      !m_jump->isBoostRampEnabled()) &&
                     !state()->isJumpPad() && state()->airtime() <= 5) ||
                    state()->isJumpPadMushroomCollision() || state()->isNoSparkInvisibleWall())) {
        if (state()->isHop()) {
            local_88 = m_hopDir;
        }

        EGG::Matrix34f mat;
        mat.setAxisRotation(DEG2RAD * (m_autoDriftAngle + m_outsideDriftAngle + m_landingAngle),
                m_smoothedUp);
        EGG::Vector3f local_b8 = mat.multVector(local_88);
        local_b8 = local_b8.perpInPlane(m_smoothedUp, true);

        EGG::Vector3f dirDiff = local_b8 - m_dir;

        if (dirDiff.squaredLength() <= std::numeric_limits<f32>::epsilon()) {
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
        m_flags.resetBit(eFlags::LaunchBoost);
    } else {
        m_vel1Dir = m_dir;
    }

    if (!state()->isOverZipper()) {
        m_jump->tryStart(m_smoothedUp.cross(m_dir));
    }

    if (m_hasLandingDir) {
        f32 dot = m_dir.dot(m_landingDir);
        EGG::Vector3f cross = m_dir.cross(m_landingDir);
        f32 crossDot = cross.length();
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
    constexpr Field::KCLTypeMask STICKY_MASK =
            KCL_TYPE_BIT(COL_TYPE_STICKY_ROAD) | KCL_TYPE_BIT(COL_TYPE_MOVING_WATER);

    if (state()->isOverZipper()) {
        state()->setStickyRoad(false);
        return;
    }

    if ((!state()->isStickyRoad() &&
                collide()->surfaceFlags().offBit(KartCollide::eSurfaceFlags::Trickable)) ||
            EGG::Mathf::abs(m_speed) <= 20.0f) {
        return;
    }

    EGG::Vector3f pos = dynamics()->pos();
    EGG::Vector3f vel = dynamics()->movingObjVel() + m_speed * m_vel1Dir;
    EGG::Vector3f down = -STICKY_RADIUS * componentYAxis();
    Field::CollisionInfo colInfo;
    colInfo.bbox.setZero();
    Field::KCLTypeMask kcl_flags = KCL_NONE;
    bool stickyRoad = false;

    for (size_t i = 0; i < 3; ++i) {
        EGG::Vector3f newPos = pos + vel;
        if (Field::CollisionDirector::Instance()->checkSphereFull(STICKY_RADIUS, newPos,
                    EGG::Vector3f::inf, STICKY_MASK, &colInfo, &kcl_flags, 0)) {
            m_vel1Dir = m_vel1Dir.perpInPlane(colInfo.floorNrm, true);
            dynamics()->setMovingObjVel(dynamics()->movingObjVel().rej(colInfo.floorNrm));
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

    if (--m_ssmtDisableAccelTimer < 0 ||
            (m_flags.offBit(eFlags::SsmtLeeway) && !state()->isBrake())) {
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
            m_flags.setBit(eFlags::SsmtCharged);
            m_ssmtLeewayTimer = 0;
        }

        return;
    }

    m_ssmtCharge = 0;

    if (m_flags.offBit(eFlags::SsmtCharged)) {
        return;
    }

    if (m_flags.onBit(eFlags::SsmtLeeway)) {
        if (--m_ssmtLeewayTimer < 0) {
            m_ssmtLeewayTimer = 0;
            m_flags.resetBit(eFlags::SsmtCharged, eFlags::SsmtLeeway);
            m_ssmtDisableAccelTimer = DISABLE_ACCEL_FRAMES;
            state()->setDisableBackwardsAccel(true);
        } else {
            if (!state()->isAccelerate() && !state()->isBrake()) {
                activateBoost(KartBoost::Type::AllMt, SSMT_BOOST_FRAMES);
                m_ssmtLeewayTimer = 0;
                m_flags.resetBit(eFlags::SsmtCharged, eFlags::SsmtLeeway);
            }
        }
    } else {
        if (state()->isAccelerate() && !state()->isBrake()) {
            activateBoost(KartBoost::Type::AllMt, SSMT_BOOST_FRAMES);
            m_ssmtLeewayTimer = 0;
            m_flags.resetBit(eFlags::SsmtCharged, eFlags::SsmtLeeway);
        } else {
            m_ssmtLeewayTimer = LEEWAY_FRAMES;
            m_flags.setBit(eFlags::SsmtLeeway);
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
    m_smtCharge = 0;
    m_mtCharge = 0;
}

/// @stage 2
/// @addr{0x8057E348}
void KartMove::clearDrift() {
    m_flags.resetBit(eFlags::DriftReset);
    m_outsideDriftAngle = 0.0f;
    m_hopStickX = 0;
    m_hopFrame = 0;
    m_driftState = DriftState::NotDrifting;
    m_smtCharge = 0;
    m_mtCharge = 0;
    m_outsideDriftBonus = 0.0f;
    state()->setHop(false);
    state()->setSlipdriftCharge(false);
    state()->setDriftManual(false);
    state()->setDriftAuto(false);
    m_autoDriftAngle = 0.0f;
    m_hopStickX = 0;
    m_autoDriftStartFrameCounter = 0;
}

/// @addr{0x80582DB4}
void KartMove::clearJumpPad() {
    m_jumpPadMinSpeed = 0.0f;
    state()->setJumpPad(false);
}

/// @addr{0x80582DD8}
void KartMove::clearRampBoost() {
    m_rampBoost = 0;
    state()->setRampBoost(false);
}

/// @addr{0x80582F38}
void KartMove::clearZipperBoost() {
    m_zipperBoostTimer = 0;
    state()->setZipperBoost(false);
}

/// @addr{0x80582D94}
void KartMove::clearBoost() {
    m_boost.resetActive();
    state()->setBoost(false);
}

/// @addr{0x80582F58}
void KartMove::clearSsmt() {
    m_ssmtCharge = 0;
    m_ssmtLeewayTimer = 0;
    m_ssmtDisableAccelTimer = 0;
    m_flags.resetBit(eFlags::SsmtCharged, eFlags::SsmtLeeway);
}

/// @addr{0x80582F7C}
void KartMove::clearOffroadInvincibility() {
    m_offroadInvincibility = 0;
    state()->setBoostOffroadInvincibility(false);
}

void KartMove::clearRejectRoad() {
    state()->setRejectRoadTrigger(false);
    state()->setNoSparkInvisibleWall(false);
}

/// @stage 2
/// @brief Each frame, handles automatic transmission drifting.
/// @addr{0x8057E0DC}
void KartMove::calcAutoDrift() {
    constexpr s16 AUTO_DRIFT_START_DELAY = 12;

    if (!state()->isAutoDrift()) {
        return;
    }

    if (canStartDrift() && !state()->isOverZipper() && !state()->isRejectRoadTrigger() &&
            !state()->isWheelie() && EGG::Mathf::abs(state()->stickX()) > 0.85f) {
        m_autoDriftStartFrameCounter =
                std::min<s16>(AUTO_DRIFT_START_DELAY, m_autoDriftStartFrameCounter + 1);
    } else {
        m_autoDriftStartFrameCounter = 0;
    }

    if (m_autoDriftStartFrameCounter >= AUTO_DRIFT_START_DELAY) {
        state()->setDriftAuto(true);

        if (state()->isTouchingGround()) {
            if (state()->stickX() < 0.0f) {
                m_hopStickX = 1;
                m_autoDriftAngle -= 30.0f * param()->stats().driftAutomaticTightness;

            } else {
                m_hopStickX = -1;
                m_autoDriftAngle += 30.0f * param()->stats().driftAutomaticTightness;
            }
        }

        f32 halfTarget = 0.5f * param()->stats().driftOutsideTargetAngle;
        m_autoDriftAngle = std::min(halfTarget, std::max(-halfTarget, m_autoDriftAngle));
    } else {
        state()->setDriftAuto(false);
        m_hopStickX = 0;

        if (m_autoDriftAngle > 0.0f) {
            m_autoDriftAngle =
                    std::max(0.0f, m_autoDriftAngle - param()->stats().driftOutsideDecrement);
        } else {
            m_autoDriftAngle =
                    std::min(0.0f, m_autoDriftAngle + param()->stats().driftOutsideDecrement);
        }
    }

    EGG::Quatf angleAxis;
    angleAxis.setAxisRotation(-m_autoDriftAngle * DEG2RAD, m_up);
    physics()->composeExtraRot(angleAxis);
}

/// @stage 2
/// @brief Each frame, handles hopping, drifting, and mini-turbos.
/// @addr{0x8057DC44}
void KartMove::calcManualDrift() {
    bool isHopping = calcPreDrift();

    if (!state()->isOverZipper()) {
        const EGG::Vector3f rotZ = dynamics()->mainRot().rotateVector(EGG::Vector3f::ez);

        if (!state()->isTouchingGround() &&
                param()->stats().driftType != KartParam::Stats::DriftType::Inside_Drift_Bike &&
                !state()->isJumpPadMushroomCollision() &&
                (state()->isDriftManual() || state()->isSlipdriftCharge()) &&
                m_flags.onBit(eFlags::LaunchBoost)) {
            const EGG::Vector3f up = dynamics()->mainRot().rotateVector(EGG::Vector3f::ey);
            EGG::Vector3f driftRej = m_outsideDriftLastDir.rej(up);

            if (driftRej.normalise() != 0.0f) {
                f32 rejCrossDirMag = driftRej.cross(rotZ).length();
                f32 angle = EGG::Mathf::atan2(rejCrossDirMag, driftRej.dot(rotZ));
                f32 sign = 1.0f;
                if ((rotZ.z * (rotZ.x - driftRej.x)) - (rotZ.x * (rotZ.z - driftRej.z)) > 0.0f) {
                    sign = -1.0f;
                }

                m_outsideDriftAngle += angle * RAD2DEG * sign;
            }
        }

        m_outsideDriftLastDir = rotZ;
    }

    // TODO: Is this backwards/inverted?
    if (((!state()->isHop() || m_hopFrame < 3) && !state()->isSlipdriftCharge()) ||
            (state()->isInAction() || !state()->isTouchingGround())) {
        if (canHop()) {
            hop();
            isHopping = true;
        }
    } else {
        startManualDrift();
        isHopping = false;
    }

    m_flags.resetBit(eFlags::DriftReset);

    if (!state()->isDriftManual()) {
        if (!isHopping && state()->isTouchingGround()) {
            resetDriftManual();

            if (action()->flags().offBit(KartAction::eFlags::Rotating) || m_speed <= 20.0f) {
                f32 driftAngleDecr = param()->stats().driftOutsideDecrement;
                if (m_outsideDriftAngle > 0.0f) {
                    m_outsideDriftAngle = std::max(0.0f, m_outsideDriftAngle - driftAngleDecr);
                } else if (m_outsideDriftAngle < 0.0f) {
                    m_outsideDriftAngle = std::min(0.0f, m_outsideDriftAngle + driftAngleDecr);
                }
            }
        }
    } else {
        // This is a different comparison than @ref KartMove::canStartDrift().
        bool canStartDrift = m_speed > MINIMUM_DRIFT_THRESOLD * m_baseSpeed;

        if (!state()->isOverZipper() &&
                (!state()->isDriftInput() || !state()->isAccelerate() || state()->isInAction() ||
                        state()->isRejectRoadTrigger() || state()->isWall3Collision() ||
                        state()->isWallCollision() || !canStartDrift)) {
            if (canStartDrift) {
                releaseMt();
            }

            resetDriftManual();
            m_flags.setBit(eFlags::DriftReset);
        } else {
            controlOutsideDriftAngle();
        }
    }
}

/// @stage 2
/// @brief Called when the player lands from a drift hop, or to start a slipdrift.
/// @addr{0x8057E3F4}
void KartMove::startManualDrift() {
    constexpr f32 OUTSIDE_DRIFT_BONUS = 0.5f;

    const auto &stats = param()->stats();

    if (stats.driftType != KartParam::Stats::DriftType::Inside_Drift_Bike) {
        f32 driftAngle = 0.0f;

        if (state()->isHop()) {
            const EGG::Vector3f rotZ = dynamics()->mainRot().rotateVector(EGG::Vector3f::ez);
            EGG::Vector3f rotRej = rotZ.rej(m_hopUp);

            if (rotRej.normalise() != 0.0f) {
                const EGG::Vector3f hopCrossRot = m_hopDir.cross(rotRej);
                driftAngle =
                        EGG::Mathf::atan2(hopCrossRot.length(), m_hopDir.dot(rotRej)) * RAD2DEG;
            }
        }

        m_outsideDriftAngle += driftAngle * static_cast<f32>(-m_hopStickX);
        m_outsideDriftAngle = std::max(-60.0f, std::min(60.0f, m_outsideDriftAngle));
    }

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
    m_outsideDriftBonus = OUTSIDE_DRIFT_BONUS * (m_speedRatioCapped * stats.driftManualTightness);
}

/// @stage 2
/// @brief Stops charging a mini-turbo, and applies boost if charged.
/// @addr{0x80582F9C}
void KartMove::releaseMt() {
    constexpr f32 SMT_LENGTH_FACTOR = 3.0f;

    if (m_driftState < DriftState::ChargedMt || state()->isBrake()) {
        m_driftState = DriftState::NotDrifting;
        return;
    }

    u16 mtLength = param()->stats().miniTurbo;

    if (m_driftState == DriftState::ChargedSmt) {
        mtLength *= SMT_LENGTH_FACTOR;
    }

    if (!state()->isBeforeRespawn() && !state()->isInAction()) {
        activateBoost(KartBoost::Type::AllMt, mtLength);
    }

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
        if (m_hopStickX == -1) {
            f32 angle = m_outsideDriftAngle;
            f32 targetAngle = param()->stats().driftOutsideTargetAngle;
            if (angle > targetAngle) {
                m_outsideDriftAngle = std::max(m_outsideDriftAngle - 2.0f, targetAngle);
            } else if (angle < targetAngle) {
                m_outsideDriftAngle += 150.0f * param()->stats().driftManualTightness;
                m_outsideDriftAngle = std::min(m_outsideDriftAngle, targetAngle);
            }
        } else if (m_hopStickX == 1) {
            f32 angle = m_outsideDriftAngle;
            f32 targetAngle = -param()->stats().driftOutsideTargetAngle;
            if (targetAngle > angle) {
                m_outsideDriftAngle = std::min(m_outsideDriftAngle + 2.0f, targetAngle);
            } else if (targetAngle < angle) {
                m_outsideDriftAngle -= 150.0f * param()->stats().driftManualTightness;
                m_outsideDriftAngle = std::max(m_outsideDriftAngle, targetAngle);
            }
        }
    }

    calcMtCharge();
}

/// @stage 1+
/// @brief Every frame, calculates kart rotation based on player input.
/// @addr{0x8057C69C}
void KartMove::calcRotation() {
    f32 turn;
    bool drifting = state()->isDrifting() && !state()->isJumpPadMushroomCollision();
    bool autoDrift = state()->isAutoDrift();
    const auto &stats = param()->stats();

    if (drifting) {
        turn = autoDrift ? stats.driftAutomaticTightness : stats.driftManualTightness;
    } else {
        turn = autoDrift ? stats.handlingAutomaticTightness : stats.handlingManualTightness;
    }

    if (drifting && stats.driftType != KartParam::Stats::DriftType::Inside_Drift_Bike) {
        m_outsideDriftBonus *= 0.99f;
        turn += m_outsideDriftBonus;
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
            if (!state()->isWallCollision() && !state()->isWall3Collision() &&
                    EGG::Mathf::abs(m_speed) < 1.0f) {
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

        if (state()->isZipperBoost() && !state()->isDriftManual()) {
            turn *= 2.0f;
        }

        f32 stickX = EGG::Mathf::abs(state()->stickX());
        if (autoDrift && stickX > 0.3f) {
            f32 stickScalar = (stickX - 0.3f) / 0.7f;
            stickX = drifting ? 0.2f : 0.5f;
            turn += stickScalar * (turn * stickX * m_speedRatioCapped);
        }
    }

    if (!state()->isInAction() && !state()->isZipperTrick()) {
        if (!state()->isTouchingGround()) {
            if (state()->isRampBoost() && m_jump->isBoostRampEnabled()) {
                turn = 0.0f;
            } else if (!state()->isJumpPadMushroomCollision()) {
                u32 airtime = state()->airtime();
                if (airtime >= 70) {
                    turn = 0.0f;
                } else if (airtime >= 30) {
                    turn = std::max(0.0f, turn * (1.0f - (airtime - 30) * 0.025f));
                }
            }
        }

        const EGG::Vector3f forward = dynamics()->mainRot().rotateVector(EGG::Vector3f::ez);
        f32 angle = EGG::Mathf::atan2(forward.cross(m_dir).length(), forward.dot(m_dir));
        angle = EGG::Mathf::abs(angle) * RAD2DEG;

        if (angle > 60.0f) {
            turn *= std::max(0.0f, 1.0f - (angle - 60.0f) / 40.0f);
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
        if (state()->isInAction() ||
                ((state()->isWallCollisionStart() || state()->wallBonkTimer() == 0 ||
                         EGG::Mathf::abs(speedFix) >= 3.0f) &&
                        !state()->isDriftManual())) {
            m_speed += speedFix;
        }
    }

    if (m_speed < -20.0f) {
        m_speed += 0.5f;
    }

    m_acceleration = 0.0f;
    m_speedDragMultiplier = 1.0f;

    if (state()->isInAction()) {
        action()->calcVehicleSpeed();
        return;
    }

    if ((state()->isSomethingWallCollision() && state()->isTouchingGround() &&
                !state()->isAnyWheelCollision()) ||
            !state()->isTouchingGround() || state()->isChargingSsmt()) {
        if (state()->isRampBoost() && state()->airtime() < 4) {
            m_acceleration = 7.0f;
        } else {
            if (state()->isJumpPad() && !state()->isAccelerate()) {
                m_speedDragMultiplier = 0.99f;
            } else {
                if (state()->isOverZipper()) {
                    m_speedDragMultiplier = 0.999f;
                } else {
                    if (state()->airtime() > 5) {
                        m_speedDragMultiplier = 0.999f;
                    }
                }
            }
            m_speed *= m_speedDragMultiplier;
        }

    } else if (state()->isBoost()) {
        m_acceleration = m_boost.acceleration();
    } else {
        if (!state()->isJumpPad() && !state()->isRampBoost()) {
            if (state()->isAccelerate()) {
                m_acceleration = state()->isHalfPipeRamp() ? 5.0f : calcVehicleAcceleration();
            } else {
                if (!state()->isBrake() || state()->isDisableBackwardsAccel() ||
                        state()->isSomethingWallCollision()) {
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
/// @addr{0x8057B028}
void KartMove::calcDeceleration() {
    f32 vel = 0.0f;
    f32 initialVel = 1.0f - m_smoothedUp.y;
    if (EGG::Mathf::abs(m_speed) < 30.0f && m_smoothedUp.y > 0.0f && initialVel > 0.0f) {
        initialVel = std::min(initialVel * 2.0f, 2.0f);
        vel += initialVel;
        vel *= std::min(0.5f, std::max(-0.5f, -bodyFront().y));
    }
    m_speed += vel;
}

/// @stage 2
/// @brief Every frame, computes acceleration based off the character/vehicle stats.
/// @addr{0x8057B868}
f32 KartMove::calcVehicleAcceleration() const {
    f32 ratio = m_speed / m_softSpeedLimit;
    if (ratio < 0.0f) {
        return 1.0f;
    }

    std::span<const f32> as;
    std::span<const f32> ts;
    if (state()->isDrifting()) {
        as = param()->stats().accelerationDriftA;
        ts = param()->stats().accelerationDriftT;
    } else {
        as = param()->stats().accelerationStandardA;
        ts = param()->stats().accelerationStandardT;
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
    constexpr f32 OOB_SLOWDOWN_RATE = 0.95f;
    constexpr f32 TERMINAL_VELOCITY = 90.0f;

    m_lastSpeed = m_speed;

    dynamics()->setKillExtVelY(state()->isRespawnKillY());

    if (state()->isBurnout()) {
        m_speed = 0.0f;
    } else {
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
    }

    if (state()->isBeforeRespawn()) {
        m_speed *= OOB_SLOWDOWN_RATE;
    } else {
        if (state()->isChargingSsmt()) {
            m_speed *= 0.8f;
        } else {
            if (m_drivingDirection == DrivingDirection::Braking && m_speed < 0.0f) {
                m_speed = 0.0f;
                m_drivingDirection = DrivingDirection::WaitingForBackwards;
                m_backwardsAllowCounter = 0;
            }
        }
    }

    f32 speedLimit = state()->isJumpPad() ? m_jumpPadMaxSpeed : m_baseSpeed;
    const f32 boostMultiplier = m_boost.multiplier();
    const f32 boostSpdLimit = m_boost.speedLimit();
    m_jumpPadBoostMultiplier = boostMultiplier;

    f32 crushMultiplier = state()->isCrushed() ? 0.7f : 1.0f;
    f32 wheelieBonus = boostMultiplier + getWheelieSoftSpeedLimitBonus();
    speedLimit *= state()->isJumpPadFixedSpeed() ?
            1.0f :
            crushMultiplier * (wheelieBonus * m_kclSpeedFactor);

    bool ignoreCrushSpeed = state()->isRampBoost() || state()->isZipperInvisibleWall() ||
            state()->isOverZipper() || state()->isHalfPipeRamp();
    f32 boostSpeed = ignoreCrushSpeed ? 1.0f : crushMultiplier;
    boostSpeed *= boostSpdLimit * m_kclSpeedFactor;

    if (!state()->isJumpPad() && boostSpeed > 0.0f && boostSpeed > speedLimit) {
        speedLimit = boostSpeed;
    }

    m_jumpPadSoftSpeedLimit = boostSpdLimit * m_kclSpeedFactor;

    if (state()->isRampBoost()) {
        speedLimit = std::max(speedLimit, 100.0f);
    }

    m_lastDir = (m_speed > 0.0f) ? 1.0f * m_dir : -1.0f * m_dir;

    f32 local_c8 = 1.0f;
    speedLimit *= calcWallCollisionSpeedFactor(local_c8);

    if (m_softSpeedLimit <= speedLimit) {
        m_softSpeedLimit = speedLimit;
    } else if (!state()->isWallCollision() && !state()->isWall3Collision()) {
        m_softSpeedLimit = std::max(m_softSpeedLimit - 3.0f, speedLimit);
    } else {
        m_softSpeedLimit = speedLimit;
    }

    m_softSpeedLimit = std::min(m_hardSpeedLimit, m_softSpeedLimit);

    m_speed = std::min(m_softSpeedLimit, std::max(-m_softSpeedLimit, m_speed));

    if (state()->isJumpPad()) {
        m_speed = std::max(m_speed, m_jumpPadMinSpeed);
    }

    calcWallCollisionStart(local_c8);

    m_speedRatio = EGG::Mathf::abs(m_speed / m_baseSpeed);
    m_speedRatioCapped = std::min(1.0f, m_speedRatio);

    EGG::Vector3f crossVec = m_smoothedUp.cross(m_dir);
    if (m_speed < 0.0f) {
        crossVec = -crossVec;
    }

    f32 rotationScalar = ROTATION_SCALAR_NORMAL;
    if (collide()->surfaceFlags().onBit(KartCollide::eSurfaceFlags::BoostRamp)) {
        rotationScalar = ROTATION_SCALAR_BOOST_RAMP;
    } else if (!state()->isTouchingGround()) {
        rotationScalar = ROTATION_SCALAR_MIDAIR;
    }

    EGG::Matrix34f local_90;
    local_90.setAxisRotation(rotationScalar * DEG2RAD, crossVec);
    m_vel1Dir = local_90.multVector33(m_vel1Dir);

    const auto *raceMgr = System::RaceManager::Instance();
    if (!state()->isInAction() && !state()->isDisableBackwardsAccel() &&
            state()->isTouchingGround() && !state()->isAccelerate() &&
            raceMgr->isStageReached(System::RaceManager::Stage::Race)) {
        calcDeceleration();
    }

    m_processedSpeed = m_speed;
    EGG::Vector3f nextSpeed = m_speed * m_vel1Dir;

    f32 maxSpeedY = state()->isOverZipper() ? KartHalfPipe::TerminalVelocity() : TERMINAL_VELOCITY;
    nextSpeed.y = std::min(nextSpeed.y, maxSpeedY);

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
    if (!state()->isWallCollision() && !state()->isWall3Collision()) {
        return 1.0f;
    }

    onWallCollision();

    if (state()->isZipperInvisibleWall() || state()->isOverZipper()) {
        return 1.0f;
    }

    EGG::Vector3f wallNrm = collisionData().wallNrm;
    if (wallNrm.y > 0.0f) {
        wallNrm.y = 0.0f;
        wallNrm.normalise();
    }

    f32 dot = m_lastDir.dot(wallNrm);

    if (dot < 0.0f) {
        f1 = std::max(0.0f, dot + 1.0f);

        return std::min(1.0f, f1 * (state()->isWallCollision() ? 0.4f : 0.7f));
    }

    return 1.0f;
}

/// @addr{0x8057B2A0}
/// @stage 2
/// @brief If we started to collide with a wall this frame, applies rotation.
void KartMove::calcWallCollisionStart(f32 param_2) {
    m_flags.resetBit(eFlags::WallBounce);

    if (!state()->isWallCollisionStart()) {
        return;
    }

    m_outsideDriftAngle = 0.0f;
    if (!state()->isInAction()) {
        m_dir = bodyFront();
        m_vel1Dir = m_dir;
        m_landingDir = m_dir;
    }

    if (!state()->isZipperInvisibleWall() && !state()->isOverZipper() && param_2 < 0.9f) {
        f32 speedDiff = m_lastSpeed - m_speed;
        const CollisionData &colData = collisionData();

        if (speedDiff > 30.0f) {
            m_flags.setBit(eFlags::WallBounce);
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
            f32 bumpDeviation = 0.0f;
            if (m_flags.offBit(eFlags::DriftReset) && state()->isTouchingGround()) {
                bumpDeviation = param()->stats().bumpDeviationLevel;
            }

            dynamics()->applyWrenchScaled(newPos, projRejSum, bumpDeviation);
        } else if (wallKclType() == COL_TYPE_SPECIAL_WALL && wallKclVariant() == 2) {
            dynamics()->addForce(colData.wallNrm * 15.0f);
            collide()->startFloorMomentRate();
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

    if (m_flags.onBit(eFlags::WallBounce)) {
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

    if (state()->isTouchingGround() || state()->isCannonStart() || state()->isInCannon() ||
            state()->isInAction() || state()->isOverZipper()) {
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
    f32 crossNorm = upCrossTop.length();
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

/// @addr{0x80579960}
void KartMove::calcRejectRoad() {
    m_reject.calcRejectRoad();
}

/// @addr{0x80583F2C}
bool KartMove::calcZipperCollision(f32 radius, f32 scale, EGG::Vector3f &pos,
        EGG::Vector3f &upLocal, const EGG::Vector3f &prevPos, Field::CollisionInfo *colInfo,
        Field::KCLTypeMask *maskOut, Field::KCLTypeMask flags) const {
    upLocal = mainRot().rotateVector(EGG::Vector3f::ey);
    pos = dynamics()->pos() + (-scale * m_scale.y) * upLocal;

    auto *colDir = Field::CollisionDirector::Instance();
    return colDir->checkSphereFullPush(radius, pos, prevPos, flags, colInfo, maskOut, 0);
}

/// @addr{0x805879A4}
f32 KartMove::calcSlerpRate(f32 scale, const EGG::Quatf &from, const EGG::Quatf &to) const {
    f32 dotNorm = std::max(-1.0f, std::min(1.0f, from.dot(to)));
    f32 acos = EGG::Mathf::acos(dotNorm);
    return acos > 0.0f ? std::min(0.1f, scale / acos) : 0.1f;
}

/// @addr{0x80586DB4}
void KartMove::applyForce(f32 force, const EGG::Vector3f &hitDir, bool stop) {
    if (m_bumpTimer >= 1) {
        return;
    }

    dynamics()->addForce(force * hitDir.perpInPlane(m_up, true));

    collide()->startFloorMomentRate();

    m_bumpTimer = 5;

    if (stop) {
        m_speed = 0.0f;
    }
}

/// @addr{0x8057CF0C}
/// @brief Every frame, calculates rotation, EV, and angular velocity for the kart.
void KartMove::calcVehicleRotation(f32 turn) {
    f32 tiltMagnitude = 0.0f;

    if (!state()->isInAction() && !state()->isSoftWallDrift() && state()->isAnyWheelCollision()) {
        EGG::Vector3f front = componentZAxis();
        front = front.perpInPlane(m_up, true);
        EGG::Vector3f frontSpeed = velocity().rej(front).perpInPlane(m_up, false);
        f32 magnitude = tiltMagnitude;

        if (frontSpeed.squaredLength() > std::numeric_limits<f32>::epsilon()) {
            magnitude = frontSpeed.length();

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

    calcDive();
}

/// @stage 2
/// @brief Every frame during a drift, calculates MT/SMT charge based on player input.
/// @addr{0x8057EE50}
void KartMove::calcMtCharge() {
    // TODO: Some of these are shared between the base and derived class implementations.
    constexpr u16 MAX_MT_CHARGE = 270;
    constexpr u16 MAX_SMT_CHARGE = 300;
    constexpr u16 BASE_MT_CHARGE = 2;
    constexpr u16 BASE_SMT_CHARGE = 2;
    constexpr f32 BONUS_CHARGE_STICK_THRESHOLD = 0.4f;
    constexpr u16 EXTRA_MT_CHARGE = 3;

    if (m_driftState == DriftState::ChargedSmt) {
        return;
    }

    f32 stickX = state()->stickX();

    if (m_driftState == DriftState::ChargingMt) {
        m_mtCharge += BASE_MT_CHARGE;

        if (-BONUS_CHARGE_STICK_THRESHOLD <= stickX) {
            if (BONUS_CHARGE_STICK_THRESHOLD < stickX && m_hopStickX == -1) {
                m_mtCharge += EXTRA_MT_CHARGE;
            }
        } else if (m_hopStickX != -1) {
            m_mtCharge += EXTRA_MT_CHARGE;
        }

        if (m_mtCharge > MAX_MT_CHARGE) {
            m_mtCharge = MAX_MT_CHARGE;
            m_driftState = DriftState::ChargingSmt;
        }
    }

    if (m_driftState != DriftState::ChargingSmt) {
        return;
    }

    m_smtCharge += BASE_SMT_CHARGE;

    if (-BONUS_CHARGE_STICK_THRESHOLD <= stickX) {
        if (BONUS_CHARGE_STICK_THRESHOLD < stickX && m_hopStickX == -1) {
            m_smtCharge += EXTRA_MT_CHARGE;
        }
    } else if (m_hopStickX != -1) {
        m_smtCharge += EXTRA_MT_CHARGE;
    }

    if (m_smtCharge > MAX_SMT_CHARGE) {
        m_smtCharge = MAX_SMT_CHARGE;
        m_driftState = DriftState::ChargedSmt;
    }
}

/// @addr{0x80583658}
void KartMove::initOob() {
    clearBoost();
    clearJumpPad();
    clearRampBoost();
    clearZipperBoost();
    clearSsmt();
    clearOffroadInvincibility();
}

/// @stage 2
/// @brief Initializes hop information, resets upwards EV and clears upwards force.
/// @addr{0x8057DA5C}
void KartMove::hop() {
    state()->setHop(true);
    state()->setDriftManual(false);
    onHop();

    m_hopUp = dynamics()->mainRot().rotateVector(EGG::Vector3f::ey);
    m_hopDir = dynamics()->mainRot().rotateVector(EGG::Vector3f::ez);
    m_driftState = DriftState::NotDrifting;
    m_smtCharge = 0;
    m_mtCharge = 0;
    m_hopStickX = 0;
    m_hopFrame = 0;
    m_hopPosY = 0.0f;
    m_hopGravity = dynamics()->gravity();
    m_hopVelY = m_driftingParams->hopVelY;
    m_outsideDriftBonus = 0.0f;

    EGG::Vector3f extVel = dynamics()->extVel();
    extVel.y = 0.0f + m_hopVelY;
    dynamics()->setExtVel(extVel);

    EGG::Vector3f totalForce = dynamics()->totalForce();
    totalForce.y = 0.0f;
    dynamics()->setTotalForce(totalForce);
}

/// @addr{Inlined at 0x80587590}
void KartMove::tryStartBoostPanel() {
    constexpr s16 BOOST_PANEL_DURATION = 60;

    if (state()->isBeforeRespawn() || state()->isInAction()) {
        return;
    }

    activateBoost(KartBoost::Type::MushroomAndBoostPanel, BOOST_PANEL_DURATION);
    setOffroadInvincibility(BOOST_PANEL_DURATION);
}

/// @stage 2
/// @brief Sets offroad invincibility and and enables the ramp boost bitfield flag.
/// @addr{Inlined at 0x80587590}
void KartMove::tryStartBoostRamp() {
    constexpr s16 BOOST_RAMP_DURATION = 60;

    if (state()->isBeforeRespawn() || state()->isInAction()) {
        return;
    }

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

    if (state()->isBeforeRespawn() || state()->isInAction() || state()->isHalfPipeRamp()) {
        return;
    }

    state()->setJumpPad(true);
    s32 jumpPadVariant = state()->jumpPadVariant();
    m_jumpPadProperties = &JUMP_PAD_PROPERTIES[jumpPadVariant];

    if (jumpPadVariant == 3 || jumpPadVariant == 4) {
        if (m_jumpPadBoostMultiplier > 1.3f || m_jumpPadSoftSpeedLimit > 110.0f) {
            // Set speed to 100 if the player has boost from a boost panel or mushroom(item) before
            // hitting the jump pad
            static constexpr std::array<JumpPadProperties, 2> JUMP_PAD_PROPERTIES_SHROOM_BOOST = {{
                    {100.0f, 100.0f, 70.0f},
                    {100.0f, 100.0f, 65.0f},
            }};
            m_jumpPadProperties = &JUMP_PAD_PROPERTIES_SHROOM_BOOST[jumpPadVariant != 3];
        }
        state()->setJumpPadFixedSpeed(true);
    }

    if (jumpPadVariant == 4) {
        state()->setJumpPadMushroomTrigger(true);
        state()->setJumpPadMushroomVelYInc(true);
        state()->setJumpPadMushroomCollision(true);
    } else {
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
    if (state()->isJumpPadMushroomTrigger()) {
        if (state()->isGroundStart()) {
            state()->setJumpPadMushroomTrigger(false);
            state()->setJumpPadFixedSpeed(false);
            state()->setJumpPadMushroomVelYInc(false);
        }

        if (state()->isJumpPadMushroomVelYInc()) {
            EGG::Vector3f newExtVel = dynamics()->extVel();
            newExtVel.y += 20.0f;
            if (m_jumpPadProperties->velY < newExtVel.y) {
                newExtVel.y = m_jumpPadProperties->velY;
                state()->setJumpPadMushroomVelYInc(false);
            }
            dynamics()->setExtVel(newExtVel);
        }
    }

    if (state()->isGroundStart() && !state()->isJumpPadMushroomTrigger()) {
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

    if (state()->isBeforeRespawn() || state()->isInAction()) {
        return;
    }

    activateBoost(KartBoost::Type::MushroomAndBoostPanel, MUSHROOM_DURATION);

    m_mushroomBoostTimer = MUSHROOM_DURATION;
    state()->setMushroomBoost(true);
    setOffroadInvincibility(MUSHROOM_DURATION);
}

/// @addr{0x8057F96C}
void KartMove::activateZipperBoost() {
    constexpr s16 BASE_DURATION = 50;
    constexpr s16 TRICK_DURATION = 100;

    if (state()->isBeforeRespawn() || state()->isInAction()) {
        return;
    }

    s16 boostDuration = state()->isZipperTrick() ? TRICK_DURATION : BASE_DURATION;
    activateBoost(KartBoost::Type::TrickAndZipper, boostDuration);

    setOffroadInvincibility(boostDuration);
    m_zipperBoostTimer = 0;
    m_zipperBoostMax = boostDuration;
    state()->setZipperBoost(true);
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

/// @addr{0x80582E34}
void KartMove::calcZipperBoost() {
    if (!state()->isZipperBoost()) {
        return;
    }

    state()->setAccelerate(true);

    if (!state()->isOverZipper() && ++m_zipperBoostTimer >= m_zipperBoostMax) {
        m_zipperBoostTimer = 0;
        state()->setZipperBoost(false);
    }

    if (m_zipperBoostTimer < 10) {
        EGG::Vector3f angVel = dynamics()->angVel0();
        angVel.y = 0.0f;
        dynamics()->setAngVel0(angVel);
    }
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

    if (state()->isBeforeRespawn() || state()->isInAction()) {
        return;
    }

    s16 duration;
    if (isBike()) {
        duration = BIKE_TRICK_BOOST_DURATION[static_cast<u32>(m_jump->variant())];
    } else {
        duration = KART_TRICK_BOOST_DURATION[static_cast<u32>(m_jump->variant())];
    }

    activateBoost(KartBoost::Type::TrickAndZipper, duration);
}

/// @addr{0x80580F28}
void KartMove::activateCrush(u16 timer) {
    state()->setCrushed(true);
    m_crushTimer = timer;
    m_kartScale->startCrush();
}

/// @addr{0x80580F9C}
void KartMove::calcCrushed() {
    if (!state()->isCrushed()) {
        return;
    }

    if (--m_crushTimer == 0) {
        state()->setCrushed(false);
        m_kartScale->startUncrush();
    }
}

/// @addr{0x8058160C}
void KartMove::calcScale() {
    m_kartScale->calc();
    setScale(m_kartScale->currScale());
}

/// @addr{0x80586DB4}
void KartMove::applyBumpForce(f32 speed, const EGG::Vector3f &hitDir, bool resetSpeed) {
    constexpr s16 BUMP_COOLDOWN = 5;

    if (m_bumpTimer >= 1) {
        return;
    }

    dynamics()->addForce(speed * hitDir.perpInPlane(move()->up(), true));
    collide()->startFloorMomentRate();

    m_bumpTimer = BUMP_COOLDOWN;

    if (resetSpeed) {
        m_speed = 0.0f;
    }
}

/// @addr{0x8058498C}
void KartMove::enterCannon() {
    init(true, true);
    physics()->clearDecayingRot();
    m_boost.resetActive();
    state()->setBoost(false);

    cancelJumpPad();
    clearRampBoost();
    clearZipperBoost();
    clearSsmt();
    clearOffroadInvincibility();

    dynamics()->reset();

    clearDrift();
    state()->setHop(false);
    state()->setInCannon(true);
    state()->setSkipWheelCalc(true);
    state()->setCannonStart(false);

    const auto [cannonPos, cannonDir] = getCannonPosRot();
    m_cannonEntryPos = pos();
    m_cannonEntryOfs = cannonPos - pos();
    m_cannonEntryOfsLength = m_cannonEntryOfs.normalise();
    m_cannonEntryOfs.normalise();
    m_dir = m_cannonEntryOfs;
    m_vel1Dir = m_cannonEntryOfs;
    m_cannonOrthog = EGG::Vector3f::ey.perpInPlane(m_cannonEntryOfs, true);
    m_cannonProgress.setZero();
}

/// @addr{0x80584D58}
void KartMove::calcCannon() {
    auto [cannonPos, cannonDir] = getCannonPosRot();
    EGG::Vector3f forwardXZ = cannonPos - m_cannonEntryPos - m_cannonProgress;
    EGG::Vector3f forward = forwardXZ;
    f32 forwardLength = forward.normalise();
    forwardXZ.y = 0;
    forwardXZ.normalise();
    EGG::Vector3f local94 = m_cannonEntryOfs;
    local94.y = 0;
    local94.normalise();
    m_speedRatioCapped = 1.0f;
    m_speedRatio = 1.5f;
    EGG::Matrix34f cannonOrientation;
    cannonOrientation.makeOrthonormalBasis(forward, EGG::Vector3f::ey);
    EGG::Vector3f up = cannonOrientation.multVector33(EGG::Vector3f::ey);
    m_smoothedUp = up;
    m_up = up;

    if (forwardLength < 30.0f || local94.dot(forwardXZ) <= 0.0f) {
        exitCannon();
        return;
    }
    m_speed = m_baseSpeed;
    const auto *cannonPoint =
            System::CourseMap::Instance()->getCannonPoint(state()->cannonPointId());
    size_t cannonParameterIdx = std::max<s16>(0, cannonPoint->parameterIdx());
    ASSERT(cannonParameterIdx < CANNON_PARAMETERS.size());
    const auto &cannonParams = CANNON_PARAMETERS[cannonParameterIdx];
    f32 newSpeed = cannonParams.speed;
    if (forwardLength < cannonParams.decelFactor) {
        f32 factor = std::max(0.0f, forwardLength / cannonParams.decelFactor);

        newSpeed = cannonParams.endDecel;
        if (newSpeed <= 0.0f) {
            newSpeed = m_baseSpeed;
        }

        newSpeed += factor * (cannonParams.speed - newSpeed);
        if (cannonParams.endDecel > 0.0f) {
            m_speed = std::min(newSpeed, m_speed);
        }
    }

    m_cannonProgress += m_cannonEntryOfs * newSpeed;

    EGG::Vector3f newPos = EGG::Vector3f::zero;
    if (cannonParams.height > 0.0f) {
        f32 fVar9 = EGG::Mathf::SinFIdx(
                (1.0f - (forwardLength / m_cannonEntryOfsLength)) * 180.0f * DEG2FIDX);
        newPos = fVar9 * cannonParams.height * m_cannonOrthog;
    }

    dynamics()->setPos(m_cannonEntryPos + m_cannonProgress + newPos);
    m_dir = m_cannonEntryOfs;
    m_vel1Dir = m_cannonEntryOfs;

    calcRotCannon(forward);

    dynamics()->setExtVel(EGG::Vector3f::zero);
}

/// @addr{0x805855BC}
void KartMove::calcRotCannon(const EGG::Vector3f &forward) {
    EGG::Vector3f local48 = forward;
    local48.normalise();
    EGG::Vector3f local54 = bodyFront();
    EGG::Vector3f local60 = local54 + ((local48 - local54) * 0.3f);
    local54.normalise();
    local60.normalise();
    // also local70, localA8
    EGG::Quatf local80;
    local80.makeVectorRotation(local54, local60);
    local80 *= dynamics()->fullRot();
    local80.normalise();
    EGG::Quatf localB8;
    localB8.makeVectorRotation(local80.rotateVector(EGG::Vector3f::ey), smoothedUp());
    EGG::Quatf newRot = local80.slerpTo(localB8.multSwap(local80), 0.3f);
    dynamics()->setFullRot(newRot);
    dynamics()->setMainRot(newRot);
}

/// @addr{0x805852C8}
void KartMove::exitCannon() {
    if (!state()->isInCannon()) {
        return;
    }

    state()->setInCannon(false);
    state()->setSkipWheelCalc(false);
    state()->setAfterCannon(true);
    dynamics()->setIntVel(m_cannonEntryOfs * m_speed);
}

/// @addr{0x805799AC}
void KartMove::triggerRespawn() {
    m_timeInRespawn = 0;
    state()->setTriggerRespawn(true);
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
    m_autoHardStickXFrames = 0;
}

/// @addr{0x805883C4}
/// @stage 1+
/// @brief Clears the wheelie bit flag and resets the rotation decrement.
void KartMoveBike::cancelWheelie() {
    state()->setWheelie(false);
    m_wheelieRotDec = 0.0f;
    m_autoHardStickXFrames = 0;
}

/// @addr{0x80587BB8}
void KartMoveBike::createSubsystems() {
    m_jump = new KartJumpBike(this);
    m_halfPipe = new KartHalfPipe();
    m_kartScale = new KartScale();
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

    if (state()->isBeforeRespawn() || state()->isInAction() || state()->isWheelie() ||
            state()->isOverZipper() || state()->isRejectRoadTrigger() ||
            state()->isAirtimeOver20() || state()->isSoftWallDrift() ||
            state()->isSomethingWallCollision() || state()->isHWG() || state()->isCannonStart() ||
            state()->isInCannon()) {
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

    EGG::Vector3f top = m_up;

    if (!state()->isRejectRoad() && !state()->isHalfPipeRamp() && !state()->isOverZipper()) {
        f32 scalar = (m_speed >= 0.0f) ? m_speedRatioCapped * 2.0f : 0.0f;
        scalar = std::min(1.0f, scalar);
        top = scalar * m_up + (1.0f - scalar) * EGG::Vector3f::ey;

        if (std::numeric_limits<f32>::epsilon() < top.squaredLength()) {
            top.normalise();
        }
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

    if (param()->stats().driftType == KartParam::Stats::DriftType::Outside_Drift_Bike) {
        m_turningParams = &TURNING_PARAMS_ARRAY[0];
    } else if (param()->stats().driftType == KartParam::Stats::DriftType::Inside_Drift_Bike) {
        m_turningParams = &TURNING_PARAMS_ARRAY[1];
    }

    if (System::RaceManager::Instance()->isStageReached(System::RaceManager::Stage::Race)) {
        m_leanRotInc = m_turningParams->leanRotIncRace;
        m_leanRotCap = m_turningParams->leanRotCapRace;
    } else {
        m_leanRotInc = m_turningParams->leanRotIncCountdown;
        m_leanRotCap = m_turningParams->leanRotCapCountdown;
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
    m_wheelieFrames = 0;
    m_wheelieCooldown = 0;
    m_autoHardStickXFrames = 0;
}

/// @addr{0x80588950}
void KartMoveBike::clear() {
    KartMove::clear();
    cancelWheelie();
}

/// @brief STAGE 1+ - Every frame, checks player input for wheelies and computes wheelie rotation.
/// @addr{0x805883F4}
void KartMoveBike::calcWheelie() {
    constexpr u32 FAILED_WHEELIE_FRAMES = 15;
    constexpr f32 AUTO_WHEELIE_CANCEL_STICK_THRESHOLD = 0.85f;

    tryStartWheelie();
    m_wheelieCooldown = std::max(0, m_wheelieCooldown - 1);

    if (state()->isWheelie()) {
        bool cancelAutoWheelie = false;

        if (!state()->isAutoDrift() ||
                EGG::Mathf::abs(state()->stickX()) <= AUTO_WHEELIE_CANCEL_STICK_THRESHOLD) {
            m_autoHardStickXFrames = 0;
        } else {
            if (++m_autoHardStickXFrames > 15) {
                cancelAutoWheelie = true;
            }
        }

        ++m_wheelieFrames;
        if (m_turningParams->maxWheelieFrames < m_wheelieFrames || cancelAutoWheelie ||
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

    if (m_mtCharge > MAX_MT_CHARGE) {
        m_mtCharge = MAX_MT_CHARGE;
        m_driftState = DriftState::ChargedMt;
    }
}

/// @addr{0x80588B58}
void KartMoveBike::initOob() {
    KartMove::initOob();
    cancelWheelie();
}

/// @brief STAGE 1+ - Every frame, checks player input to see if we should start or stop a wheelie.
/// @addr{0x80588798}
void KartMoveBike::tryStartWheelie() {
    constexpr s16 COOLDOWN_FRAMES = 20;
    bool dpadUp = inputs()->currentState().trickUp();

    if (!state()->isWheelie()) {
        if (dpadUp && state()->isTouchingGround()) {
            if (state()->isDriftManual() || state()->isWallCollision() ||
                    state()->isWall3Collision() || state()->isHop() || state()->isDriftAuto() ||
                    state()->isInAction()) {
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

} // namespace Kart
