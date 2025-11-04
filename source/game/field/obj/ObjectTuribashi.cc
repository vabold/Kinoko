#include "ObjectTuribashi.hh"

#include "game/field/CollisionDirector.hh"

#include "game/system/RaceManager.hh"

namespace Field {

/// @addr{0x80805A4C}
ObjectTuribashi::ObjectTuribashi(const System::MapdataGeoObj &params) : ObjectDrivable(params) {}

/// @addr{0x80806514}
ObjectTuribashi::~ObjectTuribashi() = default;

/// @addr{0x808064A8}
bool ObjectTuribashi::checkPointPartial(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfoPartial *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSpherePartialImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x808064B8}
bool ObjectTuribashi::checkPointPartialPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfoPartial *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSpherePartialPushImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x808064C8}
bool ObjectTuribashi::checkPointFull(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSphereFullImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x808064D8}
bool ObjectTuribashi::checkPointFullPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSphereFullPushImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x80806498}
bool ObjectTuribashi::checkSpherePartial(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
        KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSpherePartialImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x8080649C}
bool ObjectTuribashi::checkSpherePartialPush(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
        KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSpherePartialPushImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x808064A0}
bool ObjectTuribashi::checkSphereFull(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSphereFullImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x808064A4}
bool ObjectTuribashi::checkSphereFullPush(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSphereFullPushImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x80806458}
bool ObjectTuribashi::checkPointCachedPartial(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfoPartial *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSpherePartialImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x80806468}
bool ObjectTuribashi::checkPointCachedPartialPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfoPartial *pInfo, KCLTypeMask *pFlagsOut) {
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
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
        KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSpherePartialImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x8080644C}
bool ObjectTuribashi::checkSphereCachedPartialPush(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
        KCLTypeMask *pFlagsOut, u32 timeOffset) {
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
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
        KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSphereImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset, false);
}

/// @addr{0x808068A0}
bool ObjectTuribashi::checkSpherePartialPushImpl(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
        KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSphereImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset, true);
}

/// @addr{0x80806C24}
bool ObjectTuribashi::checkSphereFullImpl(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSphereImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset, false);
}

/// @addr{0x80807110}
bool ObjectTuribashi::checkSphereFullPushImpl(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSphereImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset, true);
}

/// @brief Helper function which contains frequently re-used code. Behavior branches depending on
/// whether it is a full or partial check (call CollisionInfo::updateFloor) or push (push entry in
/// the CollisionDirector).
/// @details Interestingly, this function makes assumptions about the object's rotation. Even if the
/// object is rotated, collision will still act as if it is oriented along the z-axis.
/// @tparam T The CollisionInfo object type, either CollisionInfoPartial or CollisionInfo.
/// @param push Whether to push a collision entry
template <typename T>
    requires std::is_same_v<T, CollisionInfo> || std::is_same_v<T, CollisionInfoPartial>
bool ObjectTuribashi::checkSphereImpl(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f & /*v1*/, KCLTypeMask flags, T *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset, bool push) {
    constexpr u16 PERIOD = 160;     // Framecount of a full bridge swing.
    constexpr f32 HEIGHT = 2000.0f; // Distance between min/max positions along the angled bridge.

    // This check normally happens after the stage check,
    // but there's no difference in behavior if we check earlier.
    if ((flags & 1) == 0) {
        return false;
    }

    EGG::Vector3f deltaPos = v0 - m_pos;

    if (EGG::Mathf::abs(deltaPos.z) > HALF_LENGTH || EGG::Mathf::abs(deltaPos.x) > HALF_WIDTH) {
        return false;
    }

    const auto *raceMgr = System::RaceManager::Instance();
    f32 angle = 0.0f;

    if (raceMgr->isStageReached(System::RaceManager::Stage::Race)) {
        f32 phase = -deltaPos.z / HALF_LENGTH;
        f32 lower = phase - 1.0f;
        f32 higher = 1.0f + phase;
        u32 t = (timeOffset + raceMgr->timer()) % PERIOD;
        f32 sin = EGG::Mathf::SinFIdx(RAD2FIDX *
                (F_PI * static_cast<f32>(t * 2) / static_cast<f32>(PERIOD) + 0.7f * phase));
        angle = 0.12f * (higher * (higher * (lower * (lower * sin))));
    }

    auto [sinfidx, cosfidx] = EGG::Mathf::SinCosFIdx(RAD2FIDX * angle);

    f32 phase = deltaPos.z / HALF_LENGTH;
    f32 cos = EGG::Mathf::CosFIdx(RAD2FIDX * (0.5f * (F_PI * (37.0f * phase))));
    f32 base = HEIGHT * -0.5f;
    f32 dist = base +
            (cosfidx * (-deltaPos.y - base) +
                    (-deltaPos.x * sinfidx +
                            (radius + (EGG::Mathf::abs(30.0f * cos) + 25.0f) +
                                    0.156f * deltaPos.z)));

    if (0.0f >= dist || dist >= 600.0f) {
        return false;
    }

    if (300.0f < dist) {
        dist *= 0.2f;
    }

    if (pInfo) {
        EGG::Vector3f scaledY = EGG::Vector3f::ey * dist;
        pInfo->bbox.min = pInfo->bbox.min.minimize(scaledY);
        pInfo->bbox.max = pInfo->bbox.max.maximize(scaledY);

        if constexpr (std::is_same_v<T, CollisionInfo>) {
            pInfo->updateFloor(dist, EGG::Vector3f(sinfidx, cosfidx, 0.0f));
        }
    }

    if (pFlagsOut) {
        if (push) {
            auto *colDirector = CollisionDirector::Instance();
            colDirector->pushCollisionEntry(dist, pFlagsOut, KCL_TYPE_BIT(COL_TYPE_ROAD), 0);
            colDirector->setCurrentCollisionVariant(4);
            colDirector->setCurrentCollisionTrickable(false);
        } else {
            *pFlagsOut |= KCL_TYPE_BIT(COL_TYPE_ROAD);
        }
    }

    return true;
}

} // namespace Field
