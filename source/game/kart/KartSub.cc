#include "KartSub.hh"

#include "game/kart/KartBody.hh"
#include "game/kart/KartCollide.hh"
#include "game/kart/KartMove.hh"
#include "game/kart/KartState.hh"
#include "game/kart/KartSuspensionPhysics.hh"

#include "game/field/CollisionDirector.hh"

#include "game/system/RaceManager.hh"

#include <egg/math/Math.hh>

namespace Kart {

KartSub::KartSub() = default;

/// @addr{0x80598AC8}
KartSub::~KartSub() {
    delete m_collide;
    delete m_state;
    delete m_move;
}

/// @addr{0x80595D48}
void KartSub::createSubsystems(bool isBike) {
    m_move = isBike ? new KartMoveBike : new KartMove;
    m_move->createSubsystems();
    m_state = new KartState;
    m_collide = new KartCollide;
}

/// @brief Called during static construction of KartObject to synchronize the pointers.
/// @addr{0x80596454}
void KartSub::copyPointers(KartAccessor &pointers) {
    pointers.collide = m_collide;
    pointers.state = m_state;
    pointers.move = m_move;
}

/// @addr{0x80595F78}
void KartSub::init() {
    resetPhysics();
    body()->reset();
    m_state->init();
    move()->setTurnParams();
    m_collide->init();
}

/// @addr{0x80597934}
void KartSub::initPhysicsValues() {
    physics()->updatePose();
    collide()->resetHitboxes();
}

/// @addr{0x8059617C}
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
    m_sideCollisionTimer = 0;
    m_someScale = 1.0f;
    m_maxSuspOvertravel.setZero();
    m_minSuspOvertravel.setZero();
}

/// @stage All
/// @brief The first phase of physics computations on each frame.
/// @addr{0x80596480}
/// @details Handles the first-half of physics calculations. This includes input processing,
/// subsequent position/speed updates, as well as responding to last frame's collisions.
void KartSub::calcPass0() {
    if (state()->isCannonStart()) {
        physics()->hitboxGroup()->reset();
        for (size_t i = 0; i < tireCount(); ++i) {
            tirePhysics(i)->hitboxGroup()->reset();
        }
        move()->enterCannon();
    }

    state()->calc();
    physics()->setPos(dynamics()->pos());
    physics()->setVelocity(dynamics()->velocity());
    dynamics()->setGravity(-1.3f);
    dynamics()->setAngVel0YFactor(0.9f);

    state()->calcInput();
    move()->calc();

    if (state()->isSkipWheelCalc()) {
        for (size_t tireIdx = 0; tireIdx < tireCount(); ++tireIdx) {
            tirePhysics(tireIdx)->setLastPos(pos());
        }
        return;
    }

    tryEndHWG();

    dynamics()->setTop(move()->up());

    // Pertains to startslides / leaning in stage 0 and 1
    const auto *raceManager = System::RaceManager::Instance();
    if (!raceManager->isStageReached(System::RaceManager::Stage::Race)) {
        dynamics()->setIntVel(EGG::Vector3f::zero);

        EGG::Vector3f killExtVel = dynamics()->extVel();
        if (isBike()) {
            killExtVel = killExtVel.rej(move()->smoothedUp());
        } else {
            killExtVel.x = 0.0f;
            killExtVel.z = 0.0f;
        }

        dynamics()->setExtVel(killExtVel);
    }

    f32 maxSpeed = move()->hardSpeedLimit();
    physics()->calc(DT, maxSpeed, scale(), !state()->isTouchingGround());

    if (!state()->isInCannon()) {
        collide()->calcHitboxes();
        collisionGroup()->setHitboxScale(move()->totalScale());
    }
}

