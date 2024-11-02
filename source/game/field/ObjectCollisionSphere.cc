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

/// @addr{0x80836B54}
f32 ObjectCollisionSphere::getBoundingRadius() const {
    return m_worldRadius;
}

/// @addr{0x80836920}
const EGG::Vector3f &ObjectCollisionSphere::getSupport(const EGG::Vector3f &v) const {
    // if (m_hasColDir)
    //      return m_worldPos;

    return m_worldPos.dot(v) > m_center.dot(v) ? m_worldPos : m_center;
}

} // namespace Field
