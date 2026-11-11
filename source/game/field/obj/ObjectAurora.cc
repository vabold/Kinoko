#include "ObjectAurora.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/CollisionInfo.hh"

#include "game/system/RaceManager.hh"

namespace Kinoko::Field {

/// @addr{0x807FAB58}
/// @copybrief ObjectDrivable::ObjectDrivable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
ObjectAurora::ObjectAurora(const System::MapdataGeoObj &params) : ObjectDrivable(params) {}

/// @addr{0x807FB690}
/// @brief Default virtual destructor
ObjectAurora::~ObjectAurora() = default;

/// @addr{0x807FB688}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectAurora::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x807FB684}
/// @copybrief ObjectBase::createCollision()
/// @details This is a no-op as collision is handled in the collision check functions.
void ObjectAurora::createCollision() {}

/// @addr{0x807FB680}
/// @copybrief ObjectBase::calcCollisionTransform()
/// @details This is a no-op as collision is handled in the collision check functions.
void ObjectAurora::calcCollisionTransform() {}

/// @addr{0x807FB5DC}
/// @copybrief ObjectBase::getCollisionRadius()
/// @return The fixed collision radius used for this object, `15100.0f`.
f32 ObjectAurora::getCollisionRadius() const {
    return COLLISION_SIZE.z + 100.0f;
}

/// @addr{0x807FB59C} @addr{0x807FB6D0}
/// @copydoc ObjectDrivable::checkPointPartial()
bool ObjectAurora::checkPointPartial(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfoPartial *info, KCLTypeMask *maskOut) {
    return checkSphereImpl(0.0f, pos, prevPos, mask, info, maskOut, 0, false);
}

/// @addr{0x807FB5AC} @addr{0x807FB8B0}
/// @copydoc ObjectDrivable::checkPointPartialPush()
bool ObjectAurora::checkPointPartialPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfoPartial *info, KCLTypeMask *maskOut) {
    return checkSphereImpl(0.0f, pos, prevPos, mask, info, maskOut, 0, true);
}

/// @addr{0x807FB5BC} @addr{0x807FBAC0}
/// @copydoc ObjectDrivable::checkPointFull()
bool ObjectAurora::checkPointFull(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return checkSphereImpl(0.0f, pos, prevPos, mask, info, maskOut, 0, false);
}

/// @addr{0x807FB5CC} @addr{0x807FBE6C}
/// @copydoc ObjectDrivable::checkPointFullPush()
bool ObjectAurora::checkPointFullPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return checkSphereImpl(0.0f, pos, prevPos, mask, info, maskOut, 0, true);
}

/// @addr{0x807FB58C} @addr{0x807FB6D0}
/// @copydoc ObjectDrivable::checkSpherePartial()
bool ObjectAurora::checkSpherePartial(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSphereImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset, false);
}

/// @addr{0x807FB590} @addr{0x807FB8B0}
/// @copydoc ObjectDrivable::checkSpherePartialPush()
bool ObjectAurora::checkSpherePartialPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSphereImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset, true);
}

/// @addr{0x807FB594} @addr{0x807FBAC0}
/// @copydoc ObjectDrivable::checkSphereFull()
bool ObjectAurora::checkSphereFull(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkSphereImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset, false);
}

/// @addr{0x807FB598} @addr{0x807FBE6C}
/// @copydoc ObjectDrivable::checkSphereFullPush()
bool ObjectAurora::checkSphereFullPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkSphereImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset, true);
}

/// @addr{0x807FB54C} @addr{0x807FB6D0}
/// @copydoc ObjectDrivable::checkPointCachedPartial()
bool ObjectAurora::checkPointCachedPartial(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfoPartial *info, KCLTypeMask *maskOut) {
    return checkSphereImpl(0.0f, pos, prevPos, mask, info, maskOut, 0, false);
}

/// @addr{0x807FB55C} @addr{0x807FB8B0}
/// @copydoc ObjectDrivable::checkPointCachedPartialPush()
bool ObjectAurora::checkPointCachedPartialPush(const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut) {
    return checkSphereImpl(0.0f, pos, prevPos, mask, info, maskOut, 0, true);
}

/// @addr{0x807FB56C} @addr{0x807FBAC0}
/// @copydoc ObjectDrivable::checkPointCachedFull()
bool ObjectAurora::checkPointCachedFull(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return checkSphereImpl(0.0f, pos, prevPos, mask, info, maskOut, 0, false);
}