/// @stage All
/// @brief The second phase of physics computations on each frame.
/// @addr{0x80596CFC}
/// Handles the second-half of physics calculations. This mainly includes
/// collision detection, as well as suspension physics.
void KartSub::calcPass1() {
    constexpr s16 SIDE_COLLISION_TIME = 5;

    m_floorCollisionCount = 0;
    m_maxSuspOvertravel.setZero();
    m_minSuspOvertravel.setZero();

    if (state()->isSomethingWallCollision()) {
        const EGG::Vector3f &softWallSpeed = state()->softWallSpeed();
        f32 speedFactor = 5.0f;
        EGG::Vector3f effectiveSpeed;

        if (state()->isHWG()) {
            speedFactor = 10.0f;
            effectiveSpeed = softWallSpeed;
        } else {
            effectiveSpeed = softWallSpeed.perpInPlane(move()->smoothedUp(), true);
            f32 speedDotUp = softWallSpeed.dot(move()->smoothedUp());
            if (speedDotUp < 0.0f) {
                speedFactor += -speedDotUp * 10.0f;
            }
        }

        effectiveSpeed *= speedFactor * scale().y;
        setPos(pos() + effectiveSpeed);
        collide()->setMovement(collide()->movement() + effectiveSpeed);
    }

    const auto &colData = collisionData();
    if (colData.bWallAtLeftCloser || colData.bWallAtRightCloser || m_sideCollisionTimer > 0) {
        EGG::Vector3f right = dynamics()->mainRot().rotateVector(EGG::Vector3f::ex);

        if (colData.bWallAtLeftCloser || colData.bWallAtRightCloser) {
            f32 sign = colData.bWallAtRightCloser ? 1.0f : -1.0f;
            m_colPerpendicularity = sign * colData.colPerpendicularity;
            m_sideCollisionTimer = SIDE_COLLISION_TIME;
        }

        EGG::Vector3f colPerpBounceDir = 2.0f * m_colPerpendicularity * scale().x * right;
        colPerpBounceDir.y = 0.0f;
        setPos(pos() + colPerpBounceDir);
        collide()->setMovement(collide()->movement() + colPerpBounceDir);
    }

    m_sideCollisionTimer = std::max(m_sideCollisionTimer - 1, 0);

    Field::CollisionDirector::Instance()->checkCourseColNarrScLocal(250.0f, pos(),
            KCL_TYPE_VEHICLE_INTERACTABLE, 0);

    if (!state()->isInCannon()) {
        collide()->findCollision();
        const auto &colData = collisionData();
        if (colData.bWall || colData.bWall3) {
            collide()->setMovement(collide()->movement() + colData.movement);
        }

        collide()->calcFloorEffect();

        if (colData.bFloor) {
            // Update floor count
            addFloor(colData, false);
        }
    }

    EGG::Vector3f forward = fullRot().rotateVector(EGG::Vector3f::ez);
    m_someScale = scale().y;

    const EGG::Vector3f gravity(0.0f, -1.3f, 0.0f);
    f32 speedFactor = 1.0f;
    f32 handlingFactor = 0.0f;
    for (u16 i = 0; i < suspCount(); ++i) {
        const EGG::Matrix34f wheelMatrix = body()->wheelMatrix(i);
        suspensionPhysics(i)->calcCollision(DT, gravity, wheelMatrix);

        const CollisionData &colData = tirePhysics(i)->hitboxGroup()->collisionData();

        speedFactor = std::min(speedFactor, colData.speedFactor);

        if (colData.bFloor) {
            handlingFactor += colData.rotFactor;
            addFloor(colData, false);
        }
    }

    if (!state()->isSkipWheelCalc()) {
        EGG::Vector3f vehicleCompensation = m_maxSuspOvertravel + m_minSuspOvertravel;
        dynamics()->setPos(dynamics()->pos() + vehicleCompensation);

        if (!collisionData().bFloor) {
            EGG::Vector3f relPos;
            EGG::Vector3f vel;
            EGG::Vector3f floorNrm;
            u32 count = 0;

            for (u16 wheelIdx = 0; wheelIdx < tireCount(); ++wheelIdx) {
                const WheelPhysics *wheelPhysics = tirePhysics(wheelIdx);
                if (wheelPhysics->_74() == 0.0f) {
                    continue;
                }

                const CollisionData &colData = wheelPhysics->hitboxGroup()->collisionData();
                relPos += colData.relPos;
                vel += colData.vel;
                floorNrm += colData.floorNrm;
                ++count;
            }

            if (count > 0) {
                f32 scalar = (1.0f / static_cast<f32>(count));
                floorNrm.normalise();

                collide()->setFloorColInfo(collisionData(), relPos * scalar, vel * scalar,
                        floorNrm);

                collide()->FUN_80572F4C();
            }
        }

        for (u16 wheelIdx = 0; wheelIdx < suspCount(); ++wheelIdx) {
            suspensionPhysics(wheelIdx)->calcSuspension(forward, vehicleCompensation);
        }

        move()->calcHopPhysics();
    }

    move()->setKCLWheelSpeedFactor(speedFactor);
    move()->setKCLWheelRotFactor(handlingFactor);

    move()->setFloorCollisionCount(m_floorCollisionCount);

    physics()->updatePose();

    collide()->resetHitboxes();

    // calcRotation() is only ever used for gfx rendering, so skip
}

/// @addr{0x805980D8}
void KartSub::addFloor(const CollisionData &, bool) {
    ++m_floorCollisionCount;
}

/// @addr{0x805979EC}
void KartSub::updateSuspOvertravel(const EGG::Vector3f &suspOvertravel) {
    m_maxSuspOvertravel = m_maxSuspOvertravel.minimize(suspOvertravel);
    m_minSuspOvertravel = m_minSuspOvertravel.maximize(suspOvertravel);
}

/// @addr{0x80598744}
void KartSub::tryEndHWG() {
    if (state()->isSoftWallDrift()) {
        if (EGG::Mathf::abs(move()->speed()) > 15.0f || state()->isAirtimeOver20() ||
                state()->isAllWheelsCollision()) {
            state()->setSoftWallDrift(false);
        } else if (state()->isTouchingGround()) {
            if (componentXAxis().dot(EGG::Vector3f::ey) > 0.8f) {
                state()->setSoftWallDrift(false);
            }
        }
    }

    if (state()->isHWG() && !state()->isSomethingWallCollision()) {
        if (!state()->isWallCollision() || state()->isAllWheelsCollision()) {
            state()->setHWG(false);
        }
    }
}

f32 KartSub::someScale() {
    return m_someScale;
}

} // namespace Kart
