#pragma once

#include "egg/math/Vector.hh"

namespace EGG {

/// @brief A representation of a bounding rectangle.
struct BoundBox2f {
    /// @addr{0x802145C0}
    BoundBox2f() = default;
    ~BoundBox2f() = default;

    void resetBound();

    void setDirect(const Vector2f &vMax, const Vector2f &vMin);

    void setMin(const Vector2f &v) {
        min = v;
    }

    void setMax(const Vector2f &v) {
        max = v;
    }

    Vector2f min;
    Vector2f max;
};

/// @brief A representation of a bounding cuboid.
struct BoundBox3f {
    BoundBox3f() = default;
    ~BoundBox3f() = default;

    void resetBound();

    void setZero() {
        min.setZero();
        max.setZero();
    }

    void setDirect(const Vector3f &vMin, const Vector3f &vMax);

    void setMin(const Vector3f &v) {
        min = v;
    }

    void setMax(const Vector3f &v) {
        max = v;
    }

    Vector3f min;
    Vector3f max;
};

} // namespace EGG
