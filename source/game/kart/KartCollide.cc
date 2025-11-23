#include "KartCollide.hh"

#include "game/kart/KartBody.hh"
#include "game/kart/KartMove.hh"
#include "game/kart/KartPhysics.hh"
#include "game/kart/KartState.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/ObjectCollisionKart.hh"
#include "game/field/ObjectDirector.hh"

#include <egg/math/BoundBox.hh>
#include <egg/math/Math.hh>

namespace Kart {

/// @addr{0x8056E56C}
KartCollide::KartCollide() {
    m_boundingRadius = 100.0f;
    m_surfaceFlags.makeAllZero();
}

/// @addr{0x80573FF0}
KartCollide::~KartCollide() = default;

/// @addr{0x8056E624}
void KartCollide::init() {
    calcBoundingRadius();
    m_floorMomentRate = 0.8f;
    m_surfaceFlags.makeAllZero();
    m_respawnTimer = 0;
    m_solidOobTimer = 0;
    m_smoothedBack = 0.0f;
    m_suspBottomHeightNonSoftWall = 0.0f;
    m_suspBottomHeightSoftWall = 0.0f;
    m_someNonSoftWallTimer = 0;
    m_someSoftWallTimer = 0;
    m_poleAngVelTimer = 0;
    m_poleYaw = 0.0f;
    m_colPerpendicularity = 0.0f;
}

/// @addr{0x805730D4}
void KartCollide::resetHitboxes() {
    CollisionGroup *hitboxGroup = physics()->hitboxGroup();
    for (u16 idx = 0; idx < hitboxGroup->hitboxCount(); ++idx) {
        hitboxGroup->hitbox(idx).setLastPos(scale(), pose());
    }
}

/// @stage All
/// @brief On each frame, calculates the positions for each hitbox
/// @addr{0x8056EE24}
void KartCollide::calcHitboxes() {
    CollisionGroup *hitboxGroup = physics()->hitboxGroup();
    for (u16 idx = 0; idx < hitboxGroup->hitboxCount(); ++idx) {
        hitboxGroup->hitbox(idx).calc(move()->totalScale(), body()->sinkDepth(), scale(), fullRot(),
                pos());
    }
}

/// @stage All
/// @addr{0x80572C20}
void KartCollide::findCollision() {
    bool wasHalfPipe = state()->isEndHalfPipe() || state()->isActionMidZipper();
    const EGG::Quatf &rot = wasHalfPipe ? mainRot() : fullRot();
    calcBodyCollision(move()->totalScale(), body()->sinkDepth(), rot, scale());

    auto &colData = collisionData();
    bool existingWallCollision = colData.bWall || colData.bWall3;
    bool newWallCollision =
            m_surfaceFlags.onBit(eSurfaceFlags::ObjectWall, eSurfaceFlags::ObjectWall3);
    if (existingWallCollision || newWallCollision) {
        if (!existingWallCollision) {
            colData.wallNrm = m_totalReactionWallNrm;
            if (m_surfaceFlags.onBit(eSurfaceFlags::ObjectWall)) {
                colData.bWall = true;
            } else if (m_surfaceFlags.onBit(eSurfaceFlags::ObjectWall3)) {
                colData.bWall3 = true;
            }
        } else if (newWallCollision) {
            colData.wallNrm += m_totalReactionWallNrm;
            if (m_surfaceFlags.onBit(eSurfaceFlags::ObjectWall)) {
                colData.bWall = true;
            } else if (m_surfaceFlags.onBit(eSurfaceFlags::ObjectWall3)) {
                colData.bWall3 = true;
            }
        }

        colData.wallNrm.normalise();
    }

    FUN_80572F4C();
}

/// @stage 2
/// @addr{0x80572F4C}
/// @rename
void KartCollide::FUN_80572F4C() {
    f32 fVar1;

    if (isInRespawn() || state()->isBoost() || state()->isOverZipper() ||
            state()->isZipperInvisibleWall() || state()->isNoSparkInvisibleWall() ||
            state()->isHalfPipeRamp()) {
        fVar1 = 0.0f;
    } else {
        fVar1 = 0.05f;
    }

    bool resetXZ = fVar1 > 0.0f && state()->isAirtimeOver20() && dynamics()->velocity().y < -50.0f;

    FUN_805B72B8(state()->isInAction() ? 0.3f : 0.01f, fVar1, resetXZ,
            !state()->isJumpPadDisableYsusForce());
}

/// @stage All
/// @brief Affects velocity when landing from airtime.
/// @addr{0x805B72B8}
/// @details Every frame, this function checks the player's velocity relative to the floor collision
/// and applies external velocity and angular velocity.
/// This is mostly relevant when you were midair and just landed.
/// @rename
void KartCollide::FUN_805B72B8(f32 param_1, f32 param_2, bool lockXZ, bool addExtVelY) {
    const auto &colData = collisionData();

    if (!colData.bFloor && !colData.bWall && !colData.bWall3) {
        return;
    }

    EGG::Vector3f collisionDir = colData.floorNrm + colData.wallNrm;
    collisionDir.normalise();

    f32 directionalVelocity = colData.vel.dot(collisionDir);
    if (directionalVelocity >= 0.0f) {
        return;
    }

    EGG::Matrix34f rotMat;
    EGG::Matrix34f rotMatTrans;

    rotMat.makeQ(dynamics()->mainRot());
    rotMatTrans = rotMat.transpose();
    rotMat = rotMat.multiplyTo(dynamics()->invInertiaTensor()).multiplyTo(rotMatTrans);

    EGG::Vector3f relPos = colData.relPos;
    if (lockXZ) {
        relPos.x = 0.0f;
        relPos.z = 0.0f;
    }

    EGG::Vector3f step1 = relPos.cross(collisionDir);
    EGG::Vector3f step2 = rotMat.multVector33(step1);
    EGG::Vector3f step3 = step2.cross(relPos);
    f32 val = (-directionalVelocity * (1.0f + param_2)) / (1.0f + collisionDir.dot(step3));
    EGG::Vector3f step4 = collisionDir.cross(-colData.vel);
    EGG::Vector3f step5 = step4.cross(collisionDir);
    step5.normalise();

    f32 fVar1 = param_1 * EGG::Mathf::abs(val);
    f32 otherVal = (val * colData.vel.dot(step5)) / directionalVelocity;

    f32 fVar3 = otherVal;
    if (fVar1 < EGG::Mathf::abs(otherVal)) {
        fVar3 = fVar1;
        if (otherVal < 0.0f) {
            fVar3 = -param_1 * EGG::Mathf::abs(val);
        }
    }

    EGG::Vector3f step6 = val * collisionDir + fVar3 * step5;

    f32 local_1d0 = step6.y;
    if (!addExtVelY) {
        local_1d0 = 0.0f;
    } else if (colData.bFloor) {
        f32 velY = intVel().y;
        if (velY > 0.0f) {
            velY += extVel().y;
            if (velY < 0.0f) {
                EGG::Vector3f newExtVel = extVel();
                newExtVel.y = velY;
                dynamics()->setExtVel(newExtVel);
            }
        }
    }

    f32 prevExtVelY = extVel().y;
    EGG::Vector3f extVelAdd = step6;
    extVelAdd.y = local_1d0;
    dynamics()->setExtVel(extVel() + extVelAdd);

    if (prevExtVelY < 0.0f && extVel().y > 0.0f && extVel().y < 10.0f) {
        EGG::Vector3f extVelNoY = extVel();
        extVelNoY.y = 0.0f;
        dynamics()->setExtVel(extVelNoY);
    }

    EGG::Vector3f step7 = relPos.cross(step6);
    EGG::Vector3f step8 = rotMat.multVector33(step7);
    EGG::Vector3f step9 = mainRot().rotateVectorInv(step8);
    step9.y = 0.0f;
    dynamics()->setAngVel0(dynamics()->angVel0() + step9);
}

/// @stage All
/// @addr{0x805B6724}
/// @brief Checks and acts on collision for each kart hitbox.
/// @param totalScale
/// @param rot
/// @param scale
void KartCollide::calcBodyCollision(f32 totalScale, f32 sinkDepth, const EGG::Quatf &rot,
        const EGG::Vector3f &scale) {
    CollisionGroup *hitboxGroup = physics()->hitboxGroup();
    CollisionData &collisionData = hitboxGroup->collisionData();
    collisionData.reset();

    EGG::Vector3f posRel = EGG::Vector3f::zero;
    s32 count = 0;
    Field::CollisionInfo colInfo;
    colInfo.bbox.setDirect(EGG::Vector3f::zero, EGG::Vector3f::zero);
    Field::KCLTypeMask maskOut;
    Field::CourseColMgr::NoBounceWallColInfo noBounceWallInfo;
    EGG::BoundBox3f minMax;
    minMax.setZero();
    bool bVar1 = false;

    for (u16 hitboxIdx = 0; hitboxIdx < hitboxGroup->hitboxCount(); ++hitboxIdx) {
        Field::KCLTypeMask flags = KCL_TYPE_DRIVER_SOLID_SURFACE;
        Hitbox &hitbox = hitboxGroup->hitbox(hitboxIdx);

        if (hitbox.bspHitbox()->wallsOnly != 0) {
            flags = 0x4A109000;
            Field::CourseColMgr::Instance()->setNoBounceWallInfo(&noBounceWallInfo);
        }

        hitbox.calc(totalScale, sinkDepth, scale, rot, pos());

        if (Field::CollisionDirector::Instance()->checkSphereCachedFullPush(hitbox.radius(),
                    hitbox.worldPos(), hitbox.lastPos(), flags, &colInfo, &maskOut, 0)) {
            if (!!(maskOut & KCL_TYPE_VEHICLE_COLLIDEABLE)) {
                Field::CollisionDirector::Instance()->findClosestCollisionEntry(&maskOut,
                        KCL_TYPE_VEHICLE_COLLIDEABLE);
            }

            if (!FUN_805B6A9C(collisionData, hitbox, minMax, posRel, count, maskOut, colInfo)) {
                bVar1 = true;

                if (colInfo.movingFloorDist > -std::numeric_limits<f32>::min()) {
                    collisionData.bHasRoadVel = true;
                    collisionData.roadVelocity = colInfo.roadVelocity;
                }

                processBody(collisionData, hitbox, &colInfo, &maskOut);
            }
        }
    }

    if (bVar1) {
        EGG::Vector3f movement = minMax.min + minMax.max;
        applyBodyCollision(collisionData, movement, posRel, count);
    } else {
        collisionData.speedFactor = 1.0f;
        collisionData.rotFactor = 1.0f;
    }
}

/// @addr{0x80571634}
void KartCollide::calcFloorEffect() {
    if (state()->isTouchingGround()) {
        m_surfaceFlags.setBit(eSurfaceFlags::Offroad, eSurfaceFlags::GroundBoostPanelOrRamp);
    }

    m_suspBottomHeightNonSoftWall = 0.0f;
    m_surfaceFlags.resetBit(eSurfaceFlags::Wall, eSurfaceFlags::SolidOOB, eSurfaceFlags::BoostRamp,
            eSurfaceFlags::Offroad, eSurfaceFlags::Trickable, eSurfaceFlags::NotTrickable,
            eSurfaceFlags::StopHalfPipeState);
    m_suspBottomHeightSoftWall = 0.0f;
    m_someNonSoftWallTimer = 0;
    m_someSoftWallTimer = 0;

    Field::KCLTypeMask mask = KCL_NONE;
    calcTriggers(&mask, pos(), false);

    auto *colDir = Field::CollisionDirector::Instance();

    if (m_solidOobTimer >= 3 && m_surfaceFlags.onBit(eSurfaceFlags::SolidOOB) &&
            m_surfaceFlags.offBit(eSurfaceFlags::Wall)) {
        if (mask & KCL_TYPE_BIT(COL_TYPE_SOLID_OOB)) {
            colDir->findClosestCollisionEntry(&mask, KCL_TYPE_BIT(COL_TYPE_SOLID_OOB));
        }

        activateOob(true, &mask, false, false);
    }

    mask = KCL_NONE;
    calcTriggers(&mask, pos(), true);

    m_solidOobTimer =
            m_surfaceFlags.onBit(eSurfaceFlags::SolidOOB) ? std::min(3, m_solidOobTimer + 1) : 0;

    if (state()->isWall3Collision() || state()->isWallCollision()) {
        Field::KCLTypeMask maskOut = KCL_NONE;

        if (colDir->checkSphereCachedPartialPush(m_boundingRadius, pos(), EGG::Vector3f::inf,
                    KCL_TYPE_BIT(COL_TYPE_FALL_BOUNDARY), nullptr, &maskOut, 0)) {
            calcFallBoundary(&maskOut, true);
        }
    }
}

/// @addr{0x805718D4}
void KartCollide::calcTriggers(Field::KCLTypeMask *mask, const EGG::Vector3f &pos, bool twoPoint) {
    EGG::Vector3f v1 = twoPoint ? physics()->pos() : EGG::Vector3f::inf;
    Field::KCLTypeMask typeMask = twoPoint ? KCL_TYPE_DIRECTIONAL : KCL_TYPE_NON_DIRECTIONAL;
    f32 radius = twoPoint ? 80.0f : 100.0f * move()->totalScale();
    f32 scalar = -bsp().initialYPos * move()->totalScale() * 0.3f;
    EGG::Vector3f scaledPos = pos + scalar * componentYAxis();
    EGG::Vector3f back = dynamics()->mainRot().rotateVector(EGG::Vector3f::ez);

    m_smoothedBack += (back.dot(move()->smoothedUp()) - m_smoothedBack) * 0.3f;

    scalar = m_smoothedBack * -physics()->fc() * 1.8f * move()->totalScale();
    scaledPos += scalar * back;

    bool collide = Field::CollisionDirector::Instance()->checkSphereCachedPartialPush(radius,
            scaledPos, v1, typeMask, nullptr, mask, 0);

    if (!collide) {
        return;
    }

    if (twoPoint) {
        handleTriggers(mask);
    } else {
        if (*mask & KCL_TYPE_FLOOR) {
            Field::CollisionDirector::Instance()->findClosestCollisionEntry(mask, KCL_TYPE_FLOOR);
        }

        if (*mask & KCL_TYPE_WALL) {
            m_surfaceFlags.setBit(eSurfaceFlags::Wall);
        }

        if (*mask & KCL_TYPE_BIT(COL_TYPE_SOLID_OOB)) {
            m_surfaceFlags.setBit(eSurfaceFlags::SolidOOB);
        }
    }
}

/// @addr{0x8056F510}
void KartCollide::handleTriggers(Field::KCLTypeMask *mask) {
    calcFallBoundary(mask, false);
    processCannon(mask);

    if (*mask & KCL_TYPE_BIT(COL_TYPE_EFFECT_TRIGGER)) {
        auto *colDir = Field::CollisionDirector::Instance();
        if (colDir->findClosestCollisionEntry(mask, KCL_TYPE_BIT(COL_TYPE_EFFECT_TRIGGER))) {
            if (colDir->closestCollisionEntry()->variant() == 4) {
                halfPipe()->end(true);
                state()->setEndHalfPipe(true);
                m_surfaceFlags.setBit(eSurfaceFlags::StopHalfPipeState);
            }
        }
    }
}

/// @addr{0x80571D98}
void KartCollide::calcFallBoundary(Field::KCLTypeMask *mask, bool shortBoundary) {
    if (!(*mask & KCL_TYPE_BIT(COL_TYPE_FALL_BOUNDARY))) {
        return;
    }

    auto *colDir = Field::CollisionDirector::Instance();
    if (!colDir->findClosestCollisionEntry(mask, KCL_TYPE_BIT(COL_TYPE_FALL_BOUNDARY))) {
        return;
    }

    bool safe = false;
    const auto *entry = colDir->closestCollisionEntry();

    if (shortBoundary) {
        if (entry->variant() != 7) {
            safe = true;
        }
    }

    if (!safe) {
        activateOob(false, mask, false, false);
    }
}

/// @addr{0x80573ED4}
void KartCollide::calcBeforeRespawn() {
    if (pos().y < 0.0f) {
        activateOob(true, nullptr, false, false);
    }

    if (!state()->isBeforeRespawn()) {
        return;
    }

    if (--m_respawnTimer > 0) {
        return;
    }

    state()->setBeforeRespawn(false);
    m_respawnTimer = 0;
    move()->triggerRespawn();
}

/// @addr{0x80573B00}
void KartCollide::activateOob(bool /*detachCamera*/, Field::KCLTypeMask * /*mask*/,
        bool /*somethingCPU*/, bool /*somethingBullet*/) {
    constexpr s16 RESPAWN_TIME = 130;

    if (state()->isBeforeRespawn()) {
        return;
    }

    move()->initOob();

    m_respawnTimer = RESPAWN_TIME;
    state()->setBeforeRespawn(true);
}

/// @stage All
/// @brief Checks wheel hitbox collision and stores position/velocity info.
/// @addr{0x805B6F4C}
/// @param hitboxGroup The wheel's collision information
/// @param colVel The wheel's velocity. In the base game, it is always \f$\vec{v} = \begin{bmatrix}
/// 0 \\ -13 \\ 0 \end{bmatrix}\f$
/// @param center The wheel's position
/// @param radius The wheel's size
void KartCollide::calcWheelCollision(u16 /*wheelIdx*/, CollisionGroup *hitboxGroup,
        const EGG::Vector3f &colVel, const EGG::Vector3f &center, f32 radius) {
    Hitbox &firstHitbox = hitboxGroup->hitbox(0);
    BSP::Hitbox *bspHitbox = const_cast<BSP::Hitbox *>(firstHitbox.bspHitbox());
    bspHitbox->radius = radius;
    hitboxGroup->resetCollision();
    firstHitbox.setWorldPos(center);

    Field::CollisionInfo colInfo;
    colInfo.bbox.setZero();
    Field::KCLTypeMask kclOut;
    Field::CourseColMgr::NoBounceWallColInfo noBounceWallInfo;
    Field::CourseColMgr::Instance()->setNoBounceWallInfo(&noBounceWallInfo);

    bool collided = Field::CollisionDirector::Instance()->checkSphereCachedFullPush(
            firstHitbox.radius(), firstHitbox.worldPos(), firstHitbox.lastPos(),
            KCL_TYPE_VEHICLE_COLLIDEABLE, &colInfo, &kclOut, 0);

    CollisionData &collisionData = hitboxGroup->collisionData();

    if (!collided) {
        collisionData.speedFactor = 1.0f;
        collisionData.rotFactor = 1.0f;
        return;
    }

    collisionData.tangentOff = colInfo.tangentOff;

    if (noBounceWallInfo.dist > std::numeric_limits<f32>::min()) {
        collisionData.tangentOff += noBounceWallInfo.tangentOff;
        collisionData.noBounceWallNrm = noBounceWallInfo.fnrm;
        collisionData.bSoftWall = true;
    }

    if (kclOut & KCL_TYPE_FLOOR) {
        collisionData.bFloor = true;
        collisionData.floorNrm = colInfo.floorNrm;
    }

    collisionData.relPos = firstHitbox.worldPos() - pos();
    collisionData.vel = colVel;

    if (colInfo.movingFloorDist > -std::numeric_limits<f32>::min()) {
        collisionData.bHasRoadVel = true;
        collisionData.roadVelocity = colInfo.roadVelocity;
    }

    processWheel(collisionData, firstHitbox, &colInfo, &kclOut);

    if (!(kclOut & KCL_TYPE_VEHICLE_COLLIDEABLE)) {
        return;
    }

    Field::CollisionDirector::Instance()->findClosestCollisionEntry(&kclOut,
            KCL_TYPE_VEHICLE_COLLIDEABLE);
}

/// @stage 2
/// @addr{0x8056F26C}
void KartCollide::calcSideCollision(CollisionData &collisionData, Hitbox &hitbox,
        Field::CollisionInfo *colInfo) {
    if (colInfo->perpendicularity <= 0.0f) {
        return;
    }

    m_colPerpendicularity = std::max(m_colPerpendicularity, colInfo->perpendicularity);

    if (collisionData.bWallAtLeftCloser || collisionData.bWallAtRightCloser) {
        return;
    }

    f32 bspPosX = hitbox.bspHitbox()->position.x;
    if (EGG::Mathf::abs(bspPosX) > 10.0f) {
        if (bspPosX > 0.0f) {
            collisionData.bWallAtLeftCloser = true;
        } else {
            collisionData.bWallAtRightCloser = true;
        }

        collisionData.colPerpendicularity = colInfo->perpendicularity;

        return;
    }

    EGG::Vector3f right = dynamics()->mainRot().rotateVector(EGG::Vector3f::ex);
    std::array<f32, 2> tangents = {0.0f, 0.0f};

    // The loop is just to do left/right wall
    for (size_t i = 0; i < tangents.size(); ++i) {
        f32 sign = i == 1 ? -1.0f : 1.0f;
        f32 effectiveRadius = sign * hitbox.radius();
        EGG::Vector3f effectivePos = hitbox.worldPos() + effectiveRadius * right;
        Field::CollisionInfoPartial tempColInfo;

        if (Field::CollisionDirector::Instance()->checkSphereCachedPartial(hitbox.radius(),
                    effectivePos, hitbox.lastPos(), KCL_TYPE_DRIVER_WALL, &tempColInfo, nullptr,
                    0)) {
            tangents[i] = colInfo->tangentOff.squaredLength();
        }
    }

    if (tangents[0] > tangents[1]) {
        collisionData.bWallAtLeftCloser = true;
        collisionData.colPerpendicularity = colInfo->perpendicularity;
    } else if (tangents[1] > tangents[0]) {
        collisionData.bWallAtRightCloser = true;
        collisionData.colPerpendicularity = colInfo->perpendicularity;
    }
}

/// @addr{0x8056E70C}
void KartCollide::calcBoundingRadius() {
    m_boundingRadius = collisionGroup()->boundingRadius() * move()->hitboxScale();
}

/// @addr{0x80571F10}
void KartCollide::calcObjectCollision() {
    constexpr f32 COS_PI_OVER_4 = 0.707f;
    constexpr s32 DUMMY_POLE_ANG_VEL_TIME = 3;
    constexpr f32 DUMMY_POLE_ANG_VEL = 0.005f;

    m_totalReactionWallNrm = EGG::Vector3f::zero;
    m_surfaceFlags.resetBit(eSurfaceFlags::ObjectWall, eSurfaceFlags::ObjectWall3);

    size_t collisionCount = objectCollisionKart()->checkCollision(pose(), velocity());

    const auto *objectDirector = Field::ObjectDirector::Instance();

    for (size_t i = 0; i < collisionCount; ++i) {
        Reaction reaction = objectDirector->reaction(i);
        if (reaction != Reaction::None && reaction != Reaction::UNK_7) {
            size_t handlerIdx = static_cast<std::underlying_type_t<Reaction>>(reaction);
            Action newAction = (this->*s_objectCollisionHandlers[handlerIdx])(i);
            if (newAction != Action::None) {
                action()->setHitDepth(objectDirector->hitDepth(i));
                action()->start(newAction);
            }

            if (reaction != Reaction::SmallBump && reaction != Reaction::BigBump) {
                const EGG::Vector3f &hitDepth = objectDirector->hitDepth(i);
                m_tangentOff += hitDepth; // hitDepth wrong 2615
                m_movement += hitDepth;
            }
        }

        if (objectDirector->collidingObject(i)->id() == Field::ObjectId::DummyPole) {
            EGG::Vector3f hitDirection = objectCollisionKart()->GetHitDirection(i);
            const EGG::Vector3f &lastDir = move()->lastDir();

            if (lastDir.dot(hitDirection) < -COS_PI_OVER_4) {
                EGG::Vector3f angVel = hitDirection.cross(lastDir);
                f32 sign = angVel.y > 0.0f ? -1.0f : 1.0f;

                m_poleAngVelTimer = DUMMY_POLE_ANG_VEL_TIME;
                m_poleYaw = DUMMY_POLE_ANG_VEL * sign;
            }
        }
    }

    calcPoleTimer();
}

/// @addr{Inlined in 0x80571F10}
void KartCollide::calcPoleTimer() {
    if (m_poleAngVelTimer > 0 && (state()->isAccelerate() || state()->isBrake())) {
        EGG::Vector3f angVel2 = dynamics()->angVel2();
        angVel2.y += m_poleYaw;
        dynamics()->setAngVel2(angVel2);
    }

    m_poleAngVelTimer = std::max(0, m_poleAngVelTimer - 1);
}

/// @stage All
/// @brief Processes moving water and floor collision effects
/// @addr{0x8056E8D4}
void KartCollide::processWheel(CollisionData &collisionData, Hitbox &hitbox,
        Field::CollisionInfo *colInfo, Field::KCLTypeMask *maskOut) {
    processFloor(collisionData, hitbox, colInfo, maskOut, true);
}

/// @addr{0x8056E764}
void KartCollide::processBody(CollisionData &collisionData, Hitbox &hitbox,
        Field::CollisionInfo *colInfo, Field::KCLTypeMask *maskOut) {
    bool hasWallCollision = processWall(collisionData, maskOut);

    processFloor(collisionData, hitbox, colInfo, maskOut, false);

    if (hasWallCollision) {
        calcSideCollision(collisionData, hitbox, colInfo);
    }

    processCannon(maskOut);
}

/// @addr{0x8056F184}
bool KartCollide::processWall(CollisionData &collisionData, Field::KCLTypeMask *maskOut) {
    if (!(*maskOut & KCL_TYPE_DRIVER_WALL_NO_INVISIBLE_WALL2)) {
        return false;
    }

    auto *colDirector = Field::CollisionDirector::Instance();
    if (!colDirector->findClosestCollisionEntry(maskOut, KCL_TYPE_DRIVER_WALL_NO_INVISIBLE_WALL2)) {
        return false;
    }

    if ((*maskOut & KCL_TYPE_DRIVER_WALL_NO_INVISIBLE_WALL) &&
            colDirector->findClosestCollisionEntry(maskOut,
                    KCL_TYPE_DRIVER_WALL_NO_INVISIBLE_WALL)) {
        auto *entry = colDirector->closestCollisionEntry();

        collisionData.closestWallFlags = entry->baseType();
        collisionData.closestWallSettings = entry->variant();

        if (entry->attribute.onBit(Field::CollisionDirector::eCollisionAttribute::Soft)) {
            collisionData.bSoftWall = true;
        }
    }

    return true;
}

/// @stage All
/// @addr{0x8056EA04}
/// @brief Processes the floor triangles' attributes.
/// @param collisionData Stores the resulting speed and handling info
/// @param maskOut Stores the flags from the floor KCL
/// @param wheel Differentiates between body and wheel floor collision (boost panels)
void KartCollide::processFloor(CollisionData &collisionData, Hitbox &hitbox,
        Field::CollisionInfo * /*colInfo*/, Field::KCLTypeMask *maskOut, bool wheel) {
    constexpr Field::KCLTypeMask BOOST_RAMP_MASK = KCL_TYPE_BIT(COL_TYPE_BOOST_RAMP);

    if (collisionData.bSoftWall) {
        ++m_someSoftWallTimer;
        m_suspBottomHeightSoftWall += hitbox.worldPos().y - hitbox.radius();
    }

    if (!(*maskOut & KCL_TYPE_FLOOR)) {
        return;
    }

    auto *colDirector = Field::CollisionDirector::Instance();

    if (!colDirector->findClosestCollisionEntry(maskOut, KCL_TYPE_FLOOR)) {
        return;
    }

    const auto *closestColEntry = colDirector->closestCollisionEntry();

    if (closestColEntry->attribute.offBit(
                Field::CollisionDirector::eCollisionAttribute::Trickable)) {
        m_surfaceFlags.setBit(eSurfaceFlags::NotTrickable);
    } else {
        collisionData.bTrickable = true;
        m_surfaceFlags.setBit(eSurfaceFlags::Trickable);
    }

    collisionData.speedFactor = std::min(collisionData.speedFactor,
            param()->stats().kclSpeed[closestColEntry->baseType()]);

    collisionData.intensity = closestColEntry->intensity();
    collisionData.rotFactor += param()->stats().kclRot[closestColEntry->baseType()];

    if (closestColEntry->attribute.onBit(
                Field::CollisionDirector::eCollisionAttribute::RejectRoad)) {
        state()->setRejectRoad(true);
    }

    collisionData.closestFloorFlags = closestColEntry->typeMask;
    collisionData.closestFloorSettings = closestColEntry->variant();

    if (wheel && !!(*maskOut & KCL_TYPE_BIT(COL_TYPE_BOOST_PAD))) {
        move()->padType().setBit(KartMove::ePadType::BoostPanel);
    }

    if (!!(*maskOut & BOOST_RAMP_MASK) &&
            colDirector->findClosestCollisionEntry(maskOut, BOOST_RAMP_MASK)) {
        closestColEntry = colDirector->closestCollisionEntry();
        move()->padType().setBit(KartMove::ePadType::BoostRamp);
        state()->setBoostRampType(closestColEntry->variant());
        m_surfaceFlags.setBit(eSurfaceFlags::BoostRamp, eSurfaceFlags::Trickable);
    } else {
        state()->setBoostRampType(-1);
        m_surfaceFlags.setBit(eSurfaceFlags::NotTrickable);
    }

    if (!collisionData.bSoftWall) {
        ++m_someNonSoftWallTimer;
        m_suspBottomHeightNonSoftWall += hitbox.worldPos().y - hitbox.radius();
    }

    if (*maskOut & KCL_TYPE_BIT(COL_TYPE_STICKY_ROAD)) {
        state()->setStickyRoad(true);
    }

    Field::KCLTypeMask halfPipeRampMask = KCL_TYPE_BIT(COL_TYPE_HALFPIPE_RAMP);
    if ((*maskOut & halfPipeRampMask) &&
            colDirector->findClosestCollisionEntry(maskOut, halfPipeRampMask)) {
        state()->setHalfPipeRamp(true);
        state()->setHalfPipeInvisibilityTimer(2);
        if (colDirector->closestCollisionEntry()->variant() == 1) {
            move()->padType().setBit(KartMove::ePadType::BoostPanel);
        }
    }

    Field::KCLTypeMask jumpPadMask = KCL_TYPE_BIT(COL_TYPE_JUMP_PAD);
    if (*maskOut & jumpPadMask && colDirector->findClosestCollisionEntry(maskOut, jumpPadMask)) {
        if ((!state()->isTouchingGround() || !state()->isJumpPad()) &&
                !state()->isJumpPadMushroomVelYInc()) {
            move()->padType().setBit(KartMove::ePadType::JumpPad);
            closestColEntry = colDirector->closestCollisionEntry();
            state()->setJumpPadVariant(closestColEntry->variant());
        }
        collisionData.bTrickable = true;
    }
}

/// @addr{0x8056F490}
/// @brief Checks if we are colliding with a cannon trigger and sets the state flag if so.
void KartCollide::processCannon(Field::KCLTypeMask *maskOut) {
    auto *colDirector = Field::CollisionDirector::Instance();
    if (colDirector->findClosestCollisionEntry(maskOut, KCL_TYPE_BIT(COL_TYPE_CANNON_TRIGGER))) {
        state()->setCannonPointId(colDirector->closestCollisionEntry()->variant());
        state()->setCannonStart(true);
    }
}

/// @stage All
/// @brief Applies external and angular velocity based on the collision with the floor
/// @addr{0x805B7928}
/// @param down Always 0.1f
/// @param rate Downward velocity? Related to suspension stiffness
/// @param hitboxGroup Used to retrieve CollisionData reference
/// @param forward Current world facing direction of the kart
/// @param nextDir Updated facing direction of the kart
/// @param speed Tire speed
void KartCollide::applySomeFloorMoment(f32 down, f32 rate, CollisionGroup *hitboxGroup,
        const EGG::Vector3f &forward, const EGG::Vector3f &nextDir, const EGG::Vector3f &speed,
        bool b1, bool b2, bool b3) {
    CollisionData &colData = hitboxGroup->collisionData();
    if (!colData.bFloor) {
        return;
    }

    f32 velDotFloorNrm = colData.vel.dot(colData.floorNrm);

    if (velDotFloorNrm >= 0.0f) {
        return;
    }

    EGG::Matrix34f rotMat;
    rotMat.makeQ(dynamics()->mainRot());
    EGG::Matrix34f tmp = rotMat.multiplyTo(dynamics()->invInertiaTensor());
    EGG::Matrix34f rotMatTrans = rotMat.transpose();
    tmp = tmp.multiplyTo(rotMatTrans);

    EGG::Vector3f crossVec = colData.relPos.cross(colData.floorNrm);
    crossVec = tmp.multVector(crossVec);
    crossVec = crossVec.cross(colData.relPos);

    f32 scalar = -velDotFloorNrm / (1.0f + colData.floorNrm.dot(crossVec));
    EGG::Vector3f negSpeed = -speed;
    crossVec = colData.floorNrm.cross(negSpeed);
    crossVec = crossVec.cross(colData.floorNrm);

    if (std::numeric_limits<f32>::epsilon() >= crossVec.squaredLength()) {
        return;
    }

    crossVec.normalise();
    f32 speedDot = std::min(0.0f, speed.dot(crossVec));
    crossVec *= ((scalar * speedDot) / velDotFloorNrm);

    auto [proj, rej] = crossVec.projAndRej(forward);

    f32 projNorm = proj.length();
    f32 rejNorm = rej.length();
    f32 projNorm_ = projNorm;
    f32 rejNorm_ = rejNorm;

    f32 dVar7 = down * EGG::Mathf::abs(scalar);
    if (dVar7 < EGG::Mathf::abs(projNorm)) {
        projNorm_ = dVar7;
        if (projNorm < 0.0f) {
            projNorm_ = -down * EGG::Mathf::abs(scalar);
        }
    }

    f32 dVar5 = rate * EGG::Mathf::abs(scalar);
    if (dVar5 < EGG::Mathf::abs(rejNorm)) {
        rejNorm_ = dVar5;
        if (rejNorm < 0.0f) {
            rejNorm_ = -rate * EGG::Mathf::abs(scalar);
        }
    }

    proj.normalise();
    rej.normalise();

    proj *= projNorm_;
    rej *= rejNorm_;

    EGG::Vector3f projRejSum = proj + rej;
    EGG::Vector3f projRejSumOrig = projRejSum;

    if (!b1) {
        projRejSum.x = 0.0f;
        projRejSum.z = 0.0f;
    }
    if (!b2) {
        projRejSum.y = 0.0f;
    }

    projRejSum = projRejSum.rej(nextDir);

    dynamics()->setExtVel(dynamics()->extVel() + projRejSum);

    if (b3) {
        EGG::Vector3f rotation = colData.relPos.cross(projRejSumOrig);
        EGG::Vector3f rotation2 = dynamics()->mainRot().rotateVectorInv(tmp.multVector(rotation));

        EGG::Vector3f angVel = rotation2;
        angVel.y = 0.0f;
        if (!b1) {
            angVel.x = 0.0f;
        }
        dynamics()->setAngVel0(dynamics()->angVel0() + angVel);
    }
}

/// @stage All
/// @brief Called on collision of a new KCL type??? This only happens after airtime so far.
/// @addr{0x805B6A9C}
/// @rename
bool KartCollide::FUN_805B6A9C(CollisionData &collisionData, const Hitbox &hitbox,
        EGG::BoundBox3f &minMax, EGG::Vector3f &relPos, s32 &count,
        const Field::KCLTypeMask &maskOut, const Field::CollisionInfo &colInfo) {
    if (maskOut & KCL_TYPE_WALL) {
        if (!(maskOut & KCL_TYPE_FLOOR) && state()->isHWG() &&
                state()->softWallSpeed().dot(colInfo.wallNrm) < 0.3f) {
            return true;
        }

        bool skipWalls = false;

        collisionData.wallNrm += colInfo.wallNrm;

        if (maskOut & KCL_TYPE_ANY_INVISIBLE_WALL) {
            collisionData.bInvisibleWall = true;

            if (!(maskOut & KCL_TYPE_4010D000)) {
                collisionData.bInvisibleWallOnly = true;

                if (maskOut & KCL_TYPE_BIT(COL_TYPE_HALFPIPE_INVISIBLE_WALL)) {
                    skipWalls = true;
                }
            }
        }

        if (!skipWalls) {
            if (maskOut & KCL_TYPE_BIT(COL_TYPE_WALL_2)) {
                collisionData.bWall3 = true;
            } else {
                collisionData.bWall = true;
            }
        }
    }

    if (maskOut & KCL_TYPE_FLOOR) {
        collisionData.floorNrm += colInfo.floorNrm;
        collisionData.bFloor = true;
    }

    EGG::Vector3f tangentOff = colInfo.tangentOff;
    minMax.min = minMax.min.minimize(tangentOff);
    minMax.max = minMax.max.maximize(tangentOff);
    tangentOff.normalise();

    relPos += hitbox.relPos();
    relPos += -hitbox.radius() * tangentOff;
    ++count;

    return false;
}

/// @stage 2
/// @brief Saves collision info when vehicle body collision occurs.
/// @details Additionally may apply a change in position for certain KCL types.
/// @addr{0x805B6D48}
void KartCollide::applyBodyCollision(CollisionData &collisionData, const EGG::Vector3f &movement,
        const EGG::Vector3f &posRel, s32 count) {
    setPos(pos() + movement);

    if (!collisionData.bFloor && (collisionData.bWall || collisionData.bWall3)) {
        collisionData.movement = movement;
    }

    f32 rotFactor = 1.0f / static_cast<f32>(count);
    EGG::Vector3f scaledRelPos = rotFactor * posRel;
    collisionData.rotFactor *= rotFactor;

    EGG::Vector3f scaledAngVel0 = dynamics()->angVel0Factor() * dynamics()->angVel0();
    EGG::Vector3f local_48 = mainRot().rotateVectorInv(scaledRelPos);
    EGG::Vector3f local_30 = scaledAngVel0.cross(local_48);
    local_30 = mainRot().rotateVector(local_30);
    local_30 += extVel();

    collisionData.vel = local_30;
    collisionData.relPos = scaledRelPos;

    if (collisionData.bFloor) {
        f32 intVelY = dynamics()->intVel().y;
        if (intVelY > 0.0f) {
            collisionData.vel.y += intVelY;
        }
        collisionData.floorNrm.normalise();
    }
}

/// @addr{0x805713D8}
void KartCollide::startFloorMomentRate() {
    m_floorMomentRate = 0.01f;
}

/// @addr{0x805713FC}
void KartCollide::calcFloorMomentRate() {
    if (state()->isInAction() && action()->flags().onBit(KartAction::eFlags::Rotating)) {
        m_floorMomentRate = 0.01f;
    } else {
        m_floorMomentRate = std::min(m_floorMomentRate + 0.01f, 0.8f);
    }
}

/// @addr{0x8056E564}
Action KartCollide::handleReactNone(size_t /*idx*/) {
    return Action::None;
}

/// @addr{0x8057363C}
Action KartCollide::handleReactWallAllSpeed(size_t idx) {
    m_totalReactionWallNrm += Field::ObjectCollisionKart::GetHitDirection(idx);
    m_surfaceFlags.setBit(eSurfaceFlags::ObjectWall);

    return Action::None;
}

/// @addr{0x805733CC}
Action KartCollide::handleReactSpinAllSpeed(size_t /*idx*/) {
    return Action::UNK_0;
}

/// @addr{0x805733D4}
Action KartCollide::handleReactSpinSomeSpeed(size_t /*idx*/) {
    return Action::UNK_1;
}

/// @addr{0x805735AC}
Action KartCollide::handleReactFireSpin(size_t /*idx*/) {
    return Action::UNK_9;
}

/// @addr{0x805733C4}
Action KartCollide::handleReactSmallLaunch(size_t /*idx*/) {
    return Action::UNK_2;
}

/// @addr{0x805733DC}
Action KartCollide::handleReactKnockbackSomeSpeedLoseItem(size_t /*idx*/) {
    return Action::UNK_3;
}

/// @addr{0x8057353C}
Action KartCollide::handleReactLaunchSpinLoseItem(size_t /*idx*/) {
    return Action::UNK_6;
}

/// @addr{0x805733EC}
Action KartCollide::handleReactKnockbackBumpLoseItem(size_t /*idx*/) {
    return Action::UNK_4;
}

/// @addr{0x805735B4}
Action KartCollide::handleReactLongCrushLoseItem(size_t /*idx*/) {
    return Action::UNK_12;
}

/// @addr{0x805737B8}
Action KartCollide::handleReactSmallBump(size_t idx) {
    move()->applyBumpForce(30.0f, objectCollisionKart()->GetHitDirection(idx), false);
    return Action::None;
}

/// @addr{0x805733E4}
Action KartCollide::handleReactHighLaunchLoseItem(size_t /*idx*/) {
    return Action::UNK_8;
}

/// @addr{0x80573754}
Action KartCollide::handleReactWeakWall(size_t /*idx*/) {
    move()->setSpeed(move()->speed() * 0.82f);
    return Action::None;
}

/// @addr{0x80573790}
Action KartCollide::handleReactOffroad(size_t /*idx*/) {
    state()->setCollidingOffroad(true);
    m_surfaceFlags.setBit(eSurfaceFlags::Offroad);
    return Action::None;
}

/// @addr{0x805733F4}
Action KartCollide::handleReactLaunchSpin(size_t idx) {
    action()->setTranslation(objectCollisionKart()->translation(idx));
    return Action::UNK_5;
}

/// @addr{0x805736C8}
Action KartCollide::handleReactWallSpark(size_t idx) {
    m_totalReactionWallNrm += Field::ObjectCollisionKart::GetHitDirection(idx);
    m_surfaceFlags.setBit(eSurfaceFlags::ObjectWall3);

    return Action::None;
}

/// @addr{0x805735EC}
Action KartCollide::handleReactUntrickableJumpPad(size_t /*idx*/) {
    move()->setPadType(KartMove::PadType(KartMove::ePadType::JumpPad));
    state()->setJumpPadVariant(0);

    return Action::None;
}

/// @addr{0x805735D4}
Action KartCollide::handleReactShortCrushLoseItem(size_t /*idx*/) {
    return Action::UNK_14;
}

/// @addr{0x805735DC}
Action KartCollide::handleReactCrushRespawn(size_t /*idx*/) {
    return Action::UNK_16;
}

/// @addr{0x805735E4}
Action KartCollide::handleReactExplosionLoseItem(size_t /*idx*/) {
    return Action::UNK_7;
}

std::array<KartCollide::ObjectCollisionHandler, 33> KartCollide::s_objectCollisionHandlers = {{
        &KartCollide::handleReactNone,
        &KartCollide::handleReactNone,
        &KartCollide::handleReactNone,
        &KartCollide::handleReactNone,
        &KartCollide::handleReactNone,
        &KartCollide::handleReactNone,
        &KartCollide::handleReactNone,
        &KartCollide::handleReactNone,
        &KartCollide::handleReactWallAllSpeed,
        &KartCollide::handleReactSpinAllSpeed,
        &KartCollide::handleReactSpinSomeSpeed,
        &KartCollide::handleReactFireSpin,
        &KartCollide::handleReactNone,
        &KartCollide::handleReactSmallLaunch,
        &KartCollide::handleReactKnockbackSomeSpeedLoseItem,
        &KartCollide::handleReactLaunchSpinLoseItem,
        &KartCollide::handleReactKnockbackBumpLoseItem,
        &KartCollide::handleReactLongCrushLoseItem,
        &KartCollide::handleReactSmallBump,
        &KartCollide::handleReactNone,
        &KartCollide::handleReactNone,
        &KartCollide::handleReactHighLaunchLoseItem,
        &KartCollide::handleReactNone,
        &KartCollide::handleReactWeakWall,
        &KartCollide::handleReactOffroad,
        &KartCollide::handleReactLaunchSpin,
        &KartCollide::handleReactWallSpark,
        &KartCollide::handleReactNone,
        &KartCollide::handleReactNone,
        &KartCollide::handleReactUntrickableJumpPad,
        &KartCollide::handleReactShortCrushLoseItem,
        &KartCollide::handleReactCrushRespawn,
        &KartCollide::handleReactExplosionLoseItem,
}};

} // namespace Kart