/// @addr{0x807FB57C} @addr{0x807FBE6C}
/// @copydoc ObjectDrivable::checkPointCachedFullPush()
bool ObjectAurora::checkPointCachedFullPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return checkSphereImpl(0.0f, pos, prevPos, mask, info, maskOut, 0, true);
}

/// @addr{0x807FB53C} @addr{0x807FB6D0}
/// @copydoc ObjectDrivable::checkSphereCachedPartial()
bool ObjectAurora::checkSphereCachedPartial(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSphereImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset, false);
}

/// @addr{0x807FB540} @addr{0x807FB8B0}
/// @copydoc ObjectDrivable::checkSphereCachedPartialPush()
bool ObjectAurora::checkSphereCachedPartialPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSphereImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset, true);
}

/// @addr{0x807FB544} @addr{0x807FBAC0}
/// @copydoc ObjectDrivable::checkSphereCachedFull()
bool ObjectAurora::checkSphereCachedFull(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkSphereImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset, false);
}

/// @addr{0x807FB548} @addr{0x807FBE6C}
/// @copydoc ObjectDrivable::checkSphereCachedFullPush()
bool ObjectAurora::checkSphereCachedFullPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkSphereImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset, true);
}

/// @brief Helper function for collision checks which contains frequently re-used code
/// @tparam T The collision info object type, either @ref CollisionInfoPartial or @ref
/// CollisionInfo
/// @param radius The radius of the collision sphere
/// @param pos The current position of the object
/// @param mask The collision flags
/// @param info Pointer to the collision info struct
/// @param maskOut Pointer to the output collision flags
/// @param timeOffset The time offset for the collision calculation
/// @param push Whether to push a collision entry
/// @return Whether a collision was detected
/// @details If `mask` does not include `#KCL_TYPE_FLOOR`, the function will immediately return
/// `false` without performing further collision checks. If the kart's relative X or Z position are
/// outside the bounds of the wavy road's @ref COLLISION_SIZE "bounding box", early returns `false`.
/// Calls @ref calcCollision() with the current race framecount to determine if the kart collided
/// with the wavy road, returning `false` if not. If a collision occurred, updates collision info
/// accordingly based on whether `info` was provided, whether `info` is @ref CollisionInfo or @ref
/// CollisionInfoPartial, whether `maskOut` was provided, and whether `push` is `true`. If the
/// kart's relative Z position is on the last 16% of the wavy road, then treats the collision as
/// `#COL_TYPE_BOOST_RAMP`, otherwise treats it as trickable `#COL_TYPE_ROAD2`.
/// @note OPT: The `#KCL_TYPE_FLOOR` check normally occurs after checking the kart's position
/// relative to the wavy road, but we take creative liberty to move this check earlier as a slight
/// performance improvement.
template <typename T>
    requires std::is_same_v<T, CollisionInfo> || std::is_same_v<T, CollisionInfoPartial>
bool ObjectAurora::checkSphereImpl(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f & /*prevPos*/, KCLTypeMask mask, T *info, KCLTypeMask *maskOut,
        u32 timeOffset, bool push) {
    constexpr f32 BOOST_RAMP_LENGTH = 600.0f * 4.0f;
    constexpr f32 BOOST_RAMP_START = COLLISION_SIZE.z - BOOST_RAMP_LENGTH;

    if (!(mask & KCL_TYPE_FLOOR)) {
        return false;
    }

    EGG::Vector3f relPos = pos - ObjectBase::pos();

    if (relPos.z < 0.0f || relPos.z > COLLISION_SIZE.z ||
            EGG::Mathf::abs(relPos.x) > COLLISION_SIZE.x) {
        return false;
    }

    u32 t = timeOffset + System::RaceManager::Instance()->timer();
    EGG::Vector3f offset;
    EGG::Vector3f fnrm;
    f32 dist;

    if (!calcCollision(radius, relPos, t, offset, fnrm, dist)) {
        return false;
    }

    if (info) {
        info->updateBBox(offset);

        if constexpr (std::is_same_v<T, CollisionInfo>) {
            info->updateFloor(dist, fnrm);
        }
    }

    if (maskOut) {
        auto *colDir = CollisionDirector::Instance();

        if (push) {
            colDir->pushCollisionEntry(dist, maskOut, KCL_TYPE_BIT(COL_TYPE_ROAD2), COL_TYPE_ROAD2);
            colDir->setCurrentCollisionVariant(7);
            colDir->setCurrentCollisionTrickable(true);
        } else {
            *maskOut |= KCL_TYPE_BIT(COL_TYPE_ROAD2);
        }

        if (relPos.z > BOOST_RAMP_START) {
            if (push) {
                colDir->pushCollisionEntry(dist, maskOut, KCL_TYPE_BIT(COL_TYPE_BOOST_RAMP),
                        COL_TYPE_BOOST_RAMP);
            } else {
                *maskOut |= KCL_TYPE_BIT(COL_TYPE_BOOST_RAMP);
            }
        }
    }

    return true;
}

