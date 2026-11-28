#include "ObjectTwistedWay.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/CollisionInfo.hh"

#include "game/system/RaceManager.hh"

namespace Kinoko::Field {

/// @addr{0x80813BD4}
/// @copybrief ObjectDrivable::ObjectDrivable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
ObjectTwistedWay::ObjectTwistedWay(const System::MapdataGeoObj &params) : ObjectDrivable(params) {}

/// @addr{0x80814918}
/// @brief Default virtual destructor
ObjectTwistedWay::~ObjectTwistedWay() = default;

/// @addr{0x80813C40}
/// @copybrief ObjectBase::init()
/// @details Sets @ref m_introTimer to 1000.
void ObjectTwistedWay::init() {
    m_introTimer = 1000;
}

/// @addr{0x80813CFC}
/// @copybrief ObjectBase::calc()
/// @details If the race has not yet started, increments @ref m_introTimer. Otherwise, this
/// function does nothing.
void ObjectTwistedWay::calc() {
    if (!System::RaceManager::Instance()->isStageReached(System::RaceManager::Stage::Race)) {
        ++m_introTimer;
    }
}

/// @addr{0x8081490C}
/// @copybrief ObjectBase::createCollision()
/// @details No-op because collision is handled entirely within the collision functions.
void ObjectTwistedWay::createCollision() {}

/// @addr{0x80814908}
/// @copybrief ObjectBase::calcCollisionTransform()
/// @details No-op because collision is handled entirely within the collision functions.
void ObjectTwistedWay::calcCollisionTransform() {}

/// @addr{0x80814910}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame
ObjectBase::LoadFlags ObjectTwistedWay::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x808148F8}
/// @copybrief ObjectBase::getCollisionRadius()
/// @return The collision radius of the wavy road, @ref HALF_DEPTH
f32 ObjectTwistedWay::getCollisionRadius() const {
    return HALF_DEPTH;
}

/// @addr{0x808148B8}
/// @copydoc ObjectDrivable::checkPointPartial()
bool ObjectTwistedWay::checkPointPartial(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfoPartial *info, KCLTypeMask *maskOut) {
    return checkSpherePartialImpl(0.0f, pos, prevPos, mask, info, maskOut, 0);
}

/// @addr{0x808148C8}
/// @copydoc ObjectDrivable::checkPointPartialPush()
bool ObjectTwistedWay::checkPointPartialPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfoPartial *info, KCLTypeMask *maskOut) {
    return checkSpherePartialPushImpl(0.0f, pos, prevPos, mask, info, maskOut, 0);
}

/// @addr{0x808148D8}
/// @copydoc ObjectDrivable::checkPointFull()
bool ObjectTwistedWay::checkPointFull(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return checkSphereFullImpl(0.0f, pos, prevPos, mask, info, maskOut, 0);
}

/// @addr{0x808148E8}
/// @copydoc ObjectDrivable::checkPointFullPush()
bool ObjectTwistedWay::checkPointFullPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return checkSphereFullPushImpl(0.0f, pos, prevPos, mask, info, maskOut, 0);
}

/// @addr{0x808148A8}
/// @copydoc ObjectDrivable::checkSpherePartial()
bool ObjectTwistedWay::checkSpherePartial(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSpherePartialImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset);
}

/// @addr{0x808148AC}
/// @copydoc ObjectDrivable::checkSpherePartialPush()
bool ObjectTwistedWay::checkSpherePartialPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSpherePartialPushImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset);
}

/// @addr{0x808148B0}
/// @copydoc ObjectDrivable::checkSphereFull()
bool ObjectTwistedWay::checkSphereFull(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkSphereFullImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset);
}

/// @addr{0x808148B4}
/// @copydoc ObjectDrivable::checkSphereFullPush()
bool ObjectTwistedWay::checkSphereFullPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkSphereFullPushImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset);
}

/// @addr{0x80814868}
/// @copydoc ObjectDrivable::checkPointCachedPartial()
bool ObjectTwistedWay::checkPointCachedPartial(const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut) {
    return checkSpherePartialImpl(0.0f, pos, prevPos, mask, info, maskOut, 0);
}

/// @addr{0x80814878}
/// @copydoc ObjectDrivable::checkPointCachedPartialPush()
bool ObjectTwistedWay::checkPointCachedPartialPush(const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut) {
    return checkSpherePartialPushImpl(0.0f, pos, prevPos, mask, info, maskOut, 0);
}

/// @addr{0x80814888}
/// @copydoc ObjectDrivable::checkPointCachedFull()
bool ObjectTwistedWay::checkPointCachedFull(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return checkSphereFullImpl(0.0f, pos, prevPos, mask, info, maskOut, 0);
}

