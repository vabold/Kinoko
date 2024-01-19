#include "KartSub.hh"

#include "game/kart/KartBody.hh"
#include "game/kart/KartMove.hh"

namespace Kart {

KartSub::KartSub() = default;

void KartSub::createSubsystems(bool isBike) {
    m_move = isBike ? new KartMoveBike : new KartMove;
}

void KartSub::copyPointers(KartAccessor &pointers) {
    pointers.m_move = m_move;
}

void KartSub::init() {
    resetPhysics();
    body()->reset();
}

void KartSub::initPhysicsValues() {
    physics()->updatePose();
}

void KartSub::resetPhysics() {
    physics()->reset();
    physics()->updatePose();
}

} // namespace Kart
