#pragma once

#include "game/field/ObjectCollisionBase.hh"

namespace Field {

/// @brief Smallest convex shape that encloses a given set of points.
class ObjectCollisionConvexHull : public ObjectCollisionBase {
public:
    ObjectCollisionConvexHull(const std::span<const EGG::Vector3f> &points);
    ~ObjectCollisionConvexHull() override;

    f32 getBoundingRadius() const override;
    const EGG::Vector3f &getSupport(const EGG::Vector3f &v) const override;

protected:
    ObjectCollisionConvexHull(size_t count);

private:
    std::span<EGG::Vector3f> m_points;
    std::span<EGG::Vector3f> m_worldPoints;
    f32 m_worldRadius;
};

} // namespace Field
