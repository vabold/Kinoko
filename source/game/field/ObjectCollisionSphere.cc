#include "ObjectCollisionSphere.hh"

namespace Field {

/// @addr{0x808368D0}
ObjectCollisionSphere::ObjectCollisionSphere(f32 radius, const EGG::Vector3f &center)
    : m_radius(radius), m_pos(center) {
    m_worldRadius = radius;
    m_worldPos = center;

    m_center = center;
}

/// @addr{0x80836B5C}
ObjectCollisionSphere::~ObjectCollisionSphere() = default;

} // namespace Field
