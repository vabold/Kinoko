#include "BoundBox.hh"

#include "egg/math/Math.hh"

namespace EGG {

BoundBox2f::BoundBox2f() {
    resetBound();
}

BoundBox2f::~BoundBox2f() = default;

void BoundBox2f::resetBound() {
    min.set(std::numeric_limits<f32>::max());
    max.set(-std::numeric_limits<f32>::max());
}

void BoundBox2f::setDirect(const Vector2f &vMin, const Vector2f &vMax) {
    max = vMax;
    min = vMin;
}

void BoundBox2f::setMin(const Vector2f &v) {
    min = v;
}

void BoundBox2f::setMax(const Vector2f &v) {
    max = v;
}

BoundBox3f::BoundBox3f() {
    resetBound();
}

BoundBox3f::~BoundBox3f() = default;

void BoundBox3f::resetBound() {
    min.set(std::numeric_limits<f32>::max());
    max.set(-std::numeric_limits<f32>::max());
}

void BoundBox3f::setDirect(const Vector3f &vMin, const Vector3f &vMax) {
    max = vMax;
    min = vMin;
}

void BoundBox3f::setMin(const Vector3f &v) {
    min = v;
}

void BoundBox3f::setMax(const Vector3f &v) {
    max = v;
}

} // namespace EGG
