#include "ObjectAmi.hh"

#include "game/field/CollisionDirector.hh"

#include "game/system/RaceManager.hh"

#include <egg/math/Math.hh>

namespace Field {

/// @addr{0x80807ED0}
ObjectAmi::ObjectAmi(const System::MapdataGeoObj &params) : ObjectDrivable(params) {}

/// @addr{0x80808860}
ObjectAmi::~ObjectAmi() = default;

/// @addr{0x80808800}
bool ObjectAmi::checkPointPartial(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfoPartial *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSpherePartialImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x80808810}
bool ObjectAmi::checkPointPartialPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfoPartial *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSpherePartialPushImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x80808820}
bool ObjectAmi::checkPointFull(const EGG::Vector3f &v0, const EGG::Vector3f &v1, KCLTypeMask flags,
        CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSphereFullImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x80808830}
bool ObjectAmi::checkPointFullPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSphereFullPushImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x808087F0}
bool ObjectAmi::checkSpherePartial(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfoPartial *pInfo, KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSpherePartialImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x808087F4}
bool ObjectAmi::checkSpherePartialPush(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfoPartial *pInfo, KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSpherePartialPushImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x808087F8}
bool ObjectAmi::checkSphereFull(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSphereFullImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x808087FC}
bool ObjectAmi::checkSphereFullPush(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSphereFullPushImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x808087B0}
bool ObjectAmi::checkPointCachedPartial(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfoPartial *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSpherePartialImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x808087C0}
bool ObjectAmi::checkPointCachedPartialPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfoPartial *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSpherePartialPushImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x808087D0}
bool ObjectAmi::checkPointCachedFull(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSphereFullImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x808087E0}
bool ObjectAmi::checkPointCachedFullPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSphereFullPushImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x808087A0}
bool ObjectAmi::checkSphereCachedPartial(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
        KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSpherePartialImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x808087A4}
bool ObjectAmi::checkSphereCachedPartialPush(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
        KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSpherePartialPushImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x808087A8}
bool ObjectAmi::checkSphereCachedFull(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSphereFullImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x808087AC}
bool ObjectAmi::checkSphereCachedFullPush(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSphereFullPushImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x808088A0}
bool ObjectAmi::checkSpherePartialImpl(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfoPartial *pInfo, KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSphereImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset, false);
}

/// @addr{0x80808A8C}
bool ObjectAmi::checkSpherePartialPushImpl(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
        KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSphereImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset, true);
}

/// @addr{0x80808CA8}
bool ObjectAmi::checkSphereFullImpl(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSphereImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset, false);
}

/// @addr{0x80809060}
bool ObjectAmi::checkSphereFullPushImpl(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSphereImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset, true);
}

/// @brief Helper function which contains frequently re-used code. Behavior branches depending on
/// whether it is a full or partial check (call CollisionInfo::updateFloor) or push (push entry in
/// the CollisionDirector).
/// @tparam T The CollisionInfo object type, either CollisionInfoPartial or CollisionInfo.
/// @param push Whether to push a collision entry
template <typename T>
    requires std::is_same_v<T, CollisionInfo> || std::is_same_v<T, CollisionInfoPartial>
bool ObjectAmi::checkSphereImpl(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f & /*v1*/,
        KCLTypeMask flags, T *pInfo, KCLTypeMask *pFlagsOut, u32 timeOffset, bool push) {
    // We check flags first to avoid unnecessary position posDelta computation.
    if (!(flags & KCL_TYPE_FLOOR)) {
        return false;
    }

    EGG::Vector3f posDelta = v0 - pos();
    posDelta.z *= -1.0f;

    if (posDelta.z < 0.0f || posDelta.z > DIMS.z || EGG::Mathf::abs(posDelta.x) > DIMS.x) {
        return false;
    }

    u32 t = timeOffset + System::RaceManager::Instance()->timer();
    EGG::Vector3f bbox;
    EGG::Vector3f fnrm;
    f32 dist;

    if (!checkCollision(radius, posDelta, t, bbox, fnrm, dist)) {
        return false;
    }

    if (pInfo) {
        pInfo->bbox.min = pInfo->bbox.min.minimize(bbox);
        pInfo->bbox.max = pInfo->bbox.max.maximize(bbox);

        if constexpr (std::is_same_v<T, CollisionInfo>) {
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

        if (posDelta.z > DIMS.z) {
            *pFlagsOut |= KCL_TYPE_BIT(COL_TYPE_BOOST_RAMP);
        }
    }

    return true;
}

/// @addr{0x80808308}
bool ObjectAmi::checkCollision(f32 radius, const EGG::Vector3f &posDelta, u32 time,
        EGG::Vector3f &bbox, EGG::Vector3f &fnrm, f32 &dist) {
    constexpr EGG::Vector3f FLOOR_NORMAL = EGG::Vector3f(0.0f, 1.5f, -0.5f);
    constexpr f32 Z_SLOPE = 910.0f;

    f32 zPhase = F_PI * (2.0f * posDelta.z) / DIMS.z;
    f32 depth = radius - (posDelta.y - (SpatialSin(zPhase) * TemporalSin(time) - Z_SLOPE * zPhase));

    // Not colliding if net is falling below the hitbox's radius or above by more than 600 units.
    if (depth <= 0.0f || depth >= 600.0f) {
        return false;
    }

    if (depth >= 300.0f) {
        depth *= 0.2f;
    }

    fnrm = FLOOR_NORMAL;
    fnrm.normalise();
    bbox = fnrm * depth;
    dist = depth;

    return true;
}

/// @addr{0x80808578}
/// @brief Computes a spatial sine wave as a function of the z-axis phase.
/// @details The behavior is such that the net bounce is the most extreme when in the middle of the
/// net and dampened as you approach the beginning or end along the z-axis.
f32 ObjectAmi::SpatialSin(f32 phase) {
    return 550.0f * EGG::Mathf::SinFIdx(RAD2FIDX * (phase * 0.5f));
}

/// @brief Not emitted by the game, but computes a sine wave as a function of time.
/// @details This computes the up/down motion of the net, with a period of 70 frames.
f32 ObjectAmi::TemporalSin(u32 t) {
    return EGG::Mathf::SinFIdx(RAD2FIDX * (F_PI * static_cast<f32>(t) / 35.0f));
}

} // namespace Field
