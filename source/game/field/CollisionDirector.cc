#include "CollisionDirector.hh"

#include "game/field/CourseColMgr.hh"
#include "game/field/ObjectDrivableDirector.hh"

namespace Kinoko::Field {

/// @addr{0x8078F320}
/// @brief Checks collision between a sphere and course KCL and object collision, writing
/// partial collision info. Additionally pushes the collision entry into the @ref
/// CollisionDirector cache.
/// @param radius The radius of the sphere to check
/// @param pos The position of the sphere to check
/// @param prevPos The previous position of the sphere, used for calculating collision depth
/// @param mask The KCL flags to check collision against (other types are ignored)
/// @param info Out parameter for retrieving partial collision information (if any)
/// @param maskOut The KCL flags that were hit during the collision check (if any)
/// @param timeOffset Optional time delta
/// @return Whether a collision was detected
bool CollisionDirector::checkSpherePartialPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSphereImpl<CollisionInfoPartial>(radius, pos, prevPos, mask, info, maskOut,
            timeOffset, &CourseColMgr::checkSpherePartialPush,
            &ObjectDrivableDirector::checkSpherePartialPush, false, true);
}

/// @addr{0x8078F500}
/// @brief Checks collision between a sphere and course KCL and object collision, writing out
/// full collision info
/// @param radius The radius of the sphere to check
/// @param pos The position of the sphere to check
/// @param prevPos The previous position of the sphere, used for calculating collision depth
/// @param mask The KCL flags to check collision against (other types are ignored)
/// @param info Out parameter for retrieving collision information (if any)
/// @param maskOut The KCL flags that were hit during the collision check (if any)
/// @param timeOffset Optional time delta
/// @return Whether a collision was detected
bool CollisionDirector::checkSphereFull(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkSphereImpl<CollisionInfo>(radius, pos, prevPos, mask, info, maskOut, timeOffset,
            &CourseColMgr::checkSphereFull, &ObjectDrivableDirector::checkSphereFull, false, false);
}

/// @addr{0x8078F784}
/// @brief Checks collision between a sphere and course KCL and object collision, writing out
/// full collision info. Additionally pushes the collision entry into the @ref CollisionDirector
/// cache.
/// @param radius The radius of the sphere to check
/// @param pos The position of the sphere to check
/// @param prevPos The previous position of the sphere, used for calculating collision depth
/// @param mask The KCL flags to check collision against (other types are ignored)
/// @param info Out parameter for retrieving collision information (if any)
/// @param maskOut The KCL flags that were hit during the collision check (if any)
/// @param timeOffset Optional time delta
/// @return Whether a collision was detected
bool CollisionDirector::checkSphereFullPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkSphereImpl<CollisionInfo>(radius, pos, prevPos, mask, info, maskOut, timeOffset,
            &CourseColMgr::checkSphereFullPush, &ObjectDrivableDirector::checkSphereFullPush, false,
            true);
}

/// @addr{0x807901F0}
/// @brief Checks collision between a sphere and course KCL and object collision by using the
/// collision director's local spatial cache, writing only partial collision info
/// @param radius The radius of the sphere to check
/// @param pos The position of the sphere to check
/// @param prevPos The previous position of the sphere, used for calculating collision depth
/// @param mask The KCL flags to check collision against (other types are ignored)
/// @param info Out parameter for retrieving partial collision information (if any)
/// @param maskOut The KCL flags that were hit during the collision check (if any)
/// @param timeOffset Optional time delta
/// @return Whether a collision was detected
bool CollisionDirector::checkSphereCachedPartial(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSphereImpl<CollisionInfoPartial>(radius, pos, prevPos, mask, info, maskOut,
            timeOffset, &CourseColMgr::checkSphereCachedPartial,
            &ObjectDrivableDirector::checkSphereCachedPartial, true, false);
}

/// @addr{0x807903BC}
/// @brief Checks collision between a sphere and course KCL and object collision by using the
/// collision director's local spatial cache, writing partial collision info. Additionally
/// pushes the collision entry into the @ref CollisionDirector cache.
/// @param radius The radius of the sphere to check
/// @param pos The position of the sphere to check
/// @param prevPos The previous position of the sphere, used for calculating collision depth
/// @param mask The KCL flags to check collision against (other types are ignored)
/// @param info Out parameter for retrieving partial collision information (if any)
/// @param maskOut The KCL flags that were hit during the collision check (if any)
/// @param timeOffset Optional time delta
/// @return Whether a collision was detected
bool CollisionDirector::checkSphereCachedPartialPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSphereImpl<CollisionInfoPartial>(radius, pos, prevPos, mask, info, maskOut,
            timeOffset, &CourseColMgr::checkSphereCachedPartialPush,
            &ObjectDrivableDirector::checkSphereCachedPartialPush, true, true);
}