/// @addr{0x80814898}
/// @copydoc ObjectDrivable::checkPointCachedFullPush()
bool ObjectTwistedWay::checkPointCachedFullPush(const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return checkSphereFullPushImpl(0.0f, pos, prevPos, mask, info, maskOut, 0);
}

/// @addr{0x80814858}
/// @copydoc ObjectDrivable::checkSphereCachedPartial()
bool ObjectTwistedWay::checkSphereCachedPartial(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSpherePartialImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset);
}

/// @addr{0x8081485C}
/// @copydoc ObjectDrivable::checkSphereCachedPartialPush()
bool ObjectTwistedWay::checkSphereCachedPartialPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSpherePartialPushImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset);
}

/// @addr{0x80814860}
/// @copydoc ObjectDrivable::checkSphereCachedFull()
bool ObjectTwistedWay::checkSphereCachedFull(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkSphereFullImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset);
}

/// @addr{0x80814864}
/// @copydoc ObjectDrivable::checkSphereCachedFullPush()
bool ObjectTwistedWay::checkSphereCachedFullPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkSphereFullPushImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset);
}

/// @addr{0x80814958}
/// @copydoc ObjectTwistedWay::checkSpherePartial()
bool ObjectTwistedWay::checkSpherePartialImpl(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSphereImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset, false);
}

/// @addr{0x80814EA8}
/// @copydoc ObjectTwistedWay::checkSpherePartialPush()
bool ObjectTwistedWay::checkSpherePartialPushImpl(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSphereImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset, true);
}

/// @addr{0x80815444}
/// @copydoc ObjectTwistedWay::checkSphereFull()
bool ObjectTwistedWay::checkSphereFullImpl(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkSphereImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset, false);
}

/// @addr{0x80815D64}
/// @copydoc ObjectTwistedWay::checkSphereFullPush()
bool ObjectTwistedWay::checkSphereFullPushImpl(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkSphereImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset, true);
}

/// @brief Helper function for collision checks which contains frequently re-used code
/// @tparam T The type of collision information structure (@ref CollisionInfo or @ref
/// CollisionInfoPartial)
/// @param radius The radius of the sphere to check
/// @param pos The position of the sphere to check
/// @param prevPos The previous position of the sphere (unused)
/// @param mask The KCL flags to check collision against (other types are ignored)
/// @param info Out parameter for retrieving collision information (if any)
/// @param maskOut The KCL flags that were hit during the collision check (if any)
/// @param timeOffset Optional time delta
/// @param push Whether to push the collision entry to the @ref CollisionDirector
/// @return Whether a collision was detected
/// @details If the kart's Z-position is more than @ref HALF_DEPTH units away from the wavy road's
/// position, or if the kart's X-position is more than twice the @ref HALF_WIDTH units away, this
/// function early returns `false` and does not perform a collision check. Establishes the current
/// framecount to use, which is the race manager's timer if in a race, or the intro timer if the
/// race hasn't started, plus the optional `timeOffset`. This framecount is then doubled to derive
/// the time parameter `t` used in the wave angle calculation. Calculates the angle of the wavy road
/// at the current position and time. Finally, depending on which type of collision check is being
/// performed (wall and/or floor), the appropriate collision check function is called with the
/// calculated angle.
template <typename T>
    requires std::is_same_v<T, CollisionInfo> || std::is_same_v<T, CollisionInfoPartial>
bool ObjectTwistedWay::checkSphereImpl(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f & /*prevPos*/, KCLTypeMask mask, T *info, KCLTypeMask *maskOut,
        u32 timeOffset, bool push) {
    EGG::Vector3f relPos = pos - ObjectTwistedWay::pos();

    if (EGG::Mathf::abs(relPos.z) > HALF_DEPTH || EGG::Mathf::abs(relPos.x) > HALF_WIDTH * 2.0f) {
        return false;
    }

    auto *raceMgr = System::RaceManager::Instance();
    bool isInRace = raceMgr->isStageReached(System::RaceManager::Stage::Race);

    u32 frameCount = timeOffset + (isInRace ? raceMgr->timer() : m_introTimer);
    u32 t = (frameCount % PERIOD_LENGTH) * 2;

    f32 angle = isInRace ? CalcWaveAngle(-relPos.z / HALF_DEPTH, t) : 0.0f;

    bool hasCol = false;
    if (mask & KCL_TYPE_BIT(KColType::Wall)) {
        hasCol |= checkWallCollision(angle, radius, t, relPos, info, maskOut, push);
    }

    if (mask & KCL_TYPE_BIT(KColType::Road)) {
        hasCol |= checkFloorCollision(angle, radius, relPos, info, maskOut, push);
    }

    return hasCol;
}

