#include "KartCollide.hh"

#include "game/kart/KartMove.hh"
#include "game/kart/KartPhysics.hh"
#include "game/kart/KartState.hh"

#include "game/field/CollisionDirector.hh"

#include <egg/math/Math.hh>

namespace Kart {

/// @addr{0x8056E56C}
KartCollide::KartCollide() {
    m_rampBoost = false;
    m_offRoad = false;
    m_groundBoostPanelOrRamp = false;
    m_notTrickable = false;
}

/// @addr{0x80573FF0}
KartCollide::~KartCollide() = default;

/// @addr{0x8056E624}
void KartCollide::init() {
    m_rampBoost = false;
    m_offRoad = false;
    m_groundBoostPanelOrRamp = false;
    m_trickable = false;
    m_notTrickable = false;
    m_smoothedBack = 0.0f;
    m_suspBottomHeightNonSoftWall = 0.0f;
    m_suspBottomHeightSoftWall = 0.0f;
    m_someNonSoftWallTimer = 0;
    m_someSoftWallTimer = 0;
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
        hitboxGroup->hitbox(idx).calc(move()->totalScale(), 0.0f, scale(), fullRot(), pos());
    }
}

/// @stage All
/// @addr{0x80572C20}
void KartCollide::findCollision() {
    calcBodyCollision(move()->totalScale(), fullRot(), scale());

    auto &colData = collisionData();
    if (colData.bWall) {
        colData.wallNrm.normalise();
    }

    f32 fVar1;
    if (state()->isBoost()) {
        fVar1 = 0.0f;
    } else {
        fVar1 = 0.05f;
    }

    f32 dVar14 = fVar1;
    fVar1 = 0.01f;
    bool resetXZ = dVar14 > 0.0f && state()->isAirtimeOver20() && dynamics()->velocity().y < -50.0f;

    FUN_805B72B8(fVar1, dVar14, resetXZ, true);
}

