#include "Sphere.hh"

namespace EGG {

Sphere3f::Sphere3f(const EGG::Vector3f &vec, f32 radius) : pos(vec), radius(radius) {}

bool Sphere3f::isInsideOtherSphere(const Sphere3f &rhs) const {
    f32 radiusDiff = rhs.radius - radius;
    if (radiusDiff < 0.0f) {
        return false;
    }

    return rhs.pos.ps_distance(this->pos) < radiusDiff * radiusDiff;
}

} // namespace EGG
