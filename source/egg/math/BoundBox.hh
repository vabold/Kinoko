#pragma once

#include "egg/math/Vector.hh"

namespace EGG {

/// @brief A representation of a bounding rectangle.
struct BoundBox2f {
    BoundBox2f();
    ~BoundBox2f();

    void resetBound();
    void setDirect(const Vector2f &vMax, const Vector2f &vMin);
    void setMin(const Vector2f &v);
    void setMax(const Vector2f &v);

    Vector2f min;
    Vector2f max;
};

/// @brief A representation of a bounding cuboid.
struct BoundBox3f {
    BoundBox3f();
    ~BoundBox3f();

    void resetBound();
    void setZero();
    void setDirect(const Vector3f &vMin, const Vector3f &vMax);
    void setMin(const Vector3f &V);
    void setMax(const Vector3f &v);

    Vector3f min;
    Vector3f max;
};

} // namespace EGG
