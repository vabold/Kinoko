#include "KartState.hh"

#include "game/kart/CollisionGroup.hh"
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
    m_bAccelerate = false;
    m_bBrake = false;
    m_bDriftInput = false;
    m_bHopStart = false;
    m_bAccelerateStart = false;
    m_bStickLeft = false;
    m_bTouchingGround = false;
    m_bStickRight = false;
    m_bWheelie = false;

    m_bWheelieRot = false;

    m_bChargeStartBoost = false;

    m_bAutoDrift = inputs()->driftIsAuto();
}

void KartState::init() {
    reset();
}

void KartState::reset() {
    m_bTouchingGround = false;
    m_bChargeStartBoost = false;

    m_top.setZero();
    m_startBoostCharge = 0.0f;
    m_stickX = 0.0f;
}

void KartState::calcInput() {
    const auto *raceMgr = System::RaceManager::Instance();
    if (raceMgr->isStageReached(System::RaceManager::Stage::Race)) {
        const auto &currentState = inputs()->currentState();
        const auto &lastState = inputs()->lastState();
        m_stickX = currentState.stick.x;
        m_stickY = currentState.stick.y;

        if (m_stickX < 0.0f) {
            m_bStickRight = true;
        } else if (m_stickX > 0.0f) {
            m_bStickLeft = true;
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
    m_stickX = 0.0f;
    calcCollisions();
}

void KartState::calcCollisions() {
    m_top.setZero();

    u16 wheelCollisions = 0;
    for (u16 tireIdx = 0; tireIdx < tireCount(); ++tireIdx) {
        if (hasFloorCollision(tirePhysics(tireIdx))) {
            m_top += collisionData(tireIdx).floorNrm;
            ++wheelCollisions;
        }
    }

    const CollisionData &colData = collisionData();
    if (colData.floor) {
        m_top += colData.floorNrm;
    }

    m_top.normalise();

    if (wheelCollisions < 1 && !colData.floor) {
        // Airtime
        ;
    } else {
        m_bTouchingGround = true;
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

bool KartState::isTouchingGround() const {
    return m_bTouchingGround;
}

bool KartState::isChargeStartBoost() const {
    return m_bChargeStartBoost;
}

bool KartState::isBoost() const {
    return m_bBoost;
}

bool KartState::isWheelie() const {
    return m_bWheelie;
}

bool KartState::isWheelieRot() const {
    return m_bWheelieRot;
}

bool KartState::isAutoDrift() const {
    return m_bAutoDrift;
}

f32 KartState::stickX() const {
    return m_stickX;
}

const EGG::Vector3f &KartState::top() const {
    return m_top;
}

f32 KartState::startBoostCharge() const {
    return m_startBoostCharge;
}

void KartState::setAccelerate(bool isSet) {
    m_bAccelerate = isSet;
}

void KartState::setBoost(bool isSet) {
    m_bBoost = isSet;
}

void KartState::setWheelie(bool isSet) {
    m_bWheelie = isSet;
}

void KartState::setWheelieRot(bool isSet) {
    m_bWheelieRot = isSet;
}

} // namespace Kart
