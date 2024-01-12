#pragma once

#include "egg/util/Stream.hh"

namespace EGG {

struct Vector2f {
    Vector2f(f32 x_, f32 y_);
    Vector2f();
    ~Vector2f();

    Vector2f operator-() const {
        return Vector2f(-x, -y);
    }

    Vector2f operator-(const Vector2f &rhs) const {
        return Vector2f(x - rhs.x, y - rhs.y);
    }

    Vector2f operator+(const Vector2f &rhs) const {
        return Vector2f(x + rhs.x, y + rhs.y);
    }

    Vector2f operator*(const f32 scalar) const {
        return Vector2f(x * scalar, y * scalar);
    }

    f32 cross(const Vector2f &rhs) const;
    f32 dot(const Vector2f &rhs) const;
    f32 dot() const;
    f32 length() const;
    f32 normalise();

    f32 x;
    f32 y;

    static const Vector2f zero;
    static const Vector2f ex, ey;
};

struct Vector3f {
    Vector3f(f32 x_, f32 y_, f32 z_);
    Vector3f();
    ~Vector3f();

    Vector3f operator-() const {
        return Vector3f(-x, -y, -z);
    }

    Vector3f operator-(const Vector3f &rhs) const {
        return Vector3f(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    Vector3f operator+(const Vector3f &rhs) const {
        return Vector3f(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    Vector3f operator*(float scalar) const {
        return Vector3f(x * scalar, y * scalar, z * scalar);
    }

    Vector3f cross(const EGG::Vector3f &rhs) const;
    f32 dot(const EGG::Vector3f &rhs) const;
    f32 dot() const;
    f32 length() const;
    f32 normalise();

    void read(Stream &stream);

    f32 x;
    f32 y;
    f32 z;

    static const Vector3f zero;
    static const Vector3f ex, ey, ez;
};

} // namespace EGG
