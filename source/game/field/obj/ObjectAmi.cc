#include "ObjectAmi.hh"

#include "game/field/CollisionDirector.hh"

#include "game/system/RaceManager.hh"

#include <egg/math/Math.hh>

namespace Field {

/// @addr{0x80807ED0}
ObjectAmi::ObjectAmi(const System::MapdataGeoObj &params) : ObjectDrivable(params) {}

/// @addr{0x80808860}
ObjectAmi::~ObjectAmi() = default;

/// @addr{0x80807F40}
void ObjectAmi::init() {}

/// @addr{0x80807FF4}
void ObjectAmi::calc() {}

/// @addr{0x80808858}
u32 ObjectAmi::loadFlags() const {
    return 1;
}

/// @addr{0x80808854}
void ObjectAmi::createCollision() {}

/// @addr{0x80808850}
void ObjectAmi::calcCollisionTransform() {}

/// @addr{0x80808840}
f32 ObjectAmi::getCollisionRadius() const {
    return 15000.0f;
}

/// @addr{0x808064a8}
bool ObjectAmi::checkPointPartial(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSpherePartialImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x808064b8}
bool ObjectAmi::checkPointPartialPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSpherePartialPushImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x808064c8}
bool ObjectAmi::checkPointFull(const EGG::Vector3f &v0, const EGG::Vector3f &v1, KCLTypeMask flags,
        CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSphereFullImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x808064d8}
bool ObjectAmi::checkPointFullPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSphereFullPushImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x80806498}
bool ObjectAmi::checkSpherePartial(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSpherePartialImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x8080649c}
bool ObjectAmi::checkSpherePartialPush(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSpherePartialPushImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x808064a0}
bool ObjectAmi::checkSphereFull(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSphereFullImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x808064a4}
bool ObjectAmi::checkSphereFullPush(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSphereFullPushImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x80806458}
bool ObjectAmi::checkPointCachedPartial(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSpherePartialImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x80806468}
bool ObjectAmi::checkPointCachedPartialPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSpherePartialPushImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x80806478}
bool ObjectAmi::checkPointCachedFull(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSphereFullImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x80806488}
bool ObjectAmi::checkPointCachedFullPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSphereFullPushImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x80806448}
bool ObjectAmi::checkSphereCachedPartial(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSpherePartialImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x8080644c}
bool ObjectAmi::checkSphereCachedPartialPush(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSpherePartialPushImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x80806450}
bool ObjectAmi::checkSphereCachedFull(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSphereFullImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x80806454}
bool ObjectAmi::checkSphereCachedFullPush(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSphereFullPushImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x80806554}
bool ObjectAmi::checkSpherePartialImpl(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSphereImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset, false, false);
}

/// @addr{0x808068A0}
bool ObjectAmi::checkSpherePartialPushImpl(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSphereImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset, false, true);
}

/// @addr{0x80806C24}
bool ObjectAmi::checkSphereFullImpl(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSphereImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset, true, false);
}

/// @addr{0x80807110}
bool ObjectAmi::checkSphereFullPushImpl(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSphereImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset, true, true);
}

/// @brief Helper function which contains frequently re-used code. Behavior branches depending on
/// whether it is a full or partial check (call CollisionInfo::updateFloor) or push (push entry in
/// the CollisionDirector).
/// @param full Whether to update the floor in the CollisionInfo
/// @param push Whether to push a collision entry
bool ObjectAmi::checkSphereImpl(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f & /*v1*/,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut, u32 timeOffset, bool full,
        bool push) {
    EGG::Vector3f vel = v0 - m_pos;
    vel.z *= -1.0f;

    if (vel.z < 0.0f || vel.z > m_period || EGG::Mathf::abs(vel.x) > m_ac.y) {
        return false;
    }

    if (!(flags & KCL_TYPE_FLOOR)) {
        return false;
    }

    u32 t = timeOffset + System::RaceManager::Instance()->timer();
    EGG::Vector3f bbox;
    EGG::Vector3f fnrm;
    f32 dist;

    if (!FUN_80808308(radius, vel, t, bbox, fnrm, dist)) {
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
        if (push) {
            auto *colDirector = CollisionDirector::Instance();
            colDirector->pushCollisionEntry(dist, pFlagsOut, KCL_TYPE_BIT(COL_TYPE_ROTATING_ROAD),
                    COL_TYPE_ROTATING_ROAD);
            colDirector->setCurrentCollisionVariant(0);
            colDirector->setCurrentCollisionTrickable(true);
        } else {
            *pFlagsOut |= KCL_TYPE_BIT(COL_TYPE_ROTATING_ROAD);
        }

        if (vel.z > 13800.0f) {
            *pFlagsOut |= KCL_TYPE_BIT(COL_TYPE_BOOST_RAMP);
        }
    }

    return true;
}

/// @addr{0x80808308}
bool ObjectAmi::FUN_80808308(f32 radius, const EGG::Vector3f &vel, u32 time, EGG::Vector3f &v0,
        EGG::Vector3f &fnrm, f32 &dist) {
    f32 dVar4 = (F_PI * (2.0f * vel.z)) / 13800.0f;

    // Normally dVar4 is used to compute dVar2, but for some reason, in practice, it is always
    // multiplied by 0.0f.
    f32 dVar2 = EGG::Mathf::SinFIdx(40.743664f * ((F_PI * time) / 35.0f));
    f32 dVar3 = EGG::Mathf::SinFIdx(40.743664f * (dVar4 * 0.5f));
    f32 dVar5 = radius - (vel.y - (((550.0f * dVar3) * dVar2) - 910.0f * dVar4));

    if (dVar5 <= 0.0f || dVar5 >= 600.0f) {
        return false;
    }

    if (dVar5 >= 300.0f) {
        dVar5 *= 0.2f;
    }

    fnrm = EGG::Vector3f(0.0f, 1.5f, -0.5f);
    fnrm.normalise();
    v0 = fnrm * dVar5;
    dist = dVar5;

    return true;
}

} // namespace Field
