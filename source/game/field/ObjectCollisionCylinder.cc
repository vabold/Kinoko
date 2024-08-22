#include "ObjectCollisionCylinder.hh"

namespace Field {

/// @addr{0x80836068}
ObjectCollisionCylinder::ObjectCollisionCylinder(f32 radius, f32 height,
        const EGG::Vector3f &center)
    : m_radius(radius), m_height(height), m_pos(center) {
    m_worldRadius = radius;
    m_worldHeight = height;
    m_worldPos = center;

    m_center = center;
    m_top = center + EGG::Vector3f::ey * height;
    m_bottom = center - EGG::Vector3f::ey * height;
}

/// @addr{0x808364A0}
ObjectCollisionCylinder::~ObjectCollisionCylinder() = default;

} // namespace Field
