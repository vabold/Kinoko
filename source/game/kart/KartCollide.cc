#include "KartCollide.hh"

#include "game/kart/KartMove.hh"
#include "game/kart/KartPhysics.hh"

#include <game/field/CollisionDirector.hh>

namespace Kart {

KartCollide::KartCollide() : m_notTrickable(false) {}

KartCollide::~KartCollide() = default;

void KartCollide::init() {
    m_50 = 0.0f;
    m_offRoad = false;
    m_groundBoostPanelOrRamp = false;
    m_notTrickable = false;
}

void KartCollide::resetHitboxes() {
    CollisionGroup *hitboxGroup = physics()->hitboxGroup();
    for (u16 idx = 0; idx < hitboxGroup->hitboxCount(); ++idx) {
        hitboxGroup->hitbox(idx).setLastPos(scale(), pose());
    }
}

void KartCollide::updateHitboxes() {
    CollisionGroup *hitboxGroup = physics()->hitboxGroup();
    for (u16 idx = 0; idx < hitboxGroup->hitboxCount(); ++idx) {
        hitboxGroup->hitbox(idx).update(move()->totalScale(), 0.0f, scale(), fullRot(), pos());
    }
}

void KartCollide::findCollision() {
    calcBodyCollision(move()->totalScale(), fullRot(), scale());
}

void KartCollide::calcBodyCollision(f32 totalScale, const EGG::Quatf &rot,
        const EGG::Vector3f &scale) {
    CollisionGroup *hitboxGroup = physics()->hitboxGroup();
    CollisionData &collisionData = hitboxGroup->collisionData();
    collisionData.reset();

    EGG::Vector3f posRel;
    Field::CourseColMgr::CollisionInfo colInfo;
    colInfo.bbox.setDirect(EGG::Vector3f::zero, EGG::Vector3f::zero);
    Field::KCLTypeMask maskOut;
    EGG::BoundBox3f minMax = colInfo.bbox;

    for (u16 hitboxIdx = 0; hitboxIdx < hitboxGroup->hitboxCount(); ++hitboxIdx) {
        Field::KCLTypeMask flags = 0xEAFABDFF;
        Hitbox &hitbox = hitboxGroup->hitbox(hitboxIdx);

        if (hitbox.bspHitbox()->wallsOnly != 0) {
            flags = 0x4A109000;
        }

        hitbox.update(totalScale, 0.0f, scale, rot, pos());

        Field::CollisionDirector::Instance()->checkSphereCachedFullPush(hitbox.worldPos(),
                hitbox.lastPos(), flags, &colInfo, &maskOut, hitbox.radius(), 0);
    }

    collisionData.speedFactor = 1.0f;
    collisionData.rotFactor = 1.0f;
}

void KartCollide::calcFloorEffect() {
    m_offRoad = true;
    m_groundBoostPanelOrRamp = true;

    Field::KCLTypeMask mask = KCL_NONE;
    calcTriggers(&mask, pos(), false);

    mask = KCL_NONE;
    calcTriggers(&mask, pos(), true);
}

void KartCollide::calcTriggers(Field::KCLTypeMask *mask, const EGG::Vector3f &pos, bool twoPoint) {
    EGG::Vector3f v1 = twoPoint ? physics()->pos() : EGG::Vector3f::inf;
    Field::KCLTypeMask typeMask = twoPoint ? 0x05070000 : 0xE0F8BDFF;
    f32 fVar1 = twoPoint ? 80.0f : 100.0f;
    f32 scalar = -bsp().initialYPos * move()->totalScale() * 0.3f;
    EGG::Vector3f scaledPos = pos + scalar * componentYAxis();
    EGG::Vector3f zRot = dynamics()->mainRot().rotateVector(EGG::Vector3f::ez);

    m_50 += (zRot.dot(move()->smoothedUp()) - m_50) * 0.3f;

    scalar = m_50 * -physics()->fc() * 1.8f * move()->totalScale();
    scaledPos += scalar * zRot;

    if (!twoPoint) {
        fVar1 *= move()->totalScale();
    }

    bool collide = Field::CollisionDirector::Instance()->checkSphereCachedPartialPush(scaledPos, v1,
            typeMask, nullptr, mask, fVar1, 0);

    if (!collide || twoPoint) {
        return;
    }

    if ((*mask & KCL_TYPE_FLOOR) != 0) {
        Field::CollisionDirector::Instance()->findClosestCollisionEntry(mask, KCL_TYPE_FLOOR);
    }
}

void KartCollide::calcWheelCollision(u16 /*wheelIdx*/, CollisionGroup *hitboxGroup,
        const EGG::Vector3f &_48, const EGG::Vector3f &center, f32 radius) {
    Hitbox &firstHitbox = hitboxGroup->hitbox(0);
    BSP::Hitbox *bspHitbox = const_cast<BSP::Hitbox *>(firstHitbox.bspHitbox());
    bspHitbox->radius = radius;
    hitboxGroup->resetCollision();
    firstHitbox.setWorldPos(center);

    Field::CourseColMgr::CollisionInfo colInfo;
    colInfo.bbox.min = EGG::Vector3f::zero;
    colInfo.bbox.max = EGG::Vector3f::zero;
    Field::KCLTypeMask kclOut;

    bool collided = Field::CollisionDirector::Instance()->checkSphereCachedFullPush(
            firstHitbox.worldPos(), firstHitbox.lastPos(), KCL_TYPE_IDK, &colInfo, &kclOut,
            firstHitbox.radius(), 0);

    CollisionData &collisionData = hitboxGroup->collisionData();

    if (!collided) {
        collisionData.speedFactor = 1.0f;
        collisionData.rotFactor = 1.0f;
        return;
    }

    collisionData.tangentOff = colInfo.tangentOff;

    if ((kclOut & KCL_TYPE_FLOOR) != 0) {
        collisionData.floor = true;
        collisionData.floorNrm = colInfo.floorNrm;
    }

    collisionData.relPos = firstHitbox.worldPos() - pos();
    collisionData.vel = _48;

    processWheel(collisionData, firstHitbox, &colInfo, &kclOut);

    if ((kclOut & KCL_TYPE_IDK) == 0) {
        return;
    }

    Field::CollisionDirector::Instance()->findClosestCollisionEntry(&kclOut, KCL_TYPE_IDK);
}

void KartCollide::processWheel(CollisionData &collisionData, Hitbox &hitbox,
        Field::CourseColMgr::CollisionInfo *colInfo, Field::KCLTypeMask *maskOut) {
    processFloor(collisionData, hitbox, colInfo, maskOut, true);
}

void KartCollide::processFloor(CollisionData &collisionData, Hitbox & /*hitbox*/,
        Field::CourseColMgr::CollisionInfo * /*colInfo*/, Field::KCLTypeMask *maskOut,
        bool /*wheel*/) {
    if ((*maskOut & KCL_TYPE_IDK) == 0) {
        return;
    }

    if (!Field::CollisionDirector::Instance()->findClosestCollisionEntry(maskOut, KCL_TYPE_FLOOR)) {
        return;
    }

    const Field::CollisionDirector::CollisionEntry *closestColEntry =
            Field::CollisionDirector::Instance()->closestCollisionEntry();

    if ((closestColEntry->attribute & 0x2000) == 0) {
        m_notTrickable = true;
    }

    collisionData.intensity = (closestColEntry->attribute >> 0xb) & 3;
    collisionData.closestFloorFlags = closestColEntry->typeMask;
    collisionData.closestFloorSettings = (closestColEntry->attribute >> 5) & 7;

    if ((*maskOut & KCL_TYPE_BIT(COL_TYPE_BOOST_RAMP)) == 0) {
        m_notTrickable = true;
    }
}

void KartCollide::applySomeFloorMoment(f32 down, f32 rate, CollisionGroup *hitboxGroup,
        const EGG::Vector3f &forward, const EGG::Vector3f &nextDir, const EGG::Vector3f &speed,
        bool b1, bool b2, bool b3) {
    CollisionData &colData = hitboxGroup->collisionData();
    if (!colData.floor) {
        return;
    }

    f32 velDotFloorNrm = colData.vel.dot(colData.floorNrm);

    if (velDotFloorNrm >= 0.0f) {
        return;
    }

    EGG::Matrix34f rotMat;
    rotMat.makeQ(dynamics()->mainRot());
    EGG::Matrix34f tmp = rotMat.multiplyTo(dynamics()->invInertiaTensor());
    tmp = tmp.multiplyTo(rotMat);

    EGG::Vector3f crossVec = colData.relPos.cross(colData.floorNrm);
    crossVec = tmp.multVector(crossVec);
    crossVec = crossVec.cross(colData.relPos);

    f32 idk = -velDotFloorNrm / (1.0f + colData.floorNrm.dot(crossVec));
    EGG::Vector3f negSpeed = speed * -1;
    crossVec = colData.floorNrm.cross(negSpeed);
    crossVec = crossVec.cross(colData.floorNrm);

    if (FLT_EPSILON >= crossVec.dot()) {
        return;
    }

    crossVec.normalise();
    f32 speedDot = std::min(0.0f, speed.dot(crossVec));
    crossVec *= ((idk * speedDot) / velDotFloorNrm);

    auto projAndRej = crossVec.projAndRej(forward);

    f32 projNorm = projAndRej.first.length();
    f32 rejNorm = projAndRej.second.length();
    f32 projNorm_ = projNorm;
    f32 rejNorm_ = rejNorm;

    f32 dVar7 = down * abs(idk);
    if (dVar7 < abs(projNorm)) {
        projNorm_ = dVar7;
        if (projNorm < 0.0f) {
            projNorm_ = -down * abs(idk);
        }
    }

    f32 dVar5 = rate * abs(idk);
    if (dVar5 < abs(rejNorm)) {
        rejNorm_ = dVar5;
        if (rejNorm < 0.0f) {
            rejNorm = -rate * abs(idk);
        }
    }

    projAndRej.first.normalise();
    projAndRej.second.normalise();

    projAndRej.first *= projNorm_;
    projAndRej.second *= rejNorm_;

    EGG::Vector3f projRejSum = projAndRej.first + projAndRej.second;
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

} // namespace Kart
