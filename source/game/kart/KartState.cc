#include "KartState.hh"

#include "game/kart/CollisionGroup.hh"
#include "game/kart/KartCollide.hh"
#include "game/kart/KartDynamics.hh"
#include "game/kart/KartJump.hh"
#include "game/kart/KartMove.hh"

#include "game/system/RaceManager.hh"

namespace Kart {

struct StartBoostEntry {
    f32 range;
    s16 frames;
};

/// @addr{0x808B64F8}
/// @memberof KartState
static constexpr std::array<StartBoostEntry, 6> START_BOOST_ENTRIES = {{
        {0.85f, 0},
        {0.88f, 10},
        {0.905f, 20},
        {0.925f, 30},
        {0.94f, 45},
        {0.95f, 70},
}};

/// @addr{0x805943B4}
KartState::KartState() {
    clearBitfield0();
    clearBitfield1();

    m_bWheelieRot = false;
    m_bSkipWheelCalc = false;
    m_bJumpPadDisableYsusForce = false;

    clearBitfield3();

    m_bAutoDrift = inputs()->driftIsAuto();

    m_airtime = 0;
    m_cannonPointId = 0;
    m_startBoostIdx = 0;
}

/// @addr{0x8059455C}
void KartState::init() {
    reset();
}

/// @addr{0x80594594}
void KartState::reset() {
    clearBitfield3();

    m_bWheelieRot = false;
    m_bSkipWheelCalc = false;
    m_bJumpPadDisableYsusForce = false;

    clearBitfield1();
    clearBitfield0();

    m_airtime = 0;
    m_top.setZero();
    m_hwgTimer = 0;
    m_boostRampType = -1;
    m_jumpPadVariant = -1;
    m_halfPipeInvisibilityTimer = 0;
    m_startBoostCharge = 0.0f;
    m_stickX = 0.0f;
    m_wallBonkTimer = 0;
    m_trickableTimer = 0;
}

/// @stage 1+
/// @brief Each frame, read input and save related bit flags. Also handles start boosts.
/// @addr{0x8059487C}
void KartState::calcInput() {
    const auto *raceMgr = System::RaceManager::Instance();
    if (raceMgr->isStageReached(System::RaceManager::Stage::Race)) {
        if (!state()->isBeforeRespawn() && !state()->isCannonStart() && !state()->isInCannon() &&
                !state()->isOverZipper()) {
            const auto &currentState = inputs()->currentState();
            const auto &lastState = inputs()->lastState();
            m_stickX = currentState.stick.x;
            m_stickY = currentState.stick.y;

            if (!state()->isRejectRoadTrigger()) {
                if (m_stickX < 0.0f) {
                    m_bStickLeft = true;
                } else if (m_stickX > 0.0f) {
                    m_bStickRight = true;
                }
            }

            m_bAccelerate = currentState.accelerate();
            m_bAccelerateStart = m_bAccelerate && !lastState.accelerate();
            m_bBrake = currentState.brake();
            if (!m_bAutoDrift) {
                m_bDriftInput = currentState.drift();
                m_bHopStart = m_bDriftInput && !lastState.drift();
            }
        }

        calcHandleStartBoost();
    } else {
        if (!raceMgr->isStageReached(System::RaceManager::Stage::Countdown)) {
            return;
        }

        const auto &currentState = inputs()->currentState();
        m_stickX = currentState.stick.x;
        m_bChargeStartBoost = currentState.accelerate();

        calcStartBoost();
    }
}

/// @stage All
/// @brief Every frame, resets the input state and saves collision-related bit flags.
/// @addr{0x8059474C}
void KartState::calc() {
    resetFlags();

    calcCollisions();
    collide()->calcBoundingRadius();
}

/// @addr{0x80594704}
void KartState::resetFlags() {
    m_bAccelerate = false;
    m_bBrake = false;
    m_bDriftInput = false;
    m_bHopStart = false;
    m_bAccelerateStart = false;
    m_bGroundStart = false;
    m_bStickLeft = false;
    m_bWallCollisionStart = false;
    m_bAirStart = false;
    m_bStickRight = false;

    m_bJumpPadDisableYsusForce = false;

    m_stickY = 0.0f;
    m_stickX = 0.0f;
}

/// @stage All
/// @brief Each frame, checks for collision and saves relevant bit flags.
/// @addr{0x80594BD4}
/// @details Iterates each tire to check for collision. If any tire is colliding with the floor,
/// the "Any Wheel Collision" bit is set. If all tires are colliding with the floor, the
/// "All Wheels Collision" bit is set. Tracks airtime and computes the appropriate
/// top vector, given the floor normals of all colliding floor KCLs.
void KartState::calcCollisions() {
    bool wasTouchingGround = m_bTouchingGround;
    bool wasWallCollision = m_bWallCollision || m_bWall3Collision;

    m_bWall3Collision = false;
    m_bWallCollision = false;
    m_bVehicleBodyFloorCollision = false;
    m_bAnyWheelCollision = false;
    m_bAllWheelsCollision = false;
    m_bTouchingGround = false;

    if (m_hwgTimer > 0) {
        if (--m_hwgTimer == 0) {
            m_bUNK2 = false;
            m_bSomethingWallCollision = false;
        }
    }

    m_top.setZero();
    bool softWallCollision = false;

    if (collide()->someSoftWallTimer() > 0) {
        if (collide()->someNonSoftWallTimer() == 0) {
            softWallCollision = true;
        } else {
            f32 softSusp = collide()->suspBottomHeightSoftWall() /
                    static_cast<f32>(collide()->someSoftWallTimer());
            f32 nonSusp = collide()->suspBottomHeightNonSoftWall() /
                    static_cast<f32>(collide()->someNonSoftWallTimer());

            if (softSusp - nonSusp >= 40.0f) {
                m_bSoftWallDrift = false;
            } else {
                softWallCollision = true;
            }
        }
    }

    u16 wheelCollisions = 0;
    u16 softWallCount = 0;
    EGG::Vector3f wallNrm;
    bool trickable = false;

    for (u16 tireIdx = 0; tireIdx < tireCount(); ++tireIdx) {
        const auto &colData = collisionData(tireIdx);
        if (hasFloorCollision(tirePhysics(tireIdx))) {
            m_top += colData.floorNrm;
            trickable = trickable || colData.bTrickable;
            ++wheelCollisions;
        }

        if (softWallCollision && colData.bSoftWall) {
            ++softWallCount;
            wallNrm += colData.noBounceWallNrm;
        }
    }

    if (wheelCollisions > 0) {
        m_bAnyWheelCollision = true;
        if (wheelCollisions == tireCount()) {
            m_bAllWheelsCollision = true;
        }
    }

    CollisionData &colData = collisionData();
    if (colData.bFloor) {
        m_bVehicleBodyFloorCollision = true;
        m_top += colData.floorNrm;
        trickable = trickable || colData.bTrickable;

        if (state()->isOverZipper()) {
            halfPipe()->end(true);
        }
    }

    bool hitboxGroupSoftWallCollision = false;
    if (softWallCollision && colData.bSoftWall) {
        hitboxGroupSoftWallCollision = true;
        ++softWallCount;
        wallNrm += colData.noBounceWallNrm;
    }

    bool bVar3 = colData.bInvisibleWallOnly && m_halfPipeInvisibilityTimer > 0;
    m_halfPipeInvisibilityTimer = std::max(0, m_halfPipeInvisibilityTimer - 1);

    m_wallBonkTimer = std::max(0, m_wallBonkTimer - 1);

    bool hwg = false;

    if ((colData.bWall || colData.bWall3) && !bVar3) {
        if (colData.bWall) {
            m_bWallCollision = true;
        }

        if (colData.bWall3) {
            m_bWall3Collision = true;
        }

        if (!wasWallCollision) {
            m_bWallCollisionStart = true;
        }

        m_wallBonkTimer = 2;

        if (m_hwgTimer == 0 && colData.movement.y > 1.0f) {
            EGG::Vector3f movement = colData.movement;
            movement.normalise();

            if (movement.dot(EGG::Vector3f::ey) > 0.8f &&
                    colData.wallNrm.dot(EGG::Vector3f::ey) > 0.85f &&
                    (movement.x * colData.wallNrm.x + movement.z * colData.wallNrm.z < 0.0f ||
                            collide()->colPerpendicularity() >= 1.0f)) {
                colData.wallNrm.y = 0.0f;
                colData.wallNrm.normalise();
                wallNrm = colData.wallNrm;

                if (wallNrm.length() < 0.05f) {
                    wallNrm = movement;
                    wallNrm.y = 0.0f;
                }

                hwg = true;
            }
        }
    }

    if (softWallCount > 0 || hwg) {
        m_bUNK2 = true;
        m_softWallSpeed = wallNrm;
        m_softWallSpeed.normalise();
        if (softWallCount > 0 && !m_bHop) {
            m_bSoftWallDrift = true;
        }

        if (hwg) {
            m_bHWG = true;
        }

        if (hitboxGroupSoftWallCollision || hwg || isBike()) {
            m_bSomethingWallCollision = true;
            m_hwgTimer = 10;

            if (hwg) {
                m_hwgTimer *= 2;
            }
        }
    }

    m_bAirtimeOver20 = false;
    m_trickableTimer = std::max(0, m_trickableTimer - 1);

    if (wheelCollisions < 1 && !colData.bFloor) {
        if (wasTouchingGround) {
            m_bAirStart = true;
        }

        if (++m_airtime > 20) {
            m_bAirtimeOver20 = true;
        }
    } else {
        m_top.normalise();

        m_bTouchingGround = true;

        if (state()->isOverZipper()) {
            halfPipe()->end(true);
        }

        if (trickable) {
            m_trickableTimer = 3;
        }

        m_bTrickable = m_trickableTimer > 0;

        if (!wasTouchingGround) {
            m_bGroundStart = true;
        }

        if (m_bInATrick && jump()->cooldown() == 0) {
            move()->landTrick();
            dynamics()->setForceUpright(true);
            jump()->end();
        }

        m_airtime = 0;
    }
}

/// @brief STAGE 1 - Each frame, calculates the start boost charge.
/// @addr{0x80595918}
/// @details If the player is holding accelerate, the start boost charge increases using exponential
/// decay. If the player is not holding accelerate, the start boost charge decays by 4% each frame.
void KartState::calcStartBoost() {
    constexpr f32 START_BOOST_DELTA_ONE = 0.02f;
    constexpr f32 START_BOOST_DELTA_TWO = 0.002f;
    constexpr f32 START_BOOST_FALLOFF = 0.96f;

    if (m_bChargeStartBoost) {
        m_startBoostCharge += START_BOOST_DELTA_ONE -
                (START_BOOST_DELTA_ONE - START_BOOST_DELTA_TWO) * m_startBoostCharge;
    } else {
        m_startBoostCharge *= START_BOOST_FALLOFF;
    }

    m_startBoostCharge = std::max(0.0f, std::min(1.0f, m_startBoostCharge));
}

/// @stage 1
/// @brief On countdown end, calculates and applies our start boost charge.
/// @addr{0x805959D4}
void KartState::calcHandleStartBoost() {
    if (System::RaceManager::Instance()->getCountdownTimer() != 0) {
        return;
    }

    if (m_bAccelerate) {
        if (m_startBoostCharge > START_BOOST_ENTRIES.back().range) {
            m_startBoostIdx = std::numeric_limits<size_t>::max();
        } else if (m_startBoostCharge > START_BOOST_ENTRIES.front().range) {
            // Ranges are exclusive on the lower bound and inclusive on the upper bound
            for (size_t i = 1; i < START_BOOST_ENTRIES.size(); ++i) {
                if (m_startBoostCharge > START_BOOST_ENTRIES[i - 1].range &&
                        m_startBoostCharge <= START_BOOST_ENTRIES[i].range) {
                    m_startBoostIdx = i;
                    break;
                }
            }
        }
    }

    if (m_startBoostIdx <= 0) {
        return;
    }

    handleStartBoost(m_startBoostIdx);
    m_bChargeStartBoost = false;
}

/// @brief Applies the relevant start boost duration.
/// @addr{0x80595AF8}
/// @param idx The index into the start boost entries array.
void KartState::handleStartBoost(size_t idx) {
    if (m_startBoostIdx == std::numeric_limits<size_t>::max()) {
        PANIC("More burnout RE required. See KartMoveSub264 function 0x805890b0.");
    }
    move()->applyStartBoost(START_BOOST_ENTRIES[idx].frames);
}

/// @brief Resets certain bitfields pertaining to ejections (reject road, half pipe zippers, etc.)
/// @addr{0x805958F0}
void KartState::resetEjection() {
    m_bHalfPipeRamp = false;
    m_bRejectRoad = false;
}

bool KartState::isDrifting() const {
    return m_bDriftManual || m_bDriftAuto;
}

bool KartState::isAccelerate() const {
    return m_bAccelerate;
}

bool KartState::isBrake() const {
    return m_bBrake;
}

bool KartState::isDriftInput() const {
    return m_bDriftInput;
}

bool KartState::isDriftManual() const {
    return m_bDriftManual;
}

bool KartState::isBeforeRespawn() const {
    return m_bBeforeRespawn;
}

bool KartState::isWall3Collision() const {
    return m_bWall3Collision;
}

bool KartState::isWallCollision() const {
    return m_bWallCollision;
}

bool KartState::isHopStart() const {
    return m_bHopStart;
}

bool KartState::isGroundStart() const {
    return m_bGroundStart;
}

bool KartState::isVehicleBodyFloorCollision() const {
    return m_bVehicleBodyFloorCollision;
}

bool KartState::isAnyWheelCollision() const {
    return m_bAnyWheelCollision;
}

bool KartState::isAllWheelsCollision() const {
    return m_bAllWheelsCollision;
}

bool KartState::isStickLeft() const {
    return m_bStickLeft;
}

bool KartState::isWallCollisionStart() const {
    return m_bWallCollisionStart;
}

bool KartState::isAirtimeOver20() const {
    return m_bAirtimeOver20;
}

bool KartState::isStickyRoad() const {
    return m_bStickyRoad;
}

bool KartState::isTouchingGround() const {
    return m_bTouchingGround;
}

bool KartState::isHop() const {
    return m_bHop;
}

bool KartState::isSoftWallDrift() const {
    return m_bSoftWallDrift;
}

bool KartState::isHWG() const {
    return m_bHWG;
}

bool KartState::isChargeStartBoost() const {
    return m_bChargeStartBoost;
}

bool KartState::isBoost() const {
    return m_bBoost;
}

bool KartState::isAirStart() const {
    return m_bAirStart;
}

bool KartState::isStickRight() const {
    return m_bStickRight;
}

bool KartState::isMushroomBoost() const {
    return m_bMushroomBoost;
}

bool KartState::isDriftAuto() const {
    return m_bDriftAuto;
}

bool KartState::isSlipdriftCharge() const {
    return m_bSlipdriftCharge;
}

bool KartState::isWheelie() const {
    return m_bWheelie;
}

bool KartState::isJumpPad() const {
    return m_bJumpPad;
}

bool KartState::isRampBoost() const {
    return m_bRampBoost;
}

bool KartState::isCannonStart() const {
    return m_bCannonStart;
}

bool KartState::isInCannon() const {
    return m_bInCannon;
}

bool KartState::isTrickStart() const {
    return m_bTrickStart;
}

bool KartState::isInATrick() const {
    return m_bInATrick;
}

bool KartState::isBoostOffroadInvincibility() const {
    return m_bBoostOffroadInvincibility;
}

bool KartState::isHalfPipeRamp() const {
    return m_bHalfPipeRamp;
}

bool KartState::isOverZipper() const {
    return m_bOverZipper;
}

bool KartState::isDisableBackwardsAccel() const {
    return m_bDisableBackwardsAccel;
}

bool KartState::isZipperBoost() const {
    return m_bZipperBoost;
}

bool KartState::isZipperTrick() const {
    return m_bZipperTrick;
}

bool KartState::isZipperStick() const {
    return m_bZipperStick;
}

bool KartState::isTrickRot() const {
    return m_bTrickRot;
}

bool KartState::isChargingSsmt() const {
    return m_bChargingSsmt;
}

bool KartState::isRejectRoad() const {
    return m_bRejectRoad;
}

bool KartState::isRejectRoadTrigger() const {
    return m_bRejectRoadTrigger;
}

bool KartState::isTrickable() const {
    return m_bTrickable;
}

bool KartState::isWheelieRot() const {
    return m_bWheelieRot;
}

bool KartState::isJumpPadDisableYsusForce() const {
    return m_bJumpPadDisableYsusForce;
}

bool KartState::isSkipWheelCalc() const {
    return m_bSkipWheelCalc;
}

bool KartState::isUNK2() const {
    return m_bUNK2;
}

bool KartState::isSomethingWallCollision() const {
    return m_bSomethingWallCollision;
}

bool KartState::isAutoDrift() const {
    return m_bAutoDrift;
}

u16 KartState::cannonPointId() const {
    return m_cannonPointId;
}

s32 KartState::boostRampType() const {
    return m_boostRampType;
}

s32 KartState::jumpPadVariant() const {
    return m_jumpPadVariant;
}

f32 KartState::stickX() const {
    return m_stickX;
}

f32 KartState::stickY() const {
    return m_stickY;
}

u32 KartState::airtime() const {
    return m_airtime;
}

const EGG::Vector3f &KartState::top() const {
    return m_top;
}

const EGG::Vector3f &KartState::softWallSpeed() const {
    return m_softWallSpeed;
}

f32 KartState::startBoostCharge() const {
    return m_startBoostCharge;
}

s16 KartState::wallBonkTimer() const {
    return m_wallBonkTimer;
}

s16 KartState::trickableTimer() const {
    return m_trickableTimer;
}

/// @brief Helper function to clear all bit flags at 0x4-0x7 in KartState.
void KartState::clearBitfield0() {
    m_bAccelerate = false;
    m_bBrake = false;
    m_bDriftInput = false;
    m_bDriftManual = false;
    m_bBeforeRespawn = false;
    m_bWall3Collision = false;
    m_bWallCollision = false;
    m_bHopStart = false;
    m_bAccelerateStart = false;
    m_bGroundStart = false;
    m_bVehicleBodyFloorCollision = false;
    m_bAnyWheelCollision = false;
    m_bAllWheelsCollision = false;
    m_bStickLeft = false;
    m_bWallCollisionStart = false;
    m_bAirtimeOver20 = false;
    m_bStickyRoad = false;
    m_bTouchingGround = false;
    m_bHop = false;
    m_bBoost = false;
    m_bAirStart = false;
    m_bStickRight = false;
    m_bMushroomBoost = false;
    m_bDriftAuto = false;
    m_bSlipdriftCharge = false;
    m_bWheelie = false;
    m_bJumpPad = false;
    m_bRampBoost = false;
}

/// @brief Helper function to clear all bit flags at 0x8-0xB in KartState.
void KartState::clearBitfield1() {
    m_bCannonStart = false;
    m_bInCannon = false;
    m_bTrickStart = false;
    m_bInATrick = false;
    m_bBoostOffroadInvincibility = false;
    m_bHalfPipeRamp = false;
    m_bOverZipper = false;
    m_bDisableBackwardsAccel = false;
    m_bZipperBoost = false;
    m_bZipperStick = false;
    m_bZipperTrick = false;
    m_bTrickRot = false;
    m_bChargingSsmt = false;
    m_bRejectRoad = false;
    m_bRejectRoadTrigger = false;
    m_bTrickable = false;
}

/// @brief Helper function to clear all bit flags at 0x10-0x13 in KartState.
void KartState::clearBitfield3() {
    m_bUNK2 = false;
    m_bSomethingWallCollision = false;
    m_bSoftWallDrift = false;
    m_bHWG = false;
    m_bChargeStartBoost = false;
}

void KartState::setAccelerate(bool isSet) {
    m_bAccelerate = isSet;
}

void KartState::setDriftInput(bool isSet) {
    m_bDriftInput = isSet;
}

void KartState::setDriftManual(bool isSet) {
    m_bDriftManual = isSet;
}

void KartState::setBeforeRespawn(bool isSet) {
    m_bBeforeRespawn = isSet;
}

void KartState::setWallCollision(bool isSet) {
    m_bWallCollision = isSet;
}

void KartState::setHopStart(bool isSet) {
    m_bHopStart = isSet;
}

void KartState::setVehicleBodyFloorCollision(bool isSet) {
    m_bVehicleBodyFloorCollision = isSet;
}

void KartState::setAnyWheelCollision(bool isSet) {
    m_bAnyWheelCollision = isSet;
}

void KartState::setWallCollisionStart(bool isSet) {
    m_bWallCollisionStart = isSet;
}

void KartState::setStickyRoad(bool isSet) {
    m_bStickyRoad = isSet;
}

void KartState::setAllWheelsCollision(bool isSet) {
    m_bAllWheelsCollision = isSet;
}

void KartState::setTouchingGround(bool isSet) {
    m_bTouchingGround = isSet;
}

void KartState::setHop(bool isSet) {
    m_bHop = isSet;
}

void KartState::setBoost(bool isSet) {
    m_bBoost = isSet;
}

void KartState::setMushroomBoost(bool isSet) {
    m_bMushroomBoost = isSet;
}

void KartState::setSlipdriftCharge(bool isSet) {
    m_bSlipdriftCharge = isSet;
}

void KartState::setWheelie(bool isSet) {
    m_bWheelie = isSet;
}

void KartState::setJumpPad(bool isSet) {
    m_bJumpPad = isSet;
}

void KartState::setRampBoost(bool isSet) {
    m_bRampBoost = isSet;
}

void KartState::setCannonStart(bool isSet) {
    m_bCannonStart = isSet;
}

void KartState::setInCannon(bool isSet) {
    m_bInCannon = isSet;
}

void KartState::setTrickStart(bool isSet) {
    m_bTrickStart = isSet;
}

void KartState::setInATrick(bool isSet) {
    m_bInATrick = isSet;
}

void KartState::setBoostOffroadInvincibility(bool isSet) {
    m_bBoostOffroadInvincibility = isSet;
}

void KartState::setHalfPipeRamp(bool isSet) {
    m_bHalfPipeRamp = isSet;
}

void KartState::setOverZipper(bool isSet) {
    m_bOverZipper = isSet;
}

void KartState::setDisableBackwardsAccel(bool isSet) {
    m_bDisableBackwardsAccel = isSet;
}

void KartState::setZipperBoost(bool isSet) {
    m_bZipperBoost = isSet;
}

void KartState::setZipperStick(bool isSet) {
    m_bZipperStick = isSet;
}

void KartState::setZipperTrick(bool isSet) {
    m_bZipperTrick = isSet;
}

void KartState::setTrickRot(bool isSet) {
    m_bTrickRot = isSet;
}

void KartState::setChargingSsmt(bool isSet) {
    m_bChargingSsmt = isSet;
}

void KartState::setRejectRoad(bool isSet) {
    m_bRejectRoad = isSet;
}

void KartState::setRejectRoadTrigger(bool isSet) {
    m_bRejectRoadTrigger = isSet;
}

void KartState::setTrickable(bool isSet) {
    m_bTrickable = isSet;
}

void KartState::setWheelieRot(bool isSet) {
    m_bWheelieRot = isSet;
}

void KartState::setSkipWheelCalc(bool isSet) {
    m_bSkipWheelCalc = isSet;
}

void KartState::setJumpPadDisableYsusForce(bool isSet) {
    m_bJumpPadDisableYsusForce = isSet;
}

void KartState::setSomethingWallCollision(bool isSet) {
    m_bSomethingWallCollision = isSet;
}

void KartState::setSoftWallDrift(bool isSet) {
    m_bSoftWallDrift = isSet;
}

void KartState::setHWG(bool isSet) {
    m_bHWG = isSet;
}

void KartState::setCannonPointId(u16 val) {
    m_cannonPointId = val;
}

void KartState::setBoostRampType(s32 val) {
    m_boostRampType = val;
}

void KartState::setJumpPadVariant(s32 val) {
    m_jumpPadVariant = val;
}

void KartState::setHalfPipeInvisibilityTimer(s16 val) {
    m_halfPipeInvisibilityTimer = val;
}

void KartState::setTrickableTimer(s16 val) {
    m_trickableTimer = val;
}

} // namespace Kart
