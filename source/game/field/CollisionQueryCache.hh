#pragma once

#include "game/field/KColData.hh"

namespace Kinoko::Field {

/// @brief Performs @ref CollisionDirector::checkSphereFull() queries, reusing the result of the
/// course KCL check when its inputs are unchanged
/// @details Kinoko-specific optimization. Some objects, such as Goombas resting on the floor,
/// perform the exact same collision check every frame. The course KCL never changes, so an
/// identical course KCL check always produces the same collision info and has no lasting side
/// effects. The check can therefore be skipped and its previous result is reused. Drivable objects
/// can move, so they are still checked every time.
class CollisionQueryCache {
public:
    CollisionQueryCache();
    ~CollisionQueryCache();

    [[nodiscard]] bool checkSphereFull(f32 radius, const EGG::Vector3f &pos, KCLTypeMask mask,
            CollisionInfo *info);

private:
    bool m_valid;               ///< Whether the members below hold a previous course KCL check
    f32 m_radius;               ///< The radius of the previous course KCL check
    EGG::Vector3f m_pos;        ///< The position of the previous course KCL check
    KCLTypeMask m_mask;         ///< The KCL type mask of the previous course KCL check
    bool m_courseHasCol;        ///< Whether the previous course KCL check found a collision
    CollisionInfo m_courseInfo; ///< The collision info written by the previous course KCL check

    [[nodiscard]] static bool IsBitwiseEqual(f32 lhs, f32 rhs);
    [[nodiscard]] static bool IsBitwiseEqual(const EGG::Vector3f &lhs, const EGG::Vector3f &rhs);
};

} // namespace Kinoko::Field
