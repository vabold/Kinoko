#pragma once

#include "egg/math/Vector.hh"

namespace EGG {

/// @brief Represents a plane in 3D space.
struct Plane3f {
    /// @addr{0x805AF020}
    Plane3f() = default;

    /// @addr{0x805AEF6C}
    Plane3f(const Vector3f &point, const Vector3f &normal) {
        set(point, normal);
    }

    /// @addr{0x805AF330}
    ~Plane3f() = default;

    /// @addr{0x805AF048}
    void set(const Vector3f &point, const Vector3f &normal) {
        n = normal;
        d = -(normal.dot(point));
    }

    /// @addr{0x805AF0F0}
    bool testPoint(const Vector3f &point) const {
        return d + n.dot(point) <= 0.0f;
    }

    Vector3f n;
    f32 d;
};

} // namespace EGG