/// @brief Helper function which contains frequently re-used code for checking wall collisions
/// @tparam T The type of collision information structure (@ref CollisionInfo or @ref
/// CollisionInfoPartial)
/// @param angle The twist angle of the wavy road at the current position and time
/// @param radius The radius of the sphere to check
/// @param t The time parameter used in the wave angle calculation (twice the elapsed race time)
/// @param relPos The position of the sphere relative to the wavy road's position
/// @param info Out parameter for retrieving collision information (if any)
/// @param maskOut The KCL flags that were hit during the collision check (if any)
/// @param push Whether to push the collision entry to the @ref CollisionDirector
/// @return Whether a wall collision was detected
/// @details Takes the sine and cosine of the provided twist `angle` and computes the distance from
/// the kart to the wall on the negative X side of the road. If the sphere is not touching that
/// wall, computes the distance to the wall on the positive X side instead. If either wall is hit,
/// updates the collision information, and if `maskOut` is provided, the KCL flags are updated
/// accordingly. If `push` is `true`, then pushes the collision entry to the @ref CollisionDirector.
/// Then returns `true` early. Otherwise if no wall was hit, recalculates the twist angle at the
/// center of the road using time `t` and calls @ref checkPoleCollision(). If there is no pole
/// collision, returns `false`. Otherwise, updates the collision information and KCL flags in the
/// same way and returns `true`.
template <typename T>
    requires std::is_same_v<T, CollisionInfo> || std::is_same_v<T, CollisionInfoPartial>
bool ObjectTwistedWay::checkWallCollision(f32 angle, f32 radius, u32 t, const EGG::Vector3f &relPos,
        T *info, KCLTypeMask *maskOut, bool push) {
    auto [sin, cos] = EGG::Mathf::SinCosFIdx(RAD2FIDX * angle);
    f32 yProj = sin * (relPos.y - HALF_WIDTH * 0.5f);
    f32 dist = radius + (yProj - cos * (relPos.x + HALF_WIDTH));

    EGG::Vector3f bbox = EGG::Vector3f::zero;
    EGG::Vector3f fnrm = EGG::Vector3f::zero;

    if (dist <= 0.0f) {
        dist = radius - (yProj - cos * (relPos.x - HALF_WIDTH));

        if (dist > 0.0f) {
            bbox = EGG::Vector3f(-dist * cos, dist * sin, 0.0f);
            fnrm = EGG::Vector3f(-cos, sin, 0.0f);
        }
    } else {
        bbox = EGG::Vector3f(dist * cos, dist * sin, 0.0f);
        fnrm = EGG::Vector3f(cos, sin, 0.0f);
    }

    if (dist > 0.0f) {
        if (info) {
            info->updateBBox(bbox);

            if constexpr (std::is_same_v<T, CollisionInfo>) {
                info->updateWall(dist, fnrm);
            }
        }

        if (maskOut) {
            if (push) {
                CollisionDirector::Instance()->pushCollisionEntry(dist, maskOut,
                        KCL_TYPE_BIT(KColType::Wall), static_cast<u16>(KColType::Wall));
            } else {
                *maskOut |= KCL_TYPE_BIT(KColType::Wall);
            }
        }

        return true;
    }

    angle = CalcWaveAngle(0.0f, t);

    EGG::Vector3f wnrm;

    if (!checkPoleCollision(radius, angle, relPos, bbox, wnrm, dist)) {
        return false;
    }

    if (info) {
        info->updateBBox(bbox);

        if constexpr (std::is_same_v<T, CollisionInfo>) {
            info->updateWall(dist, wnrm);
        }
    }

    if (maskOut) {
        if (push) {
            CollisionDirector::Instance()->pushCollisionEntry(dist, maskOut,
                    KCL_TYPE_BIT(KColType::Wall), static_cast<u16>(KColType::Wall));
        } else {
            *maskOut |= KCL_TYPE_BIT(KColType::Wall);
        }
    }

    return true;
}

/// @brief Helper function which contains frequently re-used code for checking floor collisions
/// @tparam T The type of collision information structure (@ref CollisionInfo or @ref
/// CollisionInfoPartial)
/// @param angle The twist angle of the wavy road at the current position and time
/// @param radius The radius of the sphere to check
/// @param relPos The position of the sphere relative to the wavy road's position
/// @param info Out parameter for retrieving collision information (if any)
/// @param maskOut The KCL flags that were hit during the collision check (if any)
/// @param push Whether to push the collision entry to the @ref CollisionDirector
/// @return Whether a floor collision was detected
/// @details Takes the cosine and sine of the provided twist `angle` and computes the distance from
/// the sphere to the floor. If the sphere is above the floor, then returns `false` early.
/// If `maskOut` is provided, the KCL flags are updated accordingly. If `push` is `true`, then
/// pushes the collision entry to the @ref CollisionDirector. Lastly, returns `true`.
template <typename T>
    requires std::is_same_v<T, CollisionInfo> || std::is_same_v<T, CollisionInfoPartial>
