#include "CollisionQueryCache.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/CourseColMgr.hh"
#include "game/field/ObjectDrivableDirector.hh"

#include <cstring>

namespace Kinoko::Field {

/// @brief Constructor
/// @details Initializes the cache to hold no previous course KCL check.
CollisionQueryCache::CollisionQueryCache() : m_valid(false) {}

/// @brief Default destructor
CollisionQueryCache::~CollisionQueryCache() = default;

/// @brief Equivalent to @ref CollisionDirector::checkSphereFull() with an infinite previous
/// position, no output mask, and no time offset
/// @param radius The radius of the sphere to check
/// @param pos The position of the sphere to check
/// @param mask The KCL flags to check collision against (other types are ignored)
/// @param info Out parameter for retrieving collision information
/// @return Whether a collision was detected
/// @details Follows the same steps as @ref CollisionDirector::checkSphereImpl(). If `radius`,
/// `pos`, and `mask` are bitwise identical to the previous course KCL check, then the previous
/// check's collision info and result are reused rather than checking the course KCL again. Drivable
/// objects are always checked. Falls back to @ref CollisionDirector::checkSphereFull() if @ref
/// CourseColMgr::m_noBounceWallInfo is set, which only happens during the kart's own collision
/// checks.
bool CollisionQueryCache::checkSphereFull(f32 radius, const EGG::Vector3f &pos, KCLTypeMask mask,
        CollisionInfo *info) {
    auto *courseColMgr = CourseColMgr::Instance();

    if (courseColMgr->noBounceWallInfo()) {
        return CollisionDirector::Instance()->checkSphereFull(radius, pos, EGG::Vector3f::inf, mask,
                info, nullptr, 0);
    }

    bool colliding;

    if (m_valid && m_mask == mask && IsBitwiseEqual(m_radius, radius) &&
            IsBitwiseEqual(m_pos, pos)) {
        *info = m_courseInfo;
        colliding = m_courseHasCol;
    } else {
        info->reset();
        colliding = mask &&
                courseColMgr->checkSphereFull(1.0f, radius, nullptr, pos, EGG::Vector3f::inf, mask,
                        info, nullptr);

        m_valid = true;
        m_radius = radius;
        m_pos = pos;
        m_mask = mask;
        m_courseHasCol = colliding;
        m_courseInfo = *info;
    }

    colliding |= ObjectDrivableDirector::Instance()->checkSphereFull(radius, pos,
            EGG::Vector3f::inf, mask, info, nullptr, 0);

    if (colliding) {
        info->tangentOff = info->bbox.min + info->bbox.max;
    }

    return colliding;
}

/// @brief Checks whether two floats have identical bits
/// @details Unlike `operator==`, this distinguishes `-0.0f` from `0.0f`, since a collision check
/// with one may not produce bitwise identical results to a check with the other.
bool CollisionQueryCache::IsBitwiseEqual(f32 lhs, f32 rhs) {
    return std::memcmp(&lhs, &rhs, sizeof(f32)) == 0;
}

/// @brief Checks whether two vectors have bitwise identical components
bool CollisionQueryCache::IsBitwiseEqual(const EGG::Vector3f &lhs, const EGG::Vector3f &rhs) {
    return std::memcmp(&lhs, &rhs, sizeof(EGG::Vector3f)) == 0;
}

} // namespace Kinoko::Field
