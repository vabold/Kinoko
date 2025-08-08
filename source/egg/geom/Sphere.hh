#pragma once

#include "egg/math/Vector.hh"

namespace EGG {

/// @brief Represents a sphere in 3D space.
struct Sphere3f {
    constexpr Sphere3f(const Vector3f &v, f32 r) : pos(v), radius(r) {}

    /// @addr{0x8051A07C}
    /// @brief Not actually part of EGG. Included here for compartmentalization.
    /// @return True if this sphere is completely inside rhs.
    [[nodiscard]] constexpr bool isInsideOtherSphere(const Sphere3f &rhs) const {
        f32 radiusDiff = rhs.radius - radius;
        if (radiusDiff < 0.0f) {
            return false;
        }

        return rhs.pos.ps_sqDistance(this->pos) < radiusDiff * radiusDiff;
    }

    Vector3f pos;
    f32 radius;
};

} // namespace EGG
