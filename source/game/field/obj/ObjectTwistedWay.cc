#include "ObjectTwistedWay.hh"

#include "game/field/CollisionDirector.hh"
#include "game/system/RaceManager.hh"     

#include <egg/math/Math.hh>

namespace Field {

/// @addr{0x80813BD4}
ObjectTwistedWay::ObjectTwistedWay(const System::MapdataGeoObj &params) : ObjectDrivable(params) {}

/// @addr{0x80814918}
ObjectTwistedWay::~ObjectTwistedWay() = default;

/// @addr{0x80813c40}
void ObjectTwistedWay::init() {}

/// @addr{0x80813cfc}
void ObjectTwistedWay::calc() {
    if (!System::RaceManager::Instance()->isStageReached(System::RaceManager::Stage::Race)) {
        m_ac++;
    }
}

/// @addr{0x80814910}
u32 ObjectTwistedWay::loadFlags() const {
    return 1;
}

/// @addr{0x8081490c}
void ObjectTwistedWay::createCollision() {}

/// @addr{0x80814908}
void ObjectTwistedWay::calcCollisionTransform() {}

/// @addr{0x808148f8}
f32 ObjectTwistedWay::getCollisionRadius() const {
    return COLLISION_RADIUS;
}

/// @addr{0x808148b8}
bool ObjectTwistedWay::checkPointPartial(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSpherePartialImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x808148c8}
bool ObjectTwistedWay::checkPointPartialPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSpherePartialPushImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x808148d8}
bool ObjectTwistedWay::checkPointFull(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSphereFullImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x808148e8}
bool ObjectTwistedWay::checkPointFullPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSphereFullPushImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x80814958}
bool ObjectTwistedWay::checkSpherePartial(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSpherePartialImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x80814ea8}
bool ObjectTwistedWay::checkSpherePartialPush(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSpherePartialPushImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x80815444}
bool ObjectTwistedWay::checkSphereFull(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSphereFullImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x80815d64}
bool ObjectTwistedWay::checkSphereFullPush(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSphereFullPushImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x807FB54C}
bool ObjectTwistedWay::checkPointCachedPartial(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSpherePartialImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x807FB55C}
bool ObjectTwistedWay::checkPointCachedPartialPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSpherePartialPushImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x807FB56C}
bool ObjectTwistedWay::checkPointCachedFull(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSphereFullImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x807FB57C}
bool ObjectTwistedWay::checkPointCachedFullPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSphereFullPushImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x807FB53C}
bool ObjectTwistedWay::checkSphereCachedPartial(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSpherePartialImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x807FB540}
bool ObjectTwistedWay::checkSphereCachedPartialPush(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSpherePartialPushImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x807FB544}
bool ObjectTwistedWay::checkSphereCachedFull(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSphereFullImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x807FB548}
bool ObjectTwistedWay::checkSphereCachedFullPush(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSphereFullPushImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x807FB6D0}
bool ObjectTwistedWay::checkSpherePartialImpl(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSphereImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset, false, false);
}

/// @addr{0x807FB8B0}
bool ObjectTwistedWay::checkSpherePartialPushImpl(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSphereImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset, false, true);
}

/// @addr{0x807FBAC0}
bool ObjectTwistedWay::checkSphereFullImpl(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSphereImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset, true, false);
}

/// @addr{0x807FBE6C}
bool ObjectTwistedWay::checkSphereFullPushImpl(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSphereImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset, true, true);
}

/// @brief Helper function to calculate the wall's height offset in the wave for the wall collision
/// check
/// @param zAxisProgress how far along the object the sphere is on the z axis
/// @param phase the current phase of the wave
/// @return the height offset for the wall collision check
f32 ObjectTwistedWay::calcWallHeightOffset(f32 zAxisProgress, s32 phase) {
    f32 heightOffset =
            EGG::Mathf::SinFIdx((phase * F_PI / PHASE_COUNT + WAVINESS * zAxisProgress) * RAD2FIDX);
    heightOffset = AMPLITUDE *
            ((zAxisProgress + 1.0f) *
                    ((zAxisProgress + 1.0f) *
                            ((zAxisProgress - 1.0f) * ((zAxisProgress - 1.0f) * heightOffset))));

    return heightOffset;
}

bool ObjectTwistedWay::checkSphereWallCollision(f32 heightOffset, f32 radius, s32 phase,
        const EGG::Vector3f &relativePos, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut, bool full,
        bool push) {
    auto [sinfidx, cosfidx] = EGG::Mathf::SinCosFIdx(heightOffset * RAD2FIDX);
    f32 fVar3 = sinfidx * (relativePos.y - _B8 * 0.5f);
    f32 dist = radius + (fVar3 - cosfidx * (relativePos.x + WIDTH));
    EGG::Vector3f bbox = EGG::Vector3f(dist * cosfidx, dist * sinfidx, 0.0f);

    if (dist <= 0.0f) {
        dist = radius - (fVar3 - cosfidx * (relativePos.x - WIDTH));
        bbox = EGG::Vector3f(-dist * cosfidx, dist * sinfidx, 0.0f);
    }

    if (dist > 0.0f) {
        if (pInfo) {
            pInfo->bbox.min = pInfo->bbox.min.minimize(bbox);
            pInfo->bbox.max = pInfo->bbox.max.maximize(bbox);
        }

        if (pFlagsOut) {
            if (push) {
                CollisionDirector::Instance()->pushCollisionEntry(dist, pFlagsOut,
                        KCL_TYPE_BIT(COL_TYPE_WALL), 0xc);
            } else {
                *pFlagsOut |= KCL_TYPE_BIT(COL_TYPE_WALL);
            }
        }

        return true;
    } else {
        heightOffset = calcWallHeightOffset(0.0f, phase);

        EGG::Vector3f wnrm;

        if (!FUN_80814270(radius, heightOffset, relativePos, bbox, wnrm, dist)) {
            return false;
        }

        if (pInfo) {
            pInfo->bbox.min = pInfo->bbox.min.minimize(bbox);
            pInfo->bbox.max = pInfo->bbox.max.maximize(bbox);

            if (full) {
                pInfo->updateWall(dist, wnrm);
            }
        }

        if (pFlagsOut) {
            if (push) {
                CollisionDirector::Instance()->pushCollisionEntry(dist, pFlagsOut,
                        KCL_TYPE_BIT(COL_TYPE_WALL), 0xc);
            } else {
                *pFlagsOut |= KCL_TYPE_BIT(COL_TYPE_WALL);
            }
        }

        return true;
    }
}

