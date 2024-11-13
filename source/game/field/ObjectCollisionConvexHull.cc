#include "ObjectCollisionConvexHull.hh"

namespace Field {

/// @addr{0x808364E0}
/// @brief Creates a convex hull with the provided points.
/// @details The base game has the possibility to only provide a count to allocate space.
/// To account for this, we split the base game's constructor into two overloads.
/// This overload enables conversion from std::array into a span, which initializes the points.
ObjectCollisionConvexHull::ObjectCollisionConvexHull(const std::span<const EGG::Vector3f> &points)
    : ObjectCollisionConvexHull(points.size()) {
    m_worldRadius = 70.0f;

    for (size_t i = 0; i < points.size(); ++i) {
        m_points[i] = points[i];
    }
}

/// @addr{0x808365A8}
ObjectCollisionConvexHull::~ObjectCollisionConvexHull() {
    delete[] m_points.data();
    delete[] m_worldPoints.data();
}

/// @addr{0x808367C4}
void ObjectCollisionConvexHull::transform(const EGG::Matrix34f &mat, const EGG::Vector3f &scale,
        const EGG::Vector3f &speed) {
    m_translation = speed;

    if (scale.x == 0.0f) {
        for (size_t i = 0; i < m_points.size(); ++i) {
            m_worldPoints[i] = mat.multVector(m_points[i]);
        }
    } else {
        EGG::Matrix34f temp;
        temp.makeS(scale);
        temp = mat.multiplyTo(temp);

        for (size_t i = 0; i < m_points.size(); ++i) {
            m_worldPoints[i] = temp.multVector(m_points[i]);
        }
    }
}

/// @addr{0x80836628}
const EGG::Vector3f &ObjectCollisionConvexHull::getSupport(const EGG::Vector3f &v) const {
    const EGG::Vector3f *result = &m_worldPoints[0];
    f32 maxDot = v.dot(*result);

    for (size_t i = 1; i < m_worldPoints.size(); ++i) {
        const auto &iter = m_worldPoints[i];
        f32 iterDot = v.dot(iter);

        if (maxDot < iterDot) {
            result = &iter;
            maxDot = iterDot;
        }
    }

    return *result;
}

/// @addr{0x807F957C}
f32 ObjectCollisionConvexHull::getBoundingRadius() const {
    return m_worldRadius;
}

/// @addr{0x8080C414}
void ObjectCollisionConvexHull::setBoundingRadius(f32 val) {
    m_worldRadius = val;
}

/// @addr{0x808364E0}
/// @brief Allocates space for a convex hull with the provided point count.
/// This overload can only be called via inheritance or delegating constructors.
/// @details The base game has the possibility to only provide a count to allocate space.
/// To account for this, we split the base game's constructor into two overloads.
/// This overload enables avoiding immediate point initialization, which is useful for inheritance.
ObjectCollisionConvexHull::ObjectCollisionConvexHull(size_t count) {
    ASSERT(count < 0x100);

    m_points = std::span<EGG::Vector3f>(new EGG::Vector3f[count], count);
    m_worldPoints = std::span<EGG::Vector3f>(new EGG::Vector3f[count], count);
}

} // namespace Field