bool ObjectTwistedWay::checkFloorCollision(f32 angle, f32 radius, const EGG::Vector3f &relPos,
        T *info, KCLTypeMask *maskOut, bool push) {
    constexpr f32 QUARTER_WIDTH = HALF_WIDTH * -0.5f;
    constexpr f32 TRICKABLE_RADIUS_FACTOR = 0.6f;

    auto [sin, cos] = EGG::Mathf::SinCosFIdx(RAD2FIDX * angle);
    f32 dist = QUARTER_WIDTH + ((radius + -relPos.x * sin) + cos * (-relPos.y - QUARTER_WIDTH));

    if (dist <= 0.0f) {
        return false;
    }

    EGG::Vector3f bbox = dist * EGG::Vector3f::ey;
    EGG::Vector3f fnrm = EGG::Vector3f(sin, cos, 0.0f);

    if (info) {
        info->updateBBox(bbox);

        if constexpr (std::is_same_v<T, CollisionInfo>) {
            info->updateFloor(dist, fnrm);
        }
    }

    if (maskOut) {
        if (push) {
            auto *colDir = CollisionDirector::Instance();
            colDir->pushCollisionEntry(dist, maskOut, KCL_TYPE_BIT(KColType::Road),
                    static_cast<u16>(KColType::Road));
            colDir->setCurrentCollisionVariant(2);

            if (EGG::Mathf::abs(relPos.z) < HALF_DEPTH * TRICKABLE_RADIUS_FACTOR) {
                colDir->setCurrentCollisionTrickable(true);
            }
        } else {
            *maskOut |= KCL_TYPE_BIT(KColType::Road);
        }
    }

    return true;
}

/// @addr{0x80814270}
/// @brief Checks for collisions with the pole in the center of the wavy road
/// @param radius The radius of the sphere to check
/// @param angle The twist angle of the wavy road at the current position and time
/// @param relPos The position of the sphere relative to the wavy road's position
/// @param tangentOff The vector to push the sphere away from the collision with the pole
/// @param wnrm The normal vector of the pole at the point of collision
/// @param dist The distance from the sphere to the pole at the point of collision
/// @return `true` if a collision with the pole occurred, `false` otherwise
/// @details Computes the sine and cosine of the provided `angle`, storing them in the X and Y
/// component of a vector respectively. Computes the wall normal based on the kart's relative
/// position. Computes the kart's distance to the pole. If the kart's distance is less than the sum
/// of the pole's radius and the sphere's radius, a collision is detected and `tangentOff` and
/// `dist` are updated accordingly.
bool ObjectTwistedWay::checkPoleCollision(f32 radius, f32 angle, const EGG::Vector3f &relPos,
        EGG::Vector3f &tangentOff, EGG::Vector3f &wnrm, f32 &dist) {
    constexpr EGG::Vector3f PIVOT = EGG::Vector3f(0.0f, 0.5f * HALF_WIDTH, 0.0f);
    constexpr f32 POLE_RADIUS = 227.0f;

    auto [sin, cos] = EGG::Mathf::SinCosFIdx(angle * RAD2FIDX);
    EGG::Vector3f sinCos = EGG::Vector3f(sin, cos, 0.0f);

    wnrm = relPos - (sinCos * (relPos - PIVOT).dot(sinCos) + PIVOT);
    f32 diff = POLE_RADIUS + radius - wnrm.length();

    if (diff <= 0.0f) {
        return false;
    }

    wnrm.normalise2();
    tangentOff = wnrm * diff;
    dist = diff;

    return true;
}

/// @addr{0x80813F40}
/// @brief Calculates the road's twist angle at the given time `t` and position along the z-axis
/// @param zPercent The position along the z-axis as a percentage of the road's half-length, in
/// the range [-1.0f, 1.0f]
/// @param t Two times the current framecount in the wavy road's oscillation period
/// @return The road's twist angle (radians) at the given position and time, used to derive
/// the floor/wall normals
f32 ObjectTwistedWay::CalcWaveAngle(f32 zPercent, u32 t) {
    constexpr f32 WAVINESS = 4.0f;
    constexpr f32 AMPLITUDE = 0.2f;

    f32 angle = EGG::Mathf::SinFIdx(
            (static_cast<f32>(t) * F_PI / PERIOD_LENGTH + WAVINESS * zPercent) * RAD2FIDX);

    f32 low = zPercent - 1.0f;
    f32 high = zPercent + 1.0f;

    return AMPLITUDE * (high * (high * (low * (low * angle))));
}

} // namespace Kinoko::Field
