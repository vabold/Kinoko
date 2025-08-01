#pragma once

#include "game/field/ObjectCollisionConvexHull.hh"

namespace Field {

/// @details Inherits ObjectCollisionConvexHull, as a box is a convex hull of its vertices.
class ObjectCollisionBox : public ObjectCollisionConvexHull {
public:
    ObjectCollisionBox(f32 x, f32 y, f32 z, const EGG::Vector3f &center);
    ~ObjectCollisionBox() override;

    void transform(const EGG::Matrix34f &mat, const EGG::Vector3f &scale,
            const EGG::Vector3f &speed) override;

private:
    EGG::Vector3f m_dimensions;
    [[maybe_unused]] EGG::Vector3f m_center;
    EGG::Vector3f m_scale;
};

} // namespace Field
