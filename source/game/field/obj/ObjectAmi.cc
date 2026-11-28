#include "ObjectAmi.hh"

#include "game/field/CollisionDirector.hh"

#include "game/system/RaceManager.hh"

namespace Kinoko::Field {

/// @addr{0x80807ED0}
/// @copybrief ObjectDrivable::ObjectDrivable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
ObjectAmi::ObjectAmi(const System::MapdataGeoObj &params) : ObjectDrivable(params) {}

/// @addr{0x80808860}
/// @brief Default virtual destructor
ObjectAmi::~ObjectAmi() = default;

/// @addr{0x80808858}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectAmi::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x80808854}
/// @copybrief ObjectBase::createCollision()
/// @details This is a no-op as collision is handled exclusively in @ref checkSphereImpl().
void ObjectAmi::createCollision() {}

/// @addr{0x80808850}
/// @copybrief ObjectBase::calcCollisionTransform()
/// @details This is a no-op as collision is handled exclusively in @ref checkSphereImpl().
void ObjectAmi::calcCollisionTransform() {}

/// @addr{0x80808840}
/// @copybrief ObjectBase::getCollisionRadius()
/// @return The fixed `15000.0f` collision radius used for this object.
f32 ObjectAmi::getCollisionRadius() const {
    return 15000.0f;
}

/// @addr{0x80808800}
/// @copydoc ObjectDrivable::checkPointPartial()
bool ObjectAmi::checkPointPartial(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfoPartial *info, KCLTypeMask *maskOut) {
    return checkSpherePartialImpl(0.0f, pos, prevPos, mask, info, maskOut, 0);
}

/// @addr{0x80808810}
/// @copydoc ObjectDrivable::checkPointPartialPush()
bool ObjectAmi::checkPointPartialPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfoPartial *info, KCLTypeMask *maskOut) {
    return checkSpherePartialPushImpl(0.0f, pos, prevPos, mask, info, maskOut, 0);
}

/// @addr{0x80808820}
/// @copydoc ObjectDrivable::checkPointFull()
bool ObjectAmi::checkPointFull(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return checkSphereFullImpl(0.0f, pos, prevPos, mask, info, maskOut, 0);
}

/// @addr{0x80808830}
/// @copydoc ObjectDrivable::checkPointFullPush()
bool ObjectAmi::checkPointFullPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return checkSphereFullPushImpl(0.0f, pos, prevPos, mask, info, maskOut, 0);
}

/// @addr{0x808087F0}
/// @copydoc ObjectDrivable::checkSpherePartial()
bool ObjectAmi::checkSpherePartial(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSpherePartialImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset);
}

/// @addr{0x808087F4}
/// @copydoc ObjectDrivable::checkSpherePartialPush()
bool ObjectAmi::checkSpherePartialPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSpherePartialPushImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset);
}

/// @addr{0x808087F8}
/// @copydoc ObjectDrivable::checkSphereFull()
bool ObjectAmi::checkSphereFull(f32 radius, const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSphereFullImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset);
}

/// @addr{0x808087FC}
/// @copydoc ObjectDrivable::checkSphereFullPush()
bool ObjectAmi::checkSphereFullPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkSphereFullPushImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset);
}

/// @addr{0x808087B0}
/// @copydoc ObjectDrivable::checkPointCachedPartial()
bool ObjectAmi::checkPointCachedPartial(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfoPartial *info, KCLTypeMask *maskOut) {
    return checkSpherePartialImpl(0.0f, pos, prevPos, mask, info, maskOut, 0);
}

/// @addr{0x808087C0}
/// @copydoc ObjectDrivable::checkPointCachedPartialPush()
bool ObjectAmi::checkPointCachedPartialPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfoPartial *info, KCLTypeMask *maskOut) {
    return checkSpherePartialPushImpl(0.0f, pos, prevPos, mask, info, maskOut, 0);
}

/// @addr{0x808087D0}
/// @copydoc ObjectDrivable::checkPointCachedFull()
bool ObjectAmi::checkPointCachedFull(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return checkSphereFullImpl(0.0f, pos, prevPos, mask, info, maskOut, 0);
}

/// @addr{0x808087E0}
/// @copydoc ObjectDrivable::checkPointCachedFullPush()
bool ObjectAmi::checkPointCachedFullPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return checkSphereFullPushImpl(0.0f, pos, prevPos, mask, info, maskOut, 0);
}

/// @addr{0x808087A0}
/// @copydoc ObjectDrivable::checkSphereCachedPartial()
bool ObjectAmi::checkSphereCachedPartial(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSpherePartialImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset);
}

/// @addr{0x808087A4}
/// @copydoc ObjectDrivable::checkSphereCachedPartialPush()
bool ObjectAmi::checkSphereCachedPartialPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSpherePartialPushImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset);
}

/// @addr{0x808087A8}
/// @copydoc ObjectDrivable::checkSphereCachedFull()
bool ObjectAmi::checkSphereCachedFull(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkSphereFullImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset);
}

