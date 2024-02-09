#include "KartState.hh"

#include "game/kart/CollisionGroup.hh"

namespace Kart {

KartState::KartState() {
    m_ground = false;
}

void KartState::init() {
    reset();
}

void KartState::reset() {
    m_ground = false;
    m_top.setZero();
}

void KartState::calc() {
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

bool KartState::isGround() const {
    return m_ground;
}

const EGG::Vector3f KartState::top() const {
    return m_top;
}

} // namespace Kart
