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

/// @addr{0x80836334}
void ObjectCollisionCylinder::transform(const EGG::Matrix34f &mat, const EGG::Vector3f &scale,
        const EGG::Vector3f &speed) {
    m_translation = speed;
    m_worldPos = m_pos * scale.x;
    m_worldHeight = m_height * scale.y;
    m_worldRadius = m_radius * scale.x;

    m_center = mat.ps_multVector(m_worldPos);
    m_top = mat.ps_multVector(m_worldPos + EGG::Vector3f::ey * m_worldHeight);
    m_bottom = mat.ps_multVector(m_worldPos - EGG::Vector3f::ey * m_worldHeight);
}

} // namespace Field