/// @addr{0x808087AC}
/// @copydoc ObjectDrivable::checkSphereCachedFullPush()
bool ObjectAmi::checkSphereCachedFullPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkSphereFullPushImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset);
}

/// @addr{0x808088A0}
/// @brief Private function that checks collision between a sphere and the object, writing
/// partial collision info
/// @param radius The radius of the sphere to check
/// @param pos The position of the sphere to check
/// @param prevPos The previous position of the sphere (unused)
/// @param mask The KCL flags to check collision against (other types are ignored)
/// @param info Out parameter for retrieving partial collision information (if any)
/// @param maskOut The KCL flags that were hit during the collision check (if any)
/// @param timeOffset Optional time offset
/// @return Whether a collision was detected
bool ObjectAmi::checkSpherePartialImpl(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f & /*prevPos*/, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSphereImpl(radius, pos, mask, info, maskOut, timeOffset, false);
}

/// @addr{0x80808A8C}
/// @brief Private function that checks collision between a sphere and the object, writing
/// partial collision info. Additionally pushes the collision entry into the @ref
/// CollisionDirector cache.
/// @param radius The radius of the sphere to check
/// @param pos The position of the sphere to check
/// @param prevPos The previous position of the sphere (unused)
/// @param mask The KCL flags to check collision against (other types are ignored)
/// @param info Out parameter for retrieving partial collision information (if any)
/// @param maskOut The KCL flags that were hit during the collision check (if any)
/// @param timeOffset Optional time offset
/// @return Whether a collision was detected
bool ObjectAmi::checkSpherePartialPushImpl(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f & /*prevPos*/, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSphereImpl(radius, pos, mask, info, maskOut, timeOffset, true);
}

/// @addr{0x80808CA8}
/// @brief Private function that checks collision between a sphere and the object, writing
/// out full collision info
/// @param radius The radius of the sphere to check
/// @param pos The position of the sphere to check
/// @param prevPos The previous position of the sphere (unused)
/// @param mask The KCL flags to check collision against (other types are ignored)
/// @param info Out parameter for retrieving collision information (if any)
/// @param maskOut The KCL flags that were hit during the collision check (if any)
/// @param timeOffset Optional time offset
/// @return Whether a collision was detected
bool ObjectAmi::checkSphereFullImpl(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f & /*prevPos*/, KCLTypeMask mask, CollisionInfo *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSphereImpl(radius, pos, mask, info, maskOut, timeOffset, false);
}

/// @addr{0x80809060}
/// @brief Private function that checks collision between a sphere and the object, writing
/// out full collision info. Additionally pushes the collision entry into the @ref
/// CollisionDirector cache.
/// @param radius The radius of the sphere to check
/// @param pos The position of the sphere to check
/// @param prevPos The previous position of the sphere (unused)
/// @param mask The KCL flags to check collision against (other types are ignored)
/// @param info Out parameter for retrieving collision information (if any)
/// @param maskOut The KCL flags that were hit during the collision check (if any)
/// @param timeOffset Optional time offset
/// @return Whether a collision was detected
bool ObjectAmi::checkSphereFullPushImpl(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f & /*prevPos*/, KCLTypeMask mask, CollisionInfo *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSphereImpl(radius, pos, mask, info, maskOut, timeOffset, true);
}

/// @brief Helper function which contains frequently re-used code. Behavior branches depending
/// on whether it is a full or partial check (call CollisionInfo::updateFloor) or push (push
/// entry in the CollisionDirector).
/// @tparam T The collision info object type, either @ref CollisionInfoPartial or @ref
/// CollisionInfo.
/// @param radius The radius of the sphere to check
/// @param pos The current position of the sphere to check
/// @param mask The KCL flags to check collision against (other types are ignored)
/// @param info Out parameter for retrieving full or partial collision information (if any)
/// @param maskOut The KCL flags that were hit during the collision check (if any)
/// @param timeOffset Optional time offset
/// @param push Whether to push a collision entry
/// @return Whether a collision was detected
/// @details If `mask` does not include `KCL_TYPE_FLOOR`, the function will immediately return
/// `false` without performing further collision checks. This check normally occurs after
/// checking the kart's position relative to the net, but we take creative liberty to move this
/// check earlier as a slight performance improvement. If the kart's X or Z position is outside the
/// net's bounds, early returns `false` without performing a collision check. Calls @ref
/// checkCollision() using the computed relative position. If a collision did not occur, returns
/// `false`. Otherwise, updates collision information accordingly, depending on whether `info`
/// was provided, the type of collision info object, whether `maskOut` was provided, and
/// whether a `push` is requested.
template <typename T>
    requires std::is_same_v<T, CollisionInfo> || std::is_same_v<T, CollisionInfoPartial>
