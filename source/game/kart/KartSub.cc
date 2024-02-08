#include "KartSub.hh"

#include "game/kart/KartBody.hh"
#include "game/kart/KartMove.hh"
#include "game/kart/KartState.hh"

namespace Kart {

KartSub::KartSub() = default;

void KartSub::createSubsystems(bool isBike) {
    m_move = isBike ? new KartMoveBike : new KartMove;
    m_state = new KartState;
}

void KartSub::copyPointers(KartAccessor &pointers) {
    pointers.m_move = m_move;
    pointers.m_state = m_state;
}

void KartSub::init() {
    resetPhysics();
    body()->reset();
    m_state->init();
}

void KartSub::initPhysicsValues() {
    physics()->updatePose();
}

void KartSub::resetPhysics() {
    physics()->reset();
    physics()->updatePose();
    m_move->setKartSpeedLimit();
}

void KartSub::calcPass0() {
    physics()->setPos(dynamics()->pos());
    physics()->setVelocity(dynamics()->velocity());
    dynamics()->setGravity(-1.3f);

    move()->calc();

    // Pertains to startslides / leaning in stage 0 and 1
    EGG::Vector3f killExtVel = dynamics()->extVel();
    if (isBike()) {
        killExtVel = killExtVel.rej(move()->smoothedUp());
    } else {
        killExtVel.x = 0.0f;
        killExtVel.z = 0.0f;
    }

    dynamics()->setExtVel(killExtVel);

    f32 maxSpeed = move()->hardSpeedLimit();
    physics()->calc(DT, maxSpeed, scale(), !state()->isGround());
}

} // namespace Kart
