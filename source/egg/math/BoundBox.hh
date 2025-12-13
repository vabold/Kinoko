#pragma once

#include "egg/math/Vector.hh"

namespace EGG {

/// @brief A representation of a bounding rectangle.
struct BoundBox2f {
    /// @addr{0x802145C0}
    constexpr BoundBox2f() = default;
    constexpr ~BoundBox2f() = default;

    /// @addr{0x802145F0}
    constexpr void resetBound() {
        min.set(std::numeric_limits<f32>::max());
        max.set(-std::numeric_limits<f32>::max());
    }

    constexpr void setDirect(const Vector2f &vMin, const Vector2f &vMax) {
        max = vMax;
        min = vMin;
    }

    constexpr void setMin(const Vector2f &v) {
        min = v;
    }

    constexpr void setMax(const Vector2f &v) {
        max = v;
    }

    Vector2f min;
    Vector2f max;
};

/// @brief A representation of a bounding cuboid.
struct BoundBox3f {
    constexpr BoundBox3f() = default;
    constexpr ~BoundBox3f() = default;

    constexpr void resetBound() {
        min.set(std::numeric_limits<f32>::max());
        max.set(-std::numeric_limits<f32>::max());
    }

    constexpr void setZero() {
        min.setZero();
        max.setZero();
    }

    constexpr void setDirect(const Vector3f &vMin, const Vector3f &vMax) {
        max = vMax;
        min = vMin;
    }

    constexpr void setMin(const Vector3f &v) {
        min = v;
    }

    constexpr void setMax(const Vector3f &v) {
        max = v;
    }

    Vector3f min;
    Vector3f max;
};

} // namespace EGG