/// @stage 2
/// @addr{0x80572F4C}
/// @rename
void KartCollide::FUN_80572F4C() {
    f32 fVar1;

    if (state()->isBoost()) {
        fVar1 = 0.0f;
    } else {
        fVar1 = 0.05f;
    }

    bool resetXZ = fVar1 > 0.0f && state()->isAirtimeOver20() && dynamics()->velocity().y < -50.0f;

    FUN_805B72B8(0.01f, fVar1, resetXZ, !state()->isJumpPadDisableYsusForce());
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

    if (!colData.bFloor && !colData.bWall) {
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
    f32 val = (-directionalVelocity * (param_2 + 1.0f)) / (1.0f + collisionDir.dot(step3));
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
void KartCollide::calcBodyCollision(f32 totalScale, const EGG::Quatf &rot,
        const EGG::Vector3f &scale) {
    CollisionGroup *hitboxGroup = physics()->hitboxGroup();
    CollisionData &collisionData = hitboxGroup->collisionData();
    collisionData.reset();

    EGG::Vector3f posRel;
    s32 count = 0;
    Field::CourseColMgr::CollisionInfo colInfo;
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

        hitbox.calc(totalScale, 0.0f, scale, rot, pos());

        if (Field::CollisionDirector::Instance()->checkSphereCachedFullPush(hitbox.worldPos(),
                    hitbox.lastPos(), flags, &colInfo, &maskOut, hitbox.radius(), 0)) {
            if (!!(maskOut & KCL_TYPE_VEHICLE_COLLIDEABLE)) {
                Field::CollisionDirector::Instance()->findClosestCollisionEntry(&maskOut,
                        KCL_TYPE_VEHICLE_COLLIDEABLE);
            }

            if (!FUN_805B6A9C(collisionData, hitbox, minMax, posRel, count, maskOut, colInfo)) {
                bVar1 = true;
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
        m_offRoad = true;
        m_groundBoostPanelOrRamp = true;
    }

    m_suspBottomHeightNonSoftWall = 0.0f;
    m_rampBoost = false;
    m_offRoad = false;
    m_trickable = false;
    m_notTrickable = false;
    m_suspBottomHeightSoftWall = 0.0f;
    m_someNonSoftWallTimer = 0;
    m_someSoftWallTimer = 0;

    Field::KCLTypeMask mask = KCL_NONE;
    calcTriggers(&mask, pos(), false);

    mask = KCL_NONE;
    calcTriggers(&mask, pos(), true);
}

/// @addr{0x805718D4}
void KartCollide::calcTriggers(Field::KCLTypeMask *mask, const EGG::Vector3f &pos, bool twoPoint) {
    EGG::Vector3f v1 = twoPoint ? physics()->pos() : EGG::Vector3f::inf;
    Field::KCLTypeMask typeMask = twoPoint ? KCL_TYPE_DIRECTIONAL : KCL_TYPE_NON_DIRECTIONAL;
    f32 fVar1 = twoPoint ? 80.0f : 100.0f * move()->totalScale();
    f32 scalar = -bsp().initialYPos * move()->totalScale() * 0.3f;
    EGG::Vector3f scaledPos = pos + scalar * componentYAxis();
    EGG::Vector3f back = dynamics()->mainRot().rotateVector(EGG::Vector3f::ez);

    m_smoothedBack += (back.dot(move()->smoothedUp()) - m_smoothedBack) * 0.3f;

    scalar = m_smoothedBack * -physics()->fc() * 1.8f * move()->totalScale();
    scaledPos += scalar * back;

    bool collide = Field::CollisionDirector::Instance()->checkSphereCachedPartialPush(scaledPos, v1,
            typeMask, nullptr, mask, fVar1, 0);

    if (!collide || twoPoint) {
        return;
    }

    if (*mask & KCL_TYPE_FLOOR) {
        Field::CollisionDirector::Instance()->findClosestCollisionEntry(mask, KCL_TYPE_FLOOR);
    }
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

    Field::CourseColMgr::CollisionInfo colInfo;
    colInfo.bbox.setZero();
    Field::KCLTypeMask kclOut;
    Field::CourseColMgr::NoBounceWallColInfo noBounceWallInfo;
    Field::CourseColMgr::Instance()->setNoBounceWallInfo(&noBounceWallInfo);

    bool collided = Field::CollisionDirector::Instance()->checkSphereCachedFullPush(
            firstHitbox.worldPos(), firstHitbox.lastPos(), KCL_TYPE_VEHICLE_COLLIDEABLE, &colInfo,
            &kclOut, firstHitbox.radius(), 0);

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

    processWheel(collisionData, firstHitbox, &colInfo, &kclOut);

    if (!(kclOut & KCL_TYPE_VEHICLE_COLLIDEABLE)) {
        return;
    }

    Field::CollisionDirector::Instance()->findClosestCollisionEntry(&kclOut,
            KCL_TYPE_VEHICLE_COLLIDEABLE);
}

/// @stage All
/// @brief Processes moving water and floor collision effects
/// @addr{0x8056E8D4}
void KartCollide::processWheel(CollisionData &collisionData, Hitbox &hitbox,
        Field::CourseColMgr::CollisionInfo *colInfo, Field::KCLTypeMask *maskOut) {
    processFloor(collisionData, hitbox, colInfo, maskOut, true);
}

/// @addr{0x8056E764}
void KartCollide::processBody(CollisionData &collisionData, Hitbox &hitbox,
        Field::CourseColMgr::CollisionInfo *colInfo, Field::KCLTypeMask *maskOut) {
    processFloor(collisionData, hitbox, colInfo, maskOut, false);
}

/// @stage All
/// @addr{0x8056EA04}
/// @brief Processes the floor triangles' attributes.
/// @param collisionData Stores the resulting speed and handling info
/// @param maskOut Stores the flags from the floor KCL
/// @param wheel Differentiates between body and wheel floor collision (boost panels)
void KartCollide::processFloor(CollisionData &collisionData, Hitbox &hitbox,
        Field::CourseColMgr::CollisionInfo * /*colInfo*/, Field::KCLTypeMask *maskOut, bool wheel) {
    constexpr Field::KCLTypeMask BOOST_RAMP_MASK = KCL_TYPE_BIT(COL_TYPE_BOOST_RAMP);

    if (collisionData.bSoftWall) {
        ++m_someSoftWallTimer;
        m_suspBottomHeightSoftWall += hitbox.worldPos().y - hitbox.radius();
    }

    if (!(*maskOut & KCL_TYPE_VEHICLE_COLLIDEABLE)) {
        return;
    }

    auto *colDirector = Field::CollisionDirector::Instance();

    if (!colDirector->findClosestCollisionEntry(maskOut, KCL_TYPE_FLOOR)) {
        return;
    }

    const auto *closestColEntry = colDirector->closestCollisionEntry();

    u16 attribute = closestColEntry->attribute;
    if (!(attribute & 0x2000)) {
        m_notTrickable = true;
    } else {
        collisionData.bTrickable = true;
        m_trickable = true;
    }

    collisionData.speedFactor = std::min(collisionData.speedFactor,
            param()->stats().kclSpeed[KCL_ATTRIBUTE_TYPE(attribute)]);

    collisionData.intensity = (attribute >> 0xb) & 3;
    collisionData.rotFactor += param()->stats().kclRot[KCL_ATTRIBUTE_TYPE(attribute)];
    collisionData.closestFloorFlags = closestColEntry->typeMask;
    collisionData.closestFloorSettings = KCL_VARIANT_TYPE(attribute);

    if (wheel && !!(*maskOut & KCL_TYPE_BIT(COL_TYPE_BOOST_PAD))) {
        move()->setPadBoost(true);
    }

    if (!!(*maskOut & BOOST_RAMP_MASK) &&
            colDirector->findClosestCollisionEntry(maskOut, BOOST_RAMP_MASK)) {
        closestColEntry = colDirector->closestCollisionEntry();
        move()->setRampBoost(true);
        state()->setBoostRampType(KCL_VARIANT_TYPE(closestColEntry->attribute));
        m_rampBoost = true;
        m_trickable = true;
    } else {
        state()->setBoostRampType(-1);
        m_notTrickable = true;
    }

    if (!collisionData.bSoftWall) {
        ++m_someNonSoftWallTimer;
        m_suspBottomHeightNonSoftWall += hitbox.worldPos().y - hitbox.radius();
    }

    if (*maskOut & KCL_TYPE_BIT(COL_TYPE_STICKY_ROAD)) {
        state()->setStickyRoad(true);
    }

    Field::KCLTypeMask jumpPadMask = KCL_TYPE_BIT(COL_TYPE_JUMP_PAD);
    if (*maskOut & jumpPadMask && colDirector->findClosestCollisionEntry(maskOut, jumpPadMask)) {
        if (!state()->isTouchingGround() || !state()->isJumpPad()) {
            move()->setPadJump(true);
            closestColEntry = colDirector->closestCollisionEntry();
            state()->setJumpPadVariant(KCL_VARIANT_TYPE(closestColEntry->attribute));
        }
        collisionData.bTrickable = true;
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

    if (FLT_EPSILON >= crossVec.dot()) {
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
        const Field::KCLTypeMask &maskOut, const Field::CourseColMgr::CollisionInfo &colInfo) {
    if (maskOut & KCL_TYPE_WALL) {
        collisionData.wallNrm += colInfo.wallNrm;
        collisionData.bWall = true;
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

    if (!collisionData.bFloor && collisionData.bWall) {
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

/// @addr{0x805B78D0}
void KartCollide::setFloorColInfo(CollisionData &collisionData, const EGG::Vector3f &relPos,
        const EGG::Vector3f &vel, const EGG::Vector3f &floorNrm) {
    collisionData.relPos = relPos;
    collisionData.vel = vel;
    collisionData.floorNrm = floorNrm;
    collisionData.bFloor = true;
}

void KartCollide::setMovement(const EGG::Vector3f &v) {
    m_movement = v;
}

const EGG::Vector3f &KartCollide::movement() const {
    return m_movement;
}

f32 KartCollide::suspBottomHeightSoftWall() const {
    return m_suspBottomHeightSoftWall;
}

u16 KartCollide::someSoftWallTimer() const {
    return m_someSoftWallTimer;
}

f32 KartCollide::suspBottomHeightNonSoftWall() const {
    return m_suspBottomHeightNonSoftWall;
}

u16 KartCollide::someNonSoftWallTimer() const {
    return m_someNonSoftWallTimer;
}

bool KartCollide::isRampBoost() const {
    return m_rampBoost;
}

bool KartCollide::isTrickable() const {
    return m_trickable;
}

bool KartCollide::isNotTrickable() const {
    return m_notTrickable;
}

} // namespace Kart