/// @addr{0x807907F8}
/// @brief Checks collision between a sphere and course KCL and object collision by using the
/// collision director's local spatial cache, writing out full collision info. Additionally
/// pushes the collision entry into the @ref CollisionDirector cache.
/// @param radius The radius of the sphere to check
/// @param pos The position of the sphere to check
/// @param prevPos The previous position of the sphere, used for calculating collision depth
/// @param mask The KCL flags to check collision against (other types are ignored)
/// @param info Out parameter for retrieving collision information (if any)
/// @param maskOut The KCL flags that were hit during the collision check (if any)
/// @param timeOffset Optional time delta
/// @return Whether a collision was detected
bool CollisionDirector::checkSphereCachedFullPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkSphereImpl<CollisionInfo>(radius, pos, prevPos, mask, info, maskOut, timeOffset,
            &CourseColMgr::checkSphereCachedFullPush,
            &ObjectDrivableDirector::checkSphereCachedFullPush, true, true);
}

/// @addr{0x8078E4F0}
/// @brief Narrows the spatial cache of the @ref CourseColMgr and @ref ObjectDrivableDirector to
/// only include KCL tris defined by the provided mask within a certain radius of the given
/// position.
/// @param radius The radius within which to narrow the spatial cache
/// @param pos The position around which to narrow the spatial cache
/// @param mask The KCL type mask to filter the tris
/// @param timeOffset Optional time delta
void CollisionDirector::checkCourseColNarrScLocal(f32 radius, const EGG::Vector3f &pos,
        KCLTypeMask mask, u32 timeOffset) {
    CourseColMgr::Instance()->scaledNarrowScopeLocal(1.0f, radius, nullptr, pos, mask);
    ObjectDrivableDirector::Instance()->colNarScLocal(radius, pos, mask, timeOffset);
}

/// @addr{0x807BDA7C}
/// @brief Clears the collision cache count, effectively emptying the cache
/// @param maskOut Out parameter that will be reset to 0
void CollisionDirector::resetCollisionEntries(KCLTypeMask *maskOut) {
    *maskOut = 0;
    m_collisionEntryCount = 0;
}

/// @addr{0x807BDA9C}
/// @brief Called upon finding collision that should be saved to the @ref m_entries cache
/// temporarily
/// @param dist How far the colliding body penetrates the tri
/// @param typeMask Out parameter that is updated to include the bit for the attribute's base type
/// @param attribute The attribute and additional info about the tri we are colliding with
/// @details The base game also takes the type mask bit as a separate parameter. Every caller
/// derives it from `attribute`, so Kinoko computes it here instead.
void CollisionDirector::pushCollisionEntry(f32 dist, KCLTypeMask *typeMask,
        CollisionAttribute attribute) {
    KCLTypeMask kclTypeBit = KCL_ATTRIBUTE_TYPE_BIT(attribute);
    *typeMask = *typeMask | kclTypeBit;
    m_collisionEntryCount = std::min(m_collisionEntryCount, m_entries.size() - 1);
    m_entries[m_collisionEntryCount++] = CollisionEntry(kclTypeBit, attribute, dist);
}

/// @brief Helper function that pushes a collision entry for a @ref KColType with no additional
/// attribute bits set
/// @param dist How far the colliding body penetrates the tri
/// @param typeMask Out parameter that is updated to include the bit for `type`
/// @param type The base type of the tri we are colliding with
/// @details This function is added in Kinoko to prevent callers from having to cast the @ref
/// KColType on each push.
void CollisionDirector::pushCollisionEntry(f32 dist, KCLTypeMask *typeMask, KColType type) {
    pushCollisionEntry(dist, typeMask, static_cast<u16>(type));
}

/// @addr{0x807BD96C}
/// @brief Finds the closest KCL triangle out of the list of tris we are colliding with (i.e. the
/// deepest penetration)
/// @param type Filters the result for particular KCL types
/// @return A pointer to the closest collision entry matching `type`, or `nullptr` if there is none
/// @details Unlike the base game, which caches the result in a member and returns a bool, Kinoko
/// returns the entry directly.
const CollisionDirector::CollisionEntry *CollisionDirector::findClosestCollisionEntry(
        KCLTypeMask type) const {
    const CollisionEntry *closest = nullptr;
    f32 minDist = -std::numeric_limits<f32>::min();

    for (size_t i = 0; i < m_collisionEntryCount; ++i) {
        const auto &entry = m_entries[i];
        u32 typeMask = entry.typeMask & type;
        if (typeMask != 0 && entry.dist > minDist) {
            minDist = entry.dist;
            closest = &entry;
        }
    }

    return closest;
}

/// @addr{0x8078DFE8}
/// @brief Creates the singleton instance of the @ref CollisionDirector
/// @return A pointer to the newly created @ref CollisionDirector instance
/// @pre The singleton instance of @ref CollisionDirector must not already exist.
CollisionDirector *CollisionDirector::CreateInstance() {
    ASSERT(!s_instance);
    s_instance = EGG::egg_new<CollisionDirector>();
    return s_instance;
}

