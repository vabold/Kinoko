#include "BoundBox.hh"

#include "egg/math/Math.hh"

namespace EGG {

/// @addr{0x802145F0}
void BoundBox2f::resetBound() {
    min.set(std::numeric_limits<f32>::max());
    max.set(-std::numeric_limits<f32>::max());
}

void BoundBox2f::setDirect(const Vector2f &vMin, const Vector2f &vMax) {
    max = vMax;
    min = vMin;
}

void BoundBox3f::resetBound() {
    min.set(std::numeric_limits<f32>::max());
    max.set(-std::numeric_limits<f32>::max());
}

void BoundBox3f::setDirect(const Vector3f &vMin, const Vector3f &vMax) {
    max = vMax;
    min = vMin;
}

} // namespace EGG
