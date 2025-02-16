#include "ObjectTuribashi.hh"

#include "game/field/CollisionDirector.hh"

#include "game/system/RaceManager.hh"

#include <egg/math/Math.hh>

namespace Field {

/// @addr{0x80805A4C}
ObjectTuribashi::ObjectTuribashi(const System::MapdataGeoObj &params)
    : ObjectDrivable(params), m_radius(7243.3198f) {}

/// @addr{0x80806514}
ObjectTuribashi::~ObjectTuribashi() = default;

/// @addr{0x80805ad8}
void ObjectTuribashi::init() {}

/// @addr{0x80805c24}
void ObjectTuribashi::calc() {}

/// @addr{0x8080650c}
u32 ObjectTuribashi::loadFlags() const {
    return 1;
}

/// @addr{0x80806508}
void ObjectTuribashi::createCollision() {}

/// @addr{0x80806504}
void ObjectTuribashi::calcCollisionTransform() {}

/// @addr{0x808064e8}
f32 ObjectTuribashi::getCollisionRadius() const {
    return m_radius + 100.0f;
}

/// @addr{0x808064a8}
bool ObjectTuribashi::checkPointPartial(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSpherePartialImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x808064b8}
bool ObjectTuribashi::checkPointPartialPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSpherePartialPushImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x808064c8}
bool ObjectTuribashi::checkPointFull(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSphereFullImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x808064d8}
bool ObjectTuribashi::checkPointFullPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSphereFullPushImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x80806498}
bool ObjectTuribashi::checkSpherePartial(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSpherePartialImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x8080649c}
bool ObjectTuribashi::checkSpherePartialPush(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSpherePartialPushImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x808064a0}
bool ObjectTuribashi::checkSphereFull(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSphereFullImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x808064a4}
bool ObjectTuribashi::checkSphereFullPush(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSphereFullPushImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x80806458}
bool ObjectTuribashi::checkPointCachedPartial(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSpherePartialImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x80806468}
bool ObjectTuribashi::checkPointCachedPartialPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSpherePartialPushImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x80806478}
bool ObjectTuribashi::checkPointCachedFull(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSphereFullImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x80806488}
bool ObjectTuribashi::checkPointCachedFullPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSphereFullPushImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x80806448}
bool ObjectTuribashi::checkSphereCachedPartial(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSpherePartialImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x8080644c}
bool ObjectTuribashi::checkSphereCachedPartialPush(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSpherePartialPushImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x80806450}
bool ObjectTuribashi::checkSphereCachedFull(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSphereFullImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x80806454}
bool ObjectTuribashi::checkSphereCachedFullPush(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSphereFullPushImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x80806554}
bool ObjectTuribashi::checkSpherePartialImpl(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSphereImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset, false, false);
}

/// @addr{0x808068A0}
bool ObjectTuribashi::checkSpherePartialPushImpl(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSphereImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset, false, true);
}

/// @addr{0x80806C24}
bool ObjectTuribashi::checkSphereFullImpl(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSphereImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset, true, false);
}

/// @addr{0x80807110}
bool ObjectTuribashi::checkSphereFullPushImpl(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSphereImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset, true, true);
}

/// @brief Helper function which contains frequently re-used code. Behavior branches depending on
/// whether it is a full or partial check (call CollisionInfo::updateFloor) or push (push entry in
/// the CollisionDirector).
/// @details Interestingly, this function makes assumptions about the object's rotation. Even if the
/// object is rotated, collision will still act as if it is oriented along the z-axis.
/// @param full Whether to update the floor in the CollisionInfo
/// @param push Whether to push a collision entry
bool ObjectTuribashi::checkSphereImpl(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f & /*v1*/, KCLTypeMask flags, CollisionInfo *pInfo,
        KCLTypeMask *pFlagsOut, u32 timeOffset, bool full, bool push) {
    constexpr u16 PERIOD_FRAMES = 160;

    EGG::Vector3f vel = v0 - m_pos;

    if (m_radius < EGG::Mathf::abs(vel.z)) {
        return false;
    }

    // This check normally happens after the stage check,
    // but there's no difference in behavior if we check earlier.
    if (!(flags & 1)) {
        return false;
    }

    auto *raceMgr = System::RaceManager::Instance();
    f32 dVar8 = 0.0f;

    if (raceMgr->isStageReached(System::RaceManager::Stage::Race)) {
        f32 radiusFactor = -vel.z / m_radius;
        f32 bound1 = radiusFactor - 1.0f;
        f32 bound2 = radiusFactor + 1.0f;

        u32 t = (timeOffset + raceMgr->timer()) % PERIOD_FRAMES;

        f32 dVar7 = EGG::Mathf::SinFIdx(
                (t * 2.0f * F_PI / PERIOD_FRAMES + 0.7f * radiusFactor) * RAD2FIDX);
        dVar8 = 0.12f * (bound2 * (bound2 * (bound1 * (bound1 * dVar7))));
    }

    auto [sinfidx, cosfidx] = EGG::Mathf::SinCosFIdx(RAD2FIDX * dVar8);

    f32 radiusFactor = vel.z / m_radius;
    f32 dVar7 = EGG::Mathf::CosFIdx(radiusFactor * 37.0f * F_PI * 0.5f * RAD2FIDX);
    f32 fVar1 = m_b4 * -0.5f;

    f32 dVar6 = fVar1 +
            (cosfidx * (-vel.y - fVar1) +
                    (-vel.x * sinfidx +
                            (radius + (EGG::Mathf::abs(30.0f * dVar7) + 25.0f) + 0.156f * vel.z)));

    if (dVar6 <= 0.0f || dVar6 >= 600.0f) {
        return false;
    }

    if (dVar6 >= 300.0f) {
        dVar6 *= 0.2f;
    }

    if (pInfo) {
        EGG::Vector3f scaledY = EGG::Vector3f::ey * dVar6;
        pInfo->bbox.min = pInfo->bbox.min.minimize(scaledY);
        pInfo->bbox.max = pInfo->bbox.max.maximize(scaledY);

        if (full) {
            pInfo->updateFloor(dVar6, EGG::Vector3f(sinfidx, cosfidx, 0.0f));
        }
    }

    if (pFlagsOut) {
        if (push) {
            auto *colDirector = CollisionDirector::Instance();
            colDirector->pushCollisionEntry(dVar6, pFlagsOut, KCL_TYPE_BIT(COL_TYPE_ROAD), 0);
            colDirector->setCurrentCollisionVariant(4);
            colDirector->setCurrentCollisionTrickable(false);
        } else {
            *pFlagsOut |= KCL_TYPE_BIT(COL_TYPE_ROAD);
        }
    }

    return true;
}

} // namespace Field