/// @addr{0x8078E124}
/// @brief Destroys the singleton instance of the @ref CollisionDirector
/// @pre The singleton instance of @ref CollisionDirector must already exist.
void CollisionDirector::DestroyInstance() {
    ASSERT(s_instance);
    auto *instance = s_instance;
    s_instance = nullptr;
    EGG::egg_delete(instance);
}

/// @addr{0x8078E33C}
/// @brief Private constructor
/// @details Initializes @ref m_collisionEntryCount to zero, then creates and initializes the @ref
/// CourseColMgr singleton.
CollisionDirector::CollisionDirector() {
    m_collisionEntryCount = 0;
    CourseColMgr::CreateInstance()->init();
}

/// @addr{0x8078E454}
/// @brief Private virtual destructor that also destroys the @ref CourseColMgr singleton
CollisionDirector::~CollisionDirector() {
    if (s_instance) {
        s_instance = nullptr;
        WARN("CollisionDirector instance not explicitly handled!");
    }

    CourseColMgr::DestroyInstance();
}

/// @brief Helper function which contains frequently re-used code shared by the `checkSphere*`
/// family of collision check functions
/// @tparam T The collision info object type, either @ref CollisionInfoPartial or @ref CollisionInfo
/// @param radius The radius of the sphere to check
/// @param pos The position of the sphere to check
/// @param prevPos The previous position of the sphere, used for calculating collision depth
/// @param mask The KCL flags to check collision against (other types are ignored)
/// @param info Out parameter of type `T` for retrieving collision information (if any)
/// @param maskOut The KCL flags that were hit during the collision check (if any)
/// @param timeOffset Optional time delta
/// @param courseCheckFunc The @ref CourseColMgr member function used for the course KCL check
/// @param drivableCheckFunc The @ref ObjectDrivableDirector member function used for the object
/// check
/// @param cached Whether the check should use the local spatial cache for the course KCL check
/// @param push Whether to push a collision entry into the @ref CollisionDirector cache
/// @return Whether a collision was detected
/// @details Resets `info` to its default state before performing the collision check. If `maskOut`
/// is provided, then it is reset to `KCL_NONE`; additionally, if `push` was also `true`, the @ref
/// CollisionDirector cache is reset as well. Resets @ref CourseColMgr::m_noBounceWallInfo to its
/// default state if it exists.
///
/// Performs a @ref CourseColMgr collision check if this is a cached query or if this is a
/// non-cached query having `mask` not equal to `KCL_NONE`. Regardless of the return of the course
/// KCL check, always performs a @ref ObjectDrivableDirector collision check. If at least one of the
/// two collision checks returned `true`, then `info` and @ref CourseColMgr::m_noBounceWallInfo are
/// updated appropriately (if they are not `nullptr`). Finally, the @ref
/// CourseColMgr::m_noBounceWallInfo is cleared if present, and the function returns a boolean
/// indicating whether or not a collision has occurred.
template <typename T>
    requires std::is_same_v<T, CollisionInfo> || std::is_same_v<T, CollisionInfoPartial>
bool CollisionDirector::checkSphereImpl(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, T *info, KCLTypeMask *maskOut,
        u32 timeOffset, CourseColCheckFunc<T> courseCheckFunc,
        DrivableColCheckFunc<T> drivableCheckFunc, bool cached, bool push) {
    if (info) {
        info->reset();
    }

    if (maskOut) {
        if (push) {
            resetCollisionEntries(maskOut);
        } else {
            *maskOut = KCL_NONE;
        }
    }

    auto *courseColMgr = CourseColMgr::Instance();
    auto *noBounceInfo = courseColMgr->noBounceWallInfo();
    if (noBounceInfo) {
        noBounceInfo->bbox.setZero();
        noBounceInfo->dist = std::numeric_limits<f32>::min();
    }

    // Cached checks always query, uncached checks skip the query entirely when mask is empty
    bool colliding = (mask || cached) &&
            (courseColMgr->*courseCheckFunc)(1.0f, radius, nullptr, pos, prevPos, mask, info,
                    maskOut);

    colliding |= (ObjectDrivableDirector::Instance()->*drivableCheckFunc)(radius, pos, prevPos,
            mask, info, maskOut, timeOffset);

    if (colliding) {
        if (info) {
            info->tangentOff = info->bbox.min + info->bbox.max;
        }

        if (noBounceInfo) {
            noBounceInfo->tangentOff = noBounceInfo->bbox.min + noBounceInfo->bbox.max;
        }
    }

    courseColMgr->clearNoBounceWallInfo();

    return colliding;
}

CollisionDirector *CollisionDirector::s_instance = nullptr;

} // namespace Kinoko::Field
