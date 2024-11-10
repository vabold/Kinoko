#include "ObjectCollisionBox.hh"

namespace Field {

/// @addr{0x80833840}
ObjectCollisionBox::ObjectCollisionBox(f32 x, f32 y, f32 z, const EGG::Vector3f &center)
    : ObjectCollisionConvexHull(8), m_dimensions(x, y, z), m_center(center),
      m_scale(1.0f, 1.0f, 1.0f) {
    // TODO: define points of the convex hull
}

/// @addr{0x808342F0}
ObjectCollisionBox::~ObjectCollisionBox() = default;

/// @addr{0x80833EEC}
void ObjectCollisionBox::transform(const EGG::Matrix34f & /*mat*/, const EGG::Vector3f & /*scale*/,
        const EGG::Vector3f & /*speed*/) {
    ; // TODO
}

} // namespace Field
