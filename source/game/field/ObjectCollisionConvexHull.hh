#pragma once

#include "game/field/ObjectCollisionBase.hh"

namespace Field {

/// @brief Smallest convex shape that encloses a given set of points.
class ObjectCollisionConvexHull : public ObjectCollisionBase {
public:
    ObjectCollisionConvexHull(const std::span<const EGG::Vector3f> &points);
    ~ObjectCollisionConvexHull() override;

    void transform(const EGG::Matrix34f &mat, const EGG::Vector3f &scale,
            const EGG::Vector3f &speed) override;
    const EGG::Vector3f &getSupport(const EGG::Vector3f &v) const override;
    f32 getBoundingRadius() const override;

    virtual void setBoundingRadius(f32 val);

protected:
    ObjectCollisionConvexHull(size_t count);

private:
    std::span<EGG::Vector3f> m_points;
    std::span<EGG::Vector3f> m_worldPoints;
    f32 m_worldRadius;
};

} // namespace Field
