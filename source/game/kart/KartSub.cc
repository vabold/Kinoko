#include "KartSub.hh"

#include "game/kart/KartBody.hh"
#include "game/kart/KartCollide.hh"
#include "game/kart/KartMove.hh"
#include "game/kart/KartState.hh"
#include "game/kart/KartSuspensionPhysics.hh"

#include "game/field/CollisionDirector.hh"

namespace Kart {

KartSub::KartSub() = default;

void KartSub::createSubsystems(bool isBike) {
    m_move = isBike ? new KartMoveBike : new KartMove;
    m_state = new KartState;
    m_collide = new KartCollide;
}

void KartSub::copyPointers(KartAccessor &pointers) {
    pointers.collide = m_collide;
    pointers.state = m_state;
    pointers.move = m_move;
}

void KartSub::init() {
    resetPhysics();
    body()->reset();
    m_state->init();
    move()->setTurnParams();
    m_collide->init();
}

void KartSub::initPhysicsValues() {
    physics()->updatePose();
    collide()->resetHitboxes();
}

void KartSub::resetPhysics() {
    physics()->reset();
    physics()->updatePose();
    collide()->resetHitboxes();

    for (u16 wheelIdx = 0; wheelIdx < suspCount(); ++wheelIdx) {
        suspensionPhysics(wheelIdx)->reset();
    }
    for (u16 tireIdx = 0; tireIdx < tireCount(); ++tireIdx) {
        tirePhysics(tireIdx)->reset();
    }
    m_move->setKartSpeedLimit();
    m_someScale = 1.0f;
    m_maxSuspOvertravel.setZero();
    m_minSuspOvertravel.setZero();
}

void KartSub::calcPass0() {
    state()->calc();
    physics()->setPos(dynamics()->pos());
    physics()->setVelocity(dynamics()->velocity());
    dynamics()->setGravity(-1.3f);
    dynamics()->setAngVel0YFactor(0.9f);

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

    collide()->calcHitboxes();
    collisionGroup()->setHitboxScale(move()->totalScale());
}

void KartSub::calcPass1() {
    m_floorCollisionCount = 0;
    m_maxSuspOvertravel.setZero();
    m_minSuspOvertravel.setZero();

    Field::CollisionDirector::Instance()->checkCourseColNarrScLocal(250.0f, pos(),
            KCL_TYPE_VEHICLE_INTERACTABLE, 0);
    collide()->findCollision();
    collide()->calcFloorEffect();

    EGG::Vector3f forward = fullRot().rotateVector(EGG::Vector3f::ez);
    m_someScale = scale().y;

    const EGG::Vector3f gravity(0.0f, -1.3f, 0.0f);
    for (u16 i = 0; i < suspCount(); ++i) {
        const EGG::Matrix34f wheelMatrix = body()->wheelMatrix(i);
        suspensionPhysics(i)->calcCollision(DT, gravity, wheelMatrix);
    }

    EGG::Vector3f vehicleCompensation = m_maxSuspOvertravel + m_minSuspOvertravel;
    dynamics()->setPos(dynamics()->pos() + vehicleCompensation);
    if (!physics()->hitboxGroup()->collisionData().floor) {
        EGG::Vector3f relPos;
        EGG::Vector3f vel;
        EGG::Vector3f floorNrm;

        for (u16 wheelIdx = 0; wheelIdx < suspCount(); ++wheelIdx) {
            suspensionPhysics(wheelIdx)->calcSuspension(forward, vehicleCompensation);
        }

        move()->setFloorCollisionCount(m_floorCollisionCount);

        physics()->updatePose();

        collide()->resetHitboxes();
    }
}

f32 KartSub::someScale() {
    return m_someScale;
}

} // namespace Kart
