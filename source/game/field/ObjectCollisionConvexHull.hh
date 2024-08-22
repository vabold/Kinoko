#pragma once

#include "game/field/ObjectCollisionBase.hh"

namespace Field {

/// @brief Smallest convex shape that encloses a given set of points.
class ObjectCollisionConvexHull : public ObjectCollisionBase {
public:
    ObjectCollisionConvexHull(const std::span<EGG::Vector3f> &points);
    ~ObjectCollisionConvexHull() override;

protected:
    ObjectCollisionConvexHull(size_t count);

private:
    std::span<EGG::Vector3f> m_points;
    std::span<EGG::Vector3f> m_worldPoints;
};

} // namespace Field
