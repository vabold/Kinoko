#include "ObjectAurora.hh"

#include "game/field/CollisionDirector.hh"

#include "game/system/RaceManager.hh"

namespace Field {

/// @addr{0x807FAB58}
ObjectAurora::ObjectAurora(const System::MapdataGeoObj &params) : ObjectDrivable(params) {}

/// @addr{0x807FB690}
ObjectAurora::~ObjectAurora() = default;

/// @addr{0x807FABC4}
void ObjectAurora::init() {}

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
    return COLLISION_SIZE.z + 100.0f;
}

/// @addr{0x807FB59C}
bool ObjectAurora::checkPointPartial(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfoPartial *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSpherePartialImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x807FB5AC}
bool ObjectAurora::checkPointPartialPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfoPartial *pInfo, KCLTypeMask *pFlagsOut) {
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
        KCLTypeMask flags, CollisionInfoPartial *pInfo, KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSpherePartialImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x807FB590}
bool ObjectAurora::checkSpherePartialPush(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
        KCLTypeMask *pFlagsOut, u32 timeOffset) {
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
        KCLTypeMask flags, CollisionInfoPartial *pInfo, KCLTypeMask *pFlagsOut) {
    return checkSpherePartialImpl(0.0f, v0, v1, flags, pInfo, pFlagsOut, 0);
}

/// @addr{0x807FB55C}
bool ObjectAurora::checkPointCachedPartialPush(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfoPartial *pInfo, KCLTypeMask *pFlagsOut) {
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
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
        KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSpherePartialImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset);
}

/// @addr{0x807FB540}
bool ObjectAurora::checkSphereCachedPartialPush(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
        KCLTypeMask *pFlagsOut, u32 timeOffset) {
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
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
        KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSpherePartialImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset, false);
}

/// @addr{0x807FB8B0}
bool ObjectAurora::checkSpherePartialPushImpl(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfoPartial *pInfo,
        KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSpherePartialImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset, true);
}

/// @addr{0x807FBAC0}
bool ObjectAurora::checkSphereFullImpl(f32 radius, const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut, u32 timeOffset) {
    return checkSphereFullImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset, false);
}

/// @addr{0x807FBE6C}
bool ObjectAurora::checkSphereFullPushImpl(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1, KCLTypeMask flags, CollisionInfo *pInfo, KCLTypeMask *pFlagsOut,
        u32 timeOffset) {
    return checkSphereFullImpl(radius, v0, v1, flags, pInfo, pFlagsOut, timeOffset, true);
}

/// @brief Helper function which contains frequently re-used code. Behavior branches depending on
/// whether it is a push (push entry in the CollisionDirector).
/// @param push Whether to push a collision entry
bool ObjectAurora::checkSpherePartialImpl(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f & /*v1*/, KCLTypeMask flags, CollisionInfoPartial *pInfo,
        KCLTypeMask *pFlagsOut, u32 timeOffset, bool push) {
    EGG::Vector3f vel = v0 - m_pos;

    if (vel.z < 0.0f || vel.z > COLLISION_SIZE.z || EGG::Mathf::abs(vel.x) > COLLISION_SIZE.x) {
        return false;
    }

    if (!(flags & KCL_TYPE_FLOOR)) {
        return false;
    }

    u32 t = timeOffset + System::RaceManager::Instance()->timer();
    EGG::Vector3f bbox;
    EGG::Vector3f fnrm;
    f32 dist;

    if (!calcCollision(radius, vel, t, bbox, fnrm, dist)) {
        return false;
    }

    if (pInfo) {
        pInfo->bbox.min = pInfo->bbox.min.minimize(bbox);
        pInfo->bbox.max = pInfo->bbox.max.maximize(bbox);
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

        if (vel.z > COLLISION_SIZE.z - 600.0f * 4.0f) {
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

/// @brief Helper function which contains frequently re-used code. Behavior branches depending on
/// whether it is a push (push entry in the CollisionDirector).
/// @param push Whether to push a collision entry
bool ObjectAurora::checkSphereFullImpl(f32 radius, const EGG::Vector3f &v0,
        const EGG::Vector3f & /*v1*/, KCLTypeMask flags, CollisionInfo *pInfo,
        KCLTypeMask *pFlagsOut, u32 timeOffset, bool push) {
    EGG::Vector3f vel = v0 - m_pos;

    if (vel.z < 0.0f || vel.z > COLLISION_SIZE.z || EGG::Mathf::abs(vel.x) > COLLISION_SIZE.x) {
        return false;
    }

    if (!(flags & KCL_TYPE_FLOOR)) {
        return false;
    }

    u32 t = timeOffset + System::RaceManager::Instance()->timer();
    EGG::Vector3f bbox;
    EGG::Vector3f fnrm;
    f32 dist;

    if (!calcCollision(radius, vel, t, bbox, fnrm, dist)) {
        return false;
    }

    if (pInfo) {
        pInfo->bbox.min = pInfo->bbox.min.minimize(bbox);
        pInfo->bbox.max = pInfo->bbox.max.maximize(bbox);

        pInfo->updateFloor(dist, fnrm);
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

        if (vel.z > COLLISION_SIZE.z - 600.0f * 4.0f) {
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
/// @brief Calculates the sin-like collision of the wavy road.
/// @details It seems to be modeled as a quadratic chirp with a starting frequency of 1 and a max
/// frequency of 4. The minimum frequency of 1 occurs 30s into the race, and the maximum frequency
/// occurs 2min 13s into the race. The "observed" frequency is dependent on the player's velocity
/// towards the wavy road as well.
bool ObjectAurora::calcCollision(f32 radius, const EGG::Vector3f &vel, u32 time, EGG::Vector3f &v0,
        EGG::Vector3f &fnrm, f32 &dist) {
    constexpr f32 INITIAL_FREQUENCY = 1.0f;
    constexpr f32 MAX_FREQUENCY = 4.0f;
    constexpr f32 PHASE_SHIFT_SECONDS = 30.0f;
    constexpr f32 COLLISION_DISTANCE_THRESHOLD = 600.0f;
    constexpr f32 UPWARP_THRESHOLD = 300.0f;
    constexpr f32 UPWARP_DIST_SCALAR = 0.2f;

    f32 minsNormalized = (static_cast<f32>(time) / 60.0f - PHASE_SHIFT_SECONDS) / 60.0f;
    f32 frequency = std::min(INITIAL_FREQUENCY + minsNormalized * minsNormalized, MAX_FREQUENCY);
    f32 velPeriod = (F_PI * (2.0f * vel.z)) / COLLISION_SIZE.z;

    f32 result = EGG::Mathf::SinFIdx(
            RAD2FIDX * (velPeriod * frequency + (F_PI * static_cast<f32>(time) / 50.0f)));
    result = radius - (vel.y - velPeriod * 80.0f * result);

    // We're not colliding if we're 600 units away or if the road is now behind us.
    if (result <= 0.0f || result >= COLLISION_DISTANCE_THRESHOLD) {
        return false;
    }

    // Responsible for the sudden upwards snap that can occur when falling off.
    if (result > UPWARP_THRESHOLD) {
        result *= UPWARP_DIST_SCALAR;
    }

    fnrm = EGG::Vector3f::ey;
    v0 = fnrm * result;
    dist = result;

    return true;
}

} // namespace Field
