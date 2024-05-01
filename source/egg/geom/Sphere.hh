#pragma once

#include "egg/math/Vector.hh"

namespace EGG {

/// @brief Represents a sphere in 3D space.
struct Sphere3f {
    Sphere3f(const Vector3f &v, f32 r);

    [[nodiscard]] bool isInsideOtherSphere(const Sphere3f &rhs) const;

    Vector3f pos;
    f32 radius;
};

} // namespace EGG
