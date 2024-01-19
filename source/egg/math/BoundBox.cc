#include "BoundBox.hh"

#include "egg/math/Math.hh"

namespace EGG {

BoundBox2f::BoundBox2f() {
    resetBound();
}

BoundBox2f::~BoundBox2f() = default;

void BoundBox2f::resetBound() {
    mMin.set(std::numeric_limits<f32>::max());
    mMax.set(-std::numeric_limits<f32>::max());
}

void BoundBox2f::setDirect(const Vector2f &min, const Vector2f &max) {
    mMax = max;
    mMin = min;
}

void BoundBox2f::setMin(const Vector2f &min) {
    mMin = min;
}

void BoundBox2f::setMax(const Vector2f &max) {
    mMax = max;
}

BoundBox3f::BoundBox3f() {
    resetBound();
}

BoundBox3f::~BoundBox3f() = default;

void BoundBox3f::resetBound() {
    mMin.set(std::numeric_limits<f32>::max());
    mMax.set(-std::numeric_limits<f32>::max());
}

void BoundBox3f::setDirect(const Vector3f &min, const Vector3f &max) {
    mMax = max;
    mMin = min;
}

void BoundBox3f::setMin(const Vector3f &min) {
    mMin = min;
}

void BoundBox3f::setMax(const Vector3f &max) {
    mMax = max;
}

} // namespace EGG
