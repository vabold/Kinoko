#include "KartState.hh"

#include "game/kart/CollisionGroup.hh"

#include <game/system/RaceManager.hh>

namespace Kart {

KartState::KartState() {
    m_ground = false;
    m_chargeStartBoost = false;
    m_wheelie = false;
    m_wheelieRot = false;
}

void KartState::init() {
    reset();
}

void KartState::reset() {
    m_ground = false;
    m_chargeStartBoost = false;

    m_top.setZero();
    m_startBoostCharge = 0.0f;
    m_stickX = 0.0f;
}

void KartState::calcInput() {
    if (System::RaceManager::Instance()->stage() != System::RaceManager::Stage::Countdown) {
        return;
    }

    const System::RaceInputState &currentState = inputs()->currentState();
    m_stickX = currentState.stick.x;
    m_chargeStartBoost = currentState.accelerate();

    calcStartBoost();
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
        m_ground = true;
    }
}

void KartState::calcStartBoost() {
    constexpr f32 START_BOOST_DELTA_ONE = 0.02f;
    constexpr f32 START_BOOST_DELTA_TWO = 0.002f;
    constexpr f32 START_BOOST_FALLOFF = 0.96f;

    if (m_chargeStartBoost) {
        m_startBoostCharge += START_BOOST_DELTA_ONE -
                (START_BOOST_DELTA_ONE - START_BOOST_DELTA_TWO) * m_startBoostCharge;
    } else {
        m_startBoostCharge *= START_BOOST_FALLOFF;
    }

    m_startBoostCharge = std::max(0.0f, std::min(1.0f, m_startBoostCharge));
}

bool KartState::isGround() const {
    return m_ground;
}

bool KartState::isChargeStartBoost() const {
    return m_chargeStartBoost;
}

bool KartState::isWheelie() const {
    return m_wheelie;
}

bool KartState::isWheelieRot() const {
    return m_wheelieRot;
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

void KartState::setWheelie(bool isSet) {
    m_wheelie = isSet;
}

void KartState::setWheelieRot(bool isSet) {
    m_wheelieRot = isSet;
}

} // namespace Kart
