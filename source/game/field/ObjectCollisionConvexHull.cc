#include "ObjectCollisionConvexHull.hh"

namespace Field {

/// @addr{0x808364E0}
/// @brief Creates a convex hull with the provided points.
/// @details The base game has the possibility to only provide a count to allocate space.
/// To account for this, we split the base game's constructor into two overloads.
/// This overload enables conversion from std::array into a span, which initializes the points.
ObjectCollisionConvexHull::ObjectCollisionConvexHull(const std::span<EGG::Vector3f> &points)
    : ObjectCollisionConvexHull(points.size()) {
    for (size_t i = 0; i < points.size(); ++i) {
        m_points[i] = points[i];
    }
}

/// @addr{0x808365A8}
ObjectCollisionConvexHull::~ObjectCollisionConvexHull() {
    delete[] m_points.data();
    delete[] m_worldPoints.data();
}

/// @addr{0x808364E0}
/// @brief Allocates space for a convex hull with the provided point count.
/// This overload can only be called via inheritance or delegating constructors.
/// @details The base game has the possibility to only provide a count to allocate space.
/// To account for this, we split the base game's constructor into two overloads.
/// This overload enables avoiding immediate point initialization, which is useful for inheritance.
ObjectCollisionConvexHull::ObjectCollisionConvexHull(size_t count) {
    m_points = std::span<EGG::Vector3f>(new EGG::Vector3f[count], count);
    m_worldPoints = std::span<EGG::Vector3f>(new EGG::Vector3f[count], count);
}

} // namespace Field
