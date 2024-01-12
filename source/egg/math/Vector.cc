#include "Vector.hh"

#include "egg/math/Math.hh"

namespace EGG {

Vector2f::Vector2f(f32 x_, f32 y_) : x(x_), y(y_) {}

Vector2f::Vector2f() = default;

Vector2f::~Vector2f() = default;

f32 Vector2f::cross(const Vector2f &rhs) const {
    return x * rhs.y - y * rhs.x;
}

f32 Vector2f::dot(const Vector2f &rhs) const {
    return x * rhs.x + y * rhs.y;
}

f32 Vector2f::dot() const {
    return x * x + y * y;
}

f32 Vector2f::length() const {
    return dot() > FLT_EPSILON ? Mathf::sqrt(dot()) : 0.0f;
}

f32 Vector2f::normalise() {
    f32 len = length();
    if (len != 0.0f) {
        *this = *this * (1.0f / len);
    }

    return len;
}

Vector3f::Vector3f(f32 x_, f32 y_, f32 z_) : x(x_), y(y_), z(z_) {}

Vector3f::Vector3f() = default;

Vector3f::~Vector3f() = default;

Vector3f Vector3f::cross(const Vector3f &rhs) const {
    return Vector3f(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
}

f32 Vector3f::dot(const Vector3f &rhs) const {
    return x * rhs.x + y * rhs.y + z * rhs.z;
}

f32 Vector3f::dot() const {
    return x * x + y * y + z * z;
}

f32 Vector3f::length() const {
    return dot() > FLT_EPSILON ? Mathf::sqrt(dot()) : 0.0f;
}

f32 Vector3f::normalise() {
    f32 len = length();
    if (len != 0.0f) {
        *this = *this * (1.0f / len);
    }

    return len;
}

void Vector3f::read(Stream &stream) {
    x = stream.read_f32();
    y = stream.read_f32();
    z = stream.read_f32();
}

const Vector2f Vector2f::zero = Vector2f(0.0f, 0.0f);
const Vector2f Vector2f::ex = Vector2f(1.0f, 0.0f);
const Vector2f Vector2f::ey = Vector2f(0.0f, 1.0f);

const Vector3f Vector3f::zero = Vector3f(0.0f, 0.0f, 0.0f);
const Vector3f Vector3f::ex = Vector3f(1.0f, 0.0f, 0.0f);
const Vector3f Vector3f::ey = Vector3f(0.0f, 1.0f, 0.0f);
const Vector3f Vector3f::ez = Vector3f(0.0f, 0.0f, 1.0f);

} // namespace EGG