bool ObjectTwistedWay::checkSphereRoadCollision(f32 heightOffset, f32 radius,
        const EGG::Vector3f &relativePos, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut, bool full,
        bool push) {
    auto [sinfidx, cosfidx] = EGG::Mathf::SinCosFIdx(heightOffset * RAD2FIDX);

    f32 scaledB8 = _B8 * -0.5f;
    f32 dist = scaledB8 + radius + -relativePos.x * sinfidx + cosfidx * (-relativePos.y - scaledB8);

    if (dist <= 0.0f) {
        return false;
    }

    EGG::Vector3f bbox = dist * EGG::Vector3f::ey;
    EGG::Vector3f fnrm = EGG::Vector3f(sinfidx, cosfidx, 0.0f);

    if (pInfo) {
        pInfo->bbox.min = pInfo->bbox.min.minimize(bbox);
        pInfo->bbox.max = pInfo->bbox.max.maximize(bbox);

        if (full) {
            pInfo->updateFloor(dist, fnrm);
        }
    }

    if (pFlagsOut) {
        if (push) {
            auto *colDirector = CollisionDirector::Instance();
            colDirector->pushCollisionEntry(dist, pFlagsOut, KCL_TYPE_BIT(COL_TYPE_ROAD), 0x0);
            colDirector->setCurrentCollisionVariant(2);
            if (EGG::Mathf::abs(relativePos.z) < COLLISION_RADIUS * TRICKABLE_RADIUS_FACTOR) {
                colDirector->setCurrentCollisionTrickable(true);
            }
        } else {
            *pFlagsOut |= KCL_TYPE_BIT(COL_TYPE_ROAD);
        }
    }

    return true;
}

/// @brief Helper function which contains frequently re-used code. Behavior branches depending on
/// whether it is a full or partial check (call CollisionInfo::updateFloor) or push (push entry in
/// the CollisionDirector).
/// @param full Whether to update the floor in the CollisionInfo
/// @param push Whether to push a collision entry
bool ObjectTwistedWay::checkSphereImpl(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f & /*v1*/, KCLTypeMask flags, CollisionInfo *pInfo,
        KCLTypeMask *pFlagsOut, u32 timeOffset, bool full, bool push) {
    EGG::Vector3f relativePos = v0 - m_pos;

    if (EGG::Mathf::abs(relativePos.z) > COLLISION_RADIUS ||
            EGG::Mathf::abs(relativePos.x) > WIDTH * 2.0f) {
        return false;
    }

    u32 frameCount;
    if (!System::RaceManager::Instance()->isStageReached(System::RaceManager::Stage::Race)) {
        frameCount = m_ac;
    } else {
        frameCount = System::RaceManager::Instance()->timer();
    }

    frameCount += timeOffset;
    s32 phase = (frameCount % PHASE_COUNT) * 2;

    f32 heightOffset = 0.0f;
    if (System::RaceManager::Instance()->isStageReached(System::RaceManager::Stage::Race)) {
        f32 zAxisProgress = -relativePos.z / COLLISION_RADIUS;
        heightOffset = calcWallHeightOffset(zAxisProgress, phase);
    }

    bool ret = false;
    if (flags & KCL_TYPE_BIT(COL_TYPE_WALL)) {
        ret |= checkSphereWallCollision(heightOffset, radius, phase, relativePos, pInfo, pFlagsOut,
                full, push);
    }

    if (flags & KCL_TYPE_BIT(COL_TYPE_ROAD)) {
        ret |= checkSphereRoadCollision(heightOffset, radius, relativePos, pInfo, pFlagsOut, full,
                push);
    }

    return ret;
}

/// @addr{0x80814270}
bool ObjectTwistedWay::FUN_80814270(f32 radius, f32 heightOffset, const EGG::Vector3f &relativePos,
        EGG::Vector3f &v0, EGG::Vector3f &fnrm, f32 &dist) {
    auto [sinfidx, cosfidx] = EGG::Mathf::SinCosFIdx(heightOffset * RAD2FIDX);
    EGG::Vector3f b8Y = EGG::Vector3f(0.0f, _B8, 0.0f);
    fnrm = relativePos - b8Y;
    fnrm *= fnrm.dot(EGG::Vector3f(sinfidx, cosfidx, 0.0f));
    fnrm = relativePos - (fnrm + b8Y);

    f32 dVar2 = radius + 227.0f - fnrm.length();
    if (dVar2 <= 0.0f) {
        return false;
    }

    (void)fnrm.normalise();
    v0 = fnrm * dVar2;
    dist = dVar2;

    return true;
}

} // namespace Field