bool ObjectAmi::checkSphereImpl(f32 radius, const EGG::Vector3f &pos, KCLTypeMask mask, T *info,
        KCLTypeMask *maskOut, u32 timeOffset, bool push) {
    // We check flags first to avoid unnecessary position posDelta computation.
    if (!(mask & KCL_TYPE_FLOOR)) {
        return false;
    }

    EGG::Vector3f posDelta = pos - ObjectBase::pos();
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

    if (info) {
        info->updateBBox(bbox);

        if constexpr (std::is_same_v<T, CollisionInfo>) {
            info->updateFloor(dist, fnrm);
        }
    }

    if (maskOut) {
        if (push) {
            auto *colDirector = CollisionDirector::Instance();
            colDirector->pushCollisionEntry(dist, maskOut, KColType::RotatingRoad);
            colDirector->setCurrentCollisionVariant(0);
            colDirector->setCurrentCollisionTrickable(true);
        } else {
            *maskOut |= KCL_TYPE_BIT(KColType::RotatingRoad);
        }
    }

    return true;
}

/// @addr{0x80808308}
/// @brief Calculates the hit depth of the net and writes out collision information if a
/// collision is detected
/// @param radius The radius of the hitbox to check against the net
/// @param posDelta The position of the hitbox relative to the net's position
/// @param time The current race time
/// @param bbox Out parameter for retrieving the bounding box of the collision (if any)
/// @param fnrm Out parameter for retrieving the floor normal of the collision (if any)
/// @param dist Out parameter for retrieving the depth of the collision (if any)
/// @return `true` if a collision with the net was detected, `false` otherwise.
/// @details If the kart's relative position along the Y-axis is above the net's surface or is
/// `600.0f` units below the net, no collision is detected. Otherwise, the kart is considered to
/// be colliding with the net. If the kart is more than `300.0f` units below the net, the
/// collision depth is reduced to 20% of its original value to prevent excessive position
/// snapping for the kart. Sets `fnrm` to the floor normal of the net's surface, `dist` based on
/// the collision depth, and `bbox` based on `fnrm * dist`.
bool ObjectAmi::checkCollision(f32 radius, const EGG::Vector3f &posDelta, u32 time,
        EGG::Vector3f &bbox, EGG::Vector3f &fnrm, f32 &dist) {
    constexpr EGG::Vector3f FLOOR_NORMAL = EGG::Vector3f(0.0f, 1.5f, -0.5f);
    constexpr f32 COL_DEPTH_MAX = 600.0f;
    constexpr f32 COL_DEPTH_DAMP_THRESHOLD = 300.0f;
    constexpr f32 COL_DAMP_FACTOR = 0.2f;

    f32 depth = radius - (posDelta.y - CalcNetHeight(posDelta.z, time));

    // Not colliding if player is above the net or is more than 600 units below it
    if (depth <= 0.0f || depth >= COL_DEPTH_MAX) {
        return false;
    }

    if (depth >= COL_DEPTH_DAMP_THRESHOLD) {
        depth *= COL_DAMP_FACTOR;
    }

    fnrm = FLOOR_NORMAL;
    fnrm.normalise();
    bbox = fnrm * depth;
    dist = depth;

    return true;
}

/// @addr{0x80808220}
/// @brief Based off the provided Z-axis position offset and time, calculates the net surface
/// height for use in collision checks
/// @param relPosZ The kart's position along the Z-axis relative to the start of the net
/// @param t The current time used to calculate the net height
/// @return The net surface height at the given Z-axis position and time
/// @details The net height is computed based on a spatial sine wave along the Z-axis and a
/// temporal sine wave over time.
f32 ObjectAmi::CalcNetHeight(f32 relPosZ, u32 t) {
    constexpr f32 Z_SLOPE = 910.0f;

    f32 zPhase = F_PI * (2.0f * relPosZ) / DIMS.z;
    return SpatialSin(zPhase) * TemporalSin(t) - Z_SLOPE * zPhase;
}

/// @addr{0x80808578}
/// @brief Computes a spatial sine wave as a function of the z-axis phase.
/// @param phase The phase along the z-axis used to calculate the spatial sine wave.
/// @return The value of the spatial sine wave at the given phase.
/// @details The sine wave has an amplitude of `550.0f`. The overall behavior is such that the
/// net bounce is the most extreme when in the middle of the net and dampened as you approach
/// the beginning or end along the z-axis.
f32 ObjectAmi::SpatialSin(f32 phase) {
    constexpr f32 AMPLITUDE = 550.0f;

    return AMPLITUDE * EGG::Mathf::SinFIdx(RAD2FIDX * (phase * 0.5f));
}

/// @brief Computes a temporal sine wave as a function of time.
/// @param t The current time used to calculate the temporal sine wave.
/// @return The value of the temporal sine wave at the given time.
/// @details This computes the up/down motion of the net, with a period of 70 frames.
f32 ObjectAmi::TemporalSin(u32 t) {
    constexpr f32 HALF_PERIOD = 35.0f;

    return EGG::Mathf::SinFIdx(RAD2FIDX * (F_PI * static_cast<f32>(t) / HALF_PERIOD));
}

} // namespace Kinoko::Field
