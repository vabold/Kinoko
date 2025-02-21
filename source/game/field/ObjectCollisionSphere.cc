#include "ObjectCollisionSphere.hh"

namespace Field {

/// @addr{0x808368D0}
ObjectCollisionSphere::ObjectCollisionSphere(f32 radius, const EGG::Vector3f &center)
    : m_radius(radius), m_pos(center), m_scaledRadius(radius), m_scaledPos(center),
      m_worldPos(center) {}

/// @addr{0x80836B5C}
ObjectCollisionSphere::~ObjectCollisionSphere() = default;

/// @addr{0x80836A50}
void ObjectCollisionSphere::transform(const EGG::Matrix34f &mat, const EGG::Vector3f &scale,
        const EGG::Vector3f &speed) {
    m_translation = speed;

    if (scale.x != 1.0f) {
        m_scaledPos = m_pos * scale.x;
        m_scaledRadius = m_radius * scale.x;
    }

    m_worldPos = mat.multVector(m_scaledPos);
    m_center = m_worldPos - speed;
}

/// @addr{0x80836920}
const EGG::Vector3f &ObjectCollisionSphere::getSupport(const EGG::Vector3f &v) const {
    // if (!m_hasColDir)
    //      return m_worldPos;

    return m_worldPos.dot(v) > m_center.dot(v) ? m_worldPos : m_center;
}

} // namespace Field
