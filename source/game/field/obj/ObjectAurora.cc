#include "ObjectAurora.hh"

#include "game/field/CollisionDirector.hh"

#include "game/system/RaceManager.hh"

#include <egg/math/Math.hh>

namespace Field {

/// @addr{0x807FAB58}
ObjectAurora::ObjectAurora(const System::MapdataGeoObj &params) : ObjectDrivable(params) {}

/// @addr{0x807FB690}
ObjectAurora::~ObjectAurora() = default;

/// @addr{0x807FABC4}
void ObjectAurora::init() {}

/// @addr{0x807FAC84}
void ObjectAurora::calc() {}

/// @addr{0x807FB688}
u32 ObjectAurora::loadFlags() const {
    return 1;
}

/// @addr{0x807FB684}
void ObjectAurora::createCollision() {}

/// @addr{0x807FB680}
void ObjectAurora::calcCollisionTransform() {}

/// @addr{0x807FB5DC}
f32 ObjectAurora::getCollisionRadius() const {
    return COLLISION_RADIUS + 100.0f;
}

/// @addr{0x807FB59C}
bool ObjectAurora::checkPointPartial(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSpherePartialImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x807FB5AC}
bool ObjectAurora::checkPointPartialPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSpherePartialPushImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x807FB5BC}
bool ObjectAurora::checkPointFull(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSphereFullImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x807FB5CC}
bool ObjectAurora::checkPointFullPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSphereFullPushImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x807FB58C}
bool ObjectAurora::checkSpherePartial(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSpherePartialImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x807FB590}
bool ObjectAurora::checkSpherePartialPush(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSpherePartialPushImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x807FB594}
bool ObjectAurora::checkSphereFull(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSphereFullImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x807FB598}
bool ObjectAurora::checkSphereFullPush(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSphereFullPushImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x807FB54C}
bool ObjectAurora::checkPointCachedPartial(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSpherePartialImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x807FB55C}
bool ObjectAurora::checkPointCachedPartialPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSpherePartialPushImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x807FB56C}
bool ObjectAurora::checkPointCachedFull(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSphereFullImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x807FB57C}
bool ObjectAurora::checkPointCachedFullPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSphereFullPushImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x807FB53C}
bool ObjectAurora::checkSphereCachedPartial(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSpherePartialImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x807FB540}
bool ObjectAurora::checkSphereCachedPartialPush(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSpherePartialPushImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x807FB544}
bool ObjectAurora::checkSphereCachedFull(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSphereFullImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x807FB548}
bool ObjectAurora::checkSphereCachedFullPush(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSphereFullPushImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x807FB6D0}
bool ObjectAurora::checkSpherePartialImpl(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSphereImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset, false, false);
}

/// @addr{0x807FB8B0}
bool ObjectAurora::checkSpherePartialPushImpl(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSphereImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset, false, true);
}

/// @addr{0x807FBAC0}
bool ObjectAurora::checkSphereFullImpl(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSphereImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset, true, false);
}

/// @addr{0x807FBE6C}
bool ObjectAurora::checkSphereFullPushImpl(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSphereImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset, true, true);
}

/// @brief Helper function which contains frequently re-used code. Behavior branches depending on
/// whether it is a full or partial check (call CollisionInfo::updateFloor) or push (push entry in
/// the CollisionDirector).
/// @param full Whether to update the floor in the CollisionInfo
/// @param push Whether to push a collision entry
bool ObjectAurora::checkSphereImpl(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f & /*v1*/, KCLTypeMask flags, CollisionInfo *pInfo,
        KCLTypeMask *pFlagsOut, u32 timeOffset, bool full, bool push) {
    EGG::Vector3f vel = v0 - m_pos;

    if (vel.z < 0.0f || vel.z > COLLISION_RADIUS) {
        return false;
    }

    if (!(flags & KCL_TYPE_FLOOR)) {
        return false;
    }

    u32 t = timeOffset + System::RaceManager::Instance()->timer();
    EGG::Vector3f bbox;
    EGG::Vector3f fnrm;
    f32 dist;

    if (!FUN_807FB060(radius, vel, t, bbox, fnrm, dist)) {
        return false;
    }

    if (pInfo) {
        pInfo->bbox.min = pInfo->bbox.min.minimize(bbox);
        pInfo->bbox.max = pInfo->bbox.max.maximize(bbox);

        if (full) {
            pInfo->updateFloor(dist, fnrm);
        }
    }

    if (pFlagsOut) {
        auto *colDirector = CollisionDirector::Instance();

        if (push) {
            colDirector->pushCollisionEntry(dist, pFlagsOut, KCL_TYPE_BIT(COL_TYPE_ROAD2),
                    COL_TYPE_ROAD2);
            colDirector->setCurrentCollisionVariant(7);
            colDirector->setCurrentCollisionTrickable(true);
        } else {
            *pFlagsOut |= KCL_TYPE_BIT(COL_TYPE_ROTATING_ROAD);
        }

        if (vel.z > COLLISION_RADIUS - 600.0f * 4.0f) {
            if (push) {
                colDirector->pushCollisionEntry(dist, pFlagsOut, KCL_TYPE_BIT(COL_TYPE_BOOST_RAMP),
                        COL_TYPE_BOOST_RAMP);
            } else {
                *pFlagsOut |= KCL_TYPE_BIT(COL_TYPE_ROAD2) | KCL_TYPE_BIT(COL_TYPE_BOOST_RAMP);
            }
        }
    }

    return true;
}

/// @addr{0x807FB060}
bool ObjectAurora::FUN_807FB060(f32 radius, const EGG::Vector3f &vel, u32 time, EGG::Vector3f &v0,
        EGG::Vector3f &fnrm, f32 &dist) {
    f32 fVar1 = (time / 60.0f - 30.0f) / 60.0f;
    f32 dVar3 = std::min(1.0f + fVar1 * fVar1, 4.0f);
    f32 dVar6 = (F_PI * (2.0f * vel.z)) / COLLISION_RADIUS;

    dVar3 = EGG::Mathf::SinFIdx(40.743664f * (dVar6 * dVar3 + (F_PI * time) / 50.0f));
    dVar3 = radius - (vel.y - dVar6 * 80.0f * dVar3);

    if (dVar3 <= 0.0f || dVar3 >= 600.0f) {
        return false;
    }

    if (dVar3 > 300.0f) {
        dVar3 *= 0.2f;
    }

    // fnrm normally depends on dVar3, dVar5, and dVar6, but it gets overwritten by the next line
    fnrm = EGG::Vector3f(0.0f, 1.0f, 0.0f);
    v0 = fnrm * dVar3;
    dist = dVar3;

    return true;
}

} // namespace Field
