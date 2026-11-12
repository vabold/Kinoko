#include "CollisionInfo.hh"

namespace Kinoko::Field {

/// @brief Initializes the @ref bbox "bounding box" to zero
void CollisionInfoPartial::reset() {
    bbox.setZero();
}

/// @brief Expands the @ref bbox "bounding box" to include the provided position vector
/// @param offset The position offset to include in the bounding box
void CollisionInfoPartial::updateBBox(const EGG::Vector3f &offset) {
    bbox.min = bbox.min.minimize(offset);
    bbox.max = bbox.max.maximize(offset);
}

/// @brief Maps the provided collision info from local to world space based on the provided
/// transformation matrix
/// @param rhs The partial collision info accumulated from local space
/// @param mtx The local-to-world transformation matrix
/// @note OPT: In the base game, this function also updates the bounding box of `rhs` to reflect
/// world space. However, in Kinoko, all uses of this function pass a temporary `rhs` object which
/// is not used after this call. As such, we can remove the bounding box update and thus make `rhs`
/// a const reference. This likely doesn't improve performance, but should help readability.
void CollisionInfoPartial::transformInfo(const CollisionInfoPartial &rhs,
        const EGG::Matrix34f &mtx) {
    EGG::Vector3f min = mtx.ps_multVector33(rhs.bbox.min);
    EGG::Vector3f max = mtx.ps_multVector33(rhs.bbox.max);

    bbox.min = bbox.min.minimize(min.minimize(max));
    bbox.max = bbox.max.maximize(min.maximize(max));
}

/// @brief Expands the @ref bbox "bounding box" to include the provided position vector
/// @param offset The position offset to include in the bounding box
void CollisionInfo::updateBBox(const EGG::Vector3f &offset) {
    bbox.min = bbox.min.minimize(offset);
    bbox.max = bbox.max.maximize(offset);
}

/// @brief Updates the floor collision info if the provided distance is closer than the
/// currently tracked floor collision
/// @param dist Distance from the colliding floor
/// @param fnrm Colliding floor's up vector
void CollisionInfo::updateFloor(f32 dist, const EGG::Vector3f &fnrm) {
    if (dist > floorDist) {
        floorDist = dist;
        floorNrm = fnrm;
    }
}

/// @brief Updates the wall collision info if the provided distance is closer than the
/// currently tracked wall collision
/// @param dist Distance from the colliding wall
/// @param fnrm Colliding wall's up vector
void CollisionInfo::updateWall(f32 dist, const EGG::Vector3f &fnrm) {
    if (dist > wallDist) {
        wallDist = dist;
        wallNrm = fnrm;
    }
}

/// @brief Sets all members to their default values
void CollisionInfo::reset() {
    bbox.setZero();
    movingFloorDist = -std::numeric_limits<f32>::min();
    wallDist = -std::numeric_limits<f32>::min();
    floorDist = -std::numeric_limits<f32>::min();
    perpendicularity = 0.0f;
}

/// @brief Updates members to reflect a new collision
/// @param dist Distance from the colliding tri
/// @param offset Position offset from the colliding tri
/// @param fnrm Colliding tri face's up vector
/// @param mask KCL base type of the colliding tri
void CollisionInfo::update(f32 dist, const EGG::Vector3f &offset, const EGG::Vector3f &fnrm,
        KCLTypeMask mask) {
    bbox.min = bbox.min.minimize(offset);
    bbox.max = bbox.max.maximize(offset);

    if (mask & KCL_TYPE_FLOOR) {
        updateFloor(dist, fnrm);
    } else if (mask & KCL_TYPE_WALL) {
        if (wallDist > -std::numeric_limits<f32>::min()) {
            f32 dot = 1.0f - wallNrm.ps_dot(fnrm);
            if (dot > perpendicularity) {
                perpendicularity = std::min(dot, 1.0f);
            }
        }

        updateWall(dist, fnrm);
    }
}

/// @addr{0x807C26AC}
/// @brief Maps the provided collision info from local to world space based on the provided
/// transformation matrix
/// @param rhs The collision info accumulated from local space
/// @param mtx The local-to-world transformation matrix
/// @param v Moving road velocity of the colliding object, if applicable
/// @note OPT: In the base game, this function also updates the bounding box of `rhs` to reflect
/// world space. However, in Kinoko, all uses of this function pass a temporary `rhs` object which
/// is not used after this call. As such, we can remove the bounding box update and thus make `rhs`
/// a const reference. This likely doesn't improve performance, but should help readability.
void CollisionInfo::transformInfo(const CollisionInfo &rhs, const EGG::Matrix34f &mtx,
        const EGG::Vector3f &v) {
    EGG::Vector3f min = mtx.ps_multVector33(rhs.bbox.min);
    EGG::Vector3f max = mtx.ps_multVector33(rhs.bbox.max);

    bbox.min = bbox.min.minimize(min.minimize(max));
    bbox.max = bbox.max.maximize(min.maximize(max));

    if (floorDist < rhs.floorDist) {
        floorDist = rhs.floorDist;
        floorNrm = mtx.ps_multVector33(rhs.floorNrm);
    }

    if (wallDist < rhs.wallDist) {
        wallDist = rhs.wallDist;
        wallNrm = mtx.ps_multVector33(rhs.wallNrm);
    }

    if (movingFloorDist < rhs.floorDist) {
        movingFloorDist = rhs.floorDist;
        roadVelocity = v;
    }

    perpendicularity = std::max(perpendicularity, rhs.perpendicularity);
}

} // namespace Kinoko::Field
