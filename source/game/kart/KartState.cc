#include "KartState.hh"

#include "game/kart/CollisionGroup.hh"
#include "game/kart/KartCollide.hh"
#include "game/kart/KartJump.hh"
#include "game/kart/KartMove.hh"

#include "game/system/RaceManager.hh"

namespace Kart {

struct StartBoostEntry {
    f32 range;
    s16 frames;
};

static constexpr std::array<StartBoostEntry, 6> START_BOOST_ENTRIES = {{
        {0.85f, 0},
        {0.88f, 10},
        {0.905f, 20},
        {0.925f, 30},
        {0.94f, 45},
        {0.95f, 70},
}};

KartState::KartState() {
    clearBitfield0();
    clearBitfield1();

    m_bWheelieRot = false;
    m_bJumpPadDisableYsusForce = false;

    clearBitfield3();

    m_bAutoDrift = inputs()->driftIsAuto();

    m_airtime = 0;
}

void KartState::init() {
    reset();
}

void KartState::reset() {
    clearBitfield3();

    m_bWheelieRot = false;
    m_bJumpPadDisableYsusForce = false;

    clearBitfield1();
    clearBitfield0();

    m_airtime = 0;
    m_top.setZero();
    m_hwgTimer = 0;
    m_boostRampType = -1;
    m_jumpPadVariant = -1;
    m_startBoostCharge = 0.0f;
    m_stickX = 0.0f;
    m_trickableTimer = 0;
}

void KartState::calcInput() {
    const auto *raceMgr = System::RaceManager::Instance();
    if (raceMgr->isStageReached(System::RaceManager::Stage::Race)) {
        const auto &currentState = inputs()->currentState();
        const auto &lastState = inputs()->lastState();
        m_stickX = currentState.stick.x;
        m_stickY = currentState.stick.y;

        if (m_stickX < 0.0f) {
            m_bStickLeft = true;
        } else if (m_stickX > 0.0f) {
            m_bStickRight = true;
        }

        m_bAccelerate = currentState.accelerate();
        m_bAccelerateStart = m_bAccelerate && !lastState.accelerate();
        m_bBrake = currentState.brake();
        if (!m_bAutoDrift) {
            m_bDriftInput = currentState.drift();
            m_bHopStart = m_bDriftInput && !lastState.drift();
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

void KartState::calc() {
    m_bStickLeft = false;
    m_bStickRight = false;
    m_bGroundStart = false;
    m_bJumpPadDisableYsusForce = false;

    m_stickY = 0.0f;
    m_stickX = 0.0f;
    calcCollisions();
}

void KartState::calcCollisions() {
    bool wasTouchingGround = state()->isTouchingGround();

    state()->setVehicleBodyFloorCollision(false);
    state()->setAnyWheelCollision(false);
    state()->setAllWheelsCollision(false);
    state()->setTouchingGround(false);

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
            m_bSoftWallDrift = (softSusp - nonSusp < 40.0f);
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
        state()->setAnyWheelCollision(true);
        if (wheelCollisions == tireCount()) {
            state()->setAllWheelsCollision(true);
        }
    }

    CollisionData &colData = collisionData();
    if (colData.bFloor) {
        state()->setVehicleBodyFloorCollision(true);
        m_top += colData.floorNrm;
        trickable = trickable || colData.bTrickable;
    }

    bool hitboxGroupSoftWallCollision = false;
    if (softWallCollision && colData.bSoftWall) {
        hitboxGroupSoftWallCollision = true;
        ++softWallCount;
        wallNrm += colData.noBounceWallNrm;
    }

    if (m_hwgTimer == 0 && colData.movement.y > 1.0f) {
        EGG::Vector3f movement = colData.movement;
        movement.normalise();

        if (movement.dot(EGG::Vector3f::ey) > 0.8f &&
                colData.floorNrm.dot(EGG::Vector3f::ey) > 0.85f &&
                movement.dot(colData.floorNrm) < 0.0f) {
            colData.floorNrm.y = 0.0f;
            colData.floorNrm.normalise();
            wallNrm = colData.floorNrm;

            if (wallNrm.length() < 0.05f) {
                wallNrm = movement;
                wallNrm.y = 0.0f;
            }
        }
    }

    if (softWallCount > 0) {
        m_bUNK2 = true;
        m_softWallSpeed = wallNrm;
        m_softWallSpeed.normalise();
        if (!m_bHop) {
            m_bSoftWallDrift = true;
        }

        if (hitboxGroupSoftWallCollision || isBike()) {
            m_bSomethingWallCollision = true;
            m_hwgTimer = 10;
        }
    }

    m_bAirtimeOver20 = false;
    m_trickableTimer = std::max(0, m_trickableTimer - 1);

    if (wheelCollisions < 1 && !colData.bFloor) {
        if (++m_airtime > 20) {
            m_bAirtimeOver20 = true;
        }
    } else {
        m_top.normalise();

        m_bTouchingGround = true;

        if (trickable) {
            m_trickableTimer = 3;
        }

        m_bTrickable = m_trickableTimer > 0;

        if (!wasTouchingGround) {
            m_bGroundStart = true;
        }

        if (m_bInATrick && jump()->cooldown() == 0) {
            move()->landTrick();
            jump()->end();
        }

        m_airtime = 0;
    }
}

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

    handleStartBoost(m_startBoostIdx);
    m_bChargeStartBoost = false;
}

void KartState::handleStartBoost(size_t idx) {
    if (m_startBoostIdx == std::numeric_limits<size_t>::max()) {
        K_PANIC("More burnout RE required. See KartMoveSub264 function 0x805890b0.");
    }
    move()->applyStartBoost(START_BOOST_ENTRIES[idx].frames);
}

bool KartState::isDrifting() const {
    return m_bDriftManual || m_bDriftAuto;
}

bool KartState::isAccelerate() const {
    return m_bAccelerate;
}

bool KartState::isDriftInput() const {
    return m_bDriftInput;
}

bool KartState::isDriftManual() const {
    return m_bDriftManual;
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

bool KartState::isChargeStartBoost() const {
    return m_bChargeStartBoost;
}

bool KartState::isBoost() const {
    return m_bBoost;
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

bool KartState::isTrickStart() const {
    return m_bTrickStart;
}

bool KartState::isInATrick() const {
    return m_bInATrick;
}

bool KartState::isBoostOffroadInvincibility() const {
    return m_bBoostOffroadInvincibility;
}

bool KartState::isTrickRot() const {
    return m_bTrickRot;
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

bool KartState::isUNK2() const {
    return m_bUNK2;
}

bool KartState::isSomethingWallCollision() const {
    return m_bSomethingWallCollision;
}

bool KartState::isAutoDrift() const {
    return m_bAutoDrift;
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

s16 KartState::trickableTimer() const {
    return m_trickableTimer;
}

void KartState::clearBitfield0() {
    m_bAccelerate = false;
    m_bBrake = false;
    m_bDriftInput = false;
    m_bDriftManual = false;
    m_bHopStart = false;
    m_bAccelerateStart = false;
    m_bGroundStart = false;
    m_bVehicleBodyFloorCollision = false;
    m_bAnyWheelCollision = false;
    m_bAllWheelsCollision = false;
    m_bStickLeft = false;
    m_bAirtimeOver20 = false;
    m_bStickyRoad = false;
    m_bTouchingGround = false;
    m_bHop = false;
    m_bBoost = false;
    m_bStickRight = false;
    m_bMushroomBoost = false;
    m_bDriftAuto = false;
    m_bSlipdriftCharge = false;
    m_bWheelie = false;
    m_bJumpPad = false;
    m_bRampBoost = false;
}

void KartState::clearBitfield1() {
    m_bTrickStart = false;
    m_bInATrick = false;
    m_bBoostOffroadInvincibility = false;
    m_bTrickRot = false;
    m_bTrickable = false;
}

void KartState::clearBitfield3() {
    m_bUNK2 = false;
    m_bSomethingWallCollision = false;
    m_bSoftWallDrift = false;
    m_bChargeStartBoost = false;
}

void KartState::setAccelerate(bool isSet) {
    m_bAccelerate = isSet;
}

void KartState::setDriftManual(bool isSet) {
    m_bDriftManual = isSet;
}

void KartState::setVehicleBodyFloorCollision(bool isSet) {
    m_bVehicleBodyFloorCollision = isSet;
}

void KartState::setAnyWheelCollision(bool isSet) {
    m_bAnyWheelCollision = isSet;
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

void KartState::setTrickStart(bool isSet) {
    m_bTrickStart = isSet;
}

void KartState::setInATrick(bool isSet) {
    m_bInATrick = isSet;
}

void KartState::setBoostOffroadInvincibility(bool isSet) {
    m_bBoostOffroadInvincibility = isSet;
}

void KartState::setTrickRot(bool isSet) {
    m_bTrickRot = isSet;
}

void KartState::setTrickable(bool isSet) {
    m_bTrickable = isSet;
}

void KartState::setWheelieRot(bool isSet) {
    m_bWheelieRot = isSet;
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

void KartState::setBoostRampType(s32 val) {
    m_boostRampType = val;
}

void KartState::setJumpPadVariant(s32 val) {
    m_jumpPadVariant = val;
}

void KartState::setTrickableTimer(s16 val) {
    m_trickableTimer = val;
}

} // namespace Kart