/// @addr{0x807FB060}
/// @brief Checks whether a collision occurs with the sinusoidal wavy road
/// @param radius The radius of the collision sphere
/// @param relPos The position of the object relative to the wavy road
/// @param time Frames since countdown start
/// @param offset Out parameter for the "push out" vector of the collision
/// @param fnrm Out parameter for retrieving the floor normal of the collision (if any)
/// @param dist Out parameter for retrieving the depth of the collision (if any)
/// @return `true` if a collision was detected, `false` otherwise
/// @details Computes the Y-position of the kart relative to the wavy road's height at the given
/// `time`. If the kart is above the wavy road or `600.0f` units below the wavy road, then returns
/// `false`. Otherwise, a collision is occurring. If the kart is more than `300.0f` units below the
/// wavy road, then dampens the collision depth to 20% to avoid excessive upwards snapping of the
/// kart's position. Finally, sets `fnrm` to the world up vector, sets `dist` to the Y-axis
/// collision depth, and sets `bbox` to `dist` in the direction of world up.
bool ObjectAurora::calcCollision(f32 radius, const EGG::Vector3f &relPos, u32 time,
        EGG::Vector3f &offset, EGG::Vector3f &fnrm, f32 &dist) {
    constexpr f32 COLLISION_DISTANCE_THRESHOLD = 600.0f;
    constexpr f32 UPWARP_THRESHOLD = 300.0f;
    constexpr f32 UPWARP_DIST_SCALAR = 0.2f;

    f32 result = radius - (relPos.y - CalcRoadHeight(relPos.z, time));

    // We're not colliding if we're 600 units below or if the road is below us.
    if (result <= 0.0f || result >= COLLISION_DISTANCE_THRESHOLD) {
        return false;
    }

    // Responsible for the sudden upwards snap that can occur when falling off.
    if (result > UPWARP_THRESHOLD) {
        result *= UPWARP_DIST_SCALAR;
    }

    fnrm = EGG::Vector3f::ey;
    offset = fnrm * result;
    dist = result;

    return true;
}

/// @addr{0x807FAF10}
/// @brief Calculates the wavy road's height based on the provided relative position and time
/// @param relPosZ The Z position of the object relative to the wavy road
/// @param t Frames since countdown start
/// @return The calculated road height
/// @details Determines the phase of the sinusoidal wave based on the relative Z position of the
/// kart. Over time, @ref TemporalFrequency() causes more oscillations in the same length of road.
f32 ObjectAurora::CalcRoadHeight(f32 relPosZ, u32 t) {
    constexpr f32 HALF_PERIOD_FRAMES = 50.0f;
    constexpr f32 AMPLITUDE = 80.0f;

    f32 phase = (F_PI * (2.0f * relPosZ)) / COLLISION_SIZE.z;
    f32 result = EGG::Mathf::SinFIdx(RAD2FIDX *
            (phase * TemporalFrequency(t) + F_PI * static_cast<f32>(t) / HALF_PERIOD_FRAMES));

    return phase * AMPLITUDE * result;
}

/// @addr{0x807FAFFC}
/// @brief Computes the wavy road's sinusoidal frequency as a function of time
/// @param t Frames since countdown start
/// @return The calculated frequency
/// @details At `t=0`, the frequency is `1.25f`. It decreases to the minimum frequency of `1.0f`
/// at 30 seconds. Afterwards, it grows quadratically over time until it reaches the maximum
/// frequency of `4.0f` after 2 minutes and 14 seconds have elapsed in the race.
f32 ObjectAurora::TemporalFrequency(f32 t) {
    constexpr f32 PHASE_SHIFT_SECONDS = 30.0f;
    constexpr f32 FRAMES_PER_SECOND = 60.0f;
    constexpr f32 SECONDS_PER_MINUTE = 60.0f;
    constexpr f32 INITIAL_FREQUENCY = 1.0f;
    constexpr f32 MAX_FREQUENCY = 4.0f;

    f32 minsSincePhaseShift =
            (static_cast<f32>(t) / FRAMES_PER_SECOND - PHASE_SHIFT_SECONDS) / SECONDS_PER_MINUTE;
    return std::min(INITIAL_FREQUENCY + minsSincePhaseShift * minsSincePhaseShift, MAX_FREQUENCY);
}

} // namespace Kinoko::Field
