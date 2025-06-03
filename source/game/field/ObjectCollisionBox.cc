#include "ObjectCollisionBox.hh"

namespace Field {

/// @addr{0x80833840}
ObjectCollisionBox::ObjectCollisionBox(f32 x, f32 y, f32 z, const EGG::Vector3f &center)
    : ObjectCollisionConvexHull(8), m_dimensions(x, y, z), m_center(center),
      m_scale(EGG::Vector3f::unit) {
    f32 radius = getBoundingRadius();
    EGG::Vector3f scaledDims = m_dimensions - EGG::Vector3f(radius, radius, radius);

    m_points[0].x = m_center.x + scaledDims.x;
    m_points[0].y = m_center.y + 2.0f * scaledDims.y;
    m_points[0].z = m_center.z + scaledDims.z;

    m_points[1].x = m_center.x + scaledDims.x;
    m_points[1].y = m_center.y + 2.0f * scaledDims.y;
    m_points[1].z = m_center.z - scaledDims.z;

    m_points[2].x = m_center.x + scaledDims.x;
    m_points[2].y = m_center.y - 2.0f * scaledDims.y;
    m_points[2].z = m_center.z + scaledDims.z;

    m_points[3].x = m_center.x - scaledDims.x;
    m_points[3].y = m_center.y + 2.0f * scaledDims.y;
    m_points[3].z = m_center.z + scaledDims.z;

    m_points[4].x = m_center.x + scaledDims.x;
    m_points[4].y = m_center.y - 2.0f * scaledDims.y;
    m_points[4].z = m_center.z - scaledDims.z;

    m_points[5].x = m_center.x - scaledDims.x;
    m_points[5].y = m_center.y + 2.0f * scaledDims.y;
    m_points[5].z = m_center.z - scaledDims.z;

    m_points[6].x = m_center.x - scaledDims.x;
    m_points[6].y = m_center.y - 2.0f * scaledDims.y;
    m_points[6].z = m_center.z + scaledDims.z;

    m_points[7].x = m_center.x - scaledDims.x;
    m_points[7].y = m_center.y - 2.0f * scaledDims.y;
    m_points[7].z = m_center.z - scaledDims.z;
}

/// @addr{0x808342F0}
ObjectCollisionBox::~ObjectCollisionBox() = default;

/// @addr{0x80833EEC}
void ObjectCollisionBox::transform(const EGG::Matrix34f &mat, const EGG::Vector3f &scale,
        const EGG::Vector3f &speed) {
    f32 radius = getBoundingRadius();
    m_scale = scale;

    EGG::Vector3f scaledDims = (m_dimensions * m_scale) - EGG::Vector3f(radius, radius, radius);

    m_points[0].x = m_center.x + scaledDims.x;
    m_points[0].y = m_center.y + 2.0f * scaledDims.y;
    m_points[0].z = m_center.z + scaledDims.z;

    m_points[1].x = m_center.x + scaledDims.x;
    m_points[1].y = m_center.y + 2.0f * scaledDims.y;
    m_points[1].z = m_center.z - scaledDims.z;

    m_points[2].x = m_center.x + scaledDims.x;
    m_points[2].y = m_center.y - 2.0f * scaledDims.y;
    m_points[2].z = m_center.z + scaledDims.z;

    m_points[3].x = m_center.x - scaledDims.x;
    m_points[3].y = m_center.y + 2.0f * scaledDims.y;
    m_points[3].z = m_center.z + scaledDims.z;

    m_points[4].x = m_center.x + scaledDims.x;
    m_points[4].y = m_center.y - 2.0f * scaledDims.y;
    m_points[4].z = m_center.z - scaledDims.z;

    m_points[5].x = m_center.x - scaledDims.x;
    m_points[5].y = m_center.y + 2.0f * scaledDims.y;
    m_points[5].z = m_center.z - scaledDims.z;

    m_points[6].x = m_center.x - scaledDims.x;
    m_points[6].y = m_center.y - 2.0f * scaledDims.y;
    m_points[6].z = m_center.z + scaledDims.z;

    m_points[7].x = m_center.x - scaledDims.x;
    m_points[7].y = m_center.y - 2.0f * scaledDims.y;
    m_points[7].z = m_center.z - scaledDims.z;

    ObjectCollisionConvexHull::transform(mat, EGG::Vector3f::unit, speed);
}

} // namespace Field
