#pragma once

#include "egg/util/Stream.hh"

namespace EGG {

struct Vector2f {
    Vector2f(f32 x_, f32 y_);
    Vector2f();
    ~Vector2f();

    inline void set(f32 val) {
        x = y = val;
    }

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

    inline void setZero() {
        set(0.0f);
    }

    inline void set(f32 val) {
        x = y = z = val;
    }

    Vector3f operator-() const {
        return Vector3f(-x, -y, -z);
    }

    Vector3f operator-(const Vector3f &rhs) const {
        return Vector3f(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    Vector3f operator+(const Vector3f &rhs) const {
        return Vector3f(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    Vector3f &operator+=(const Vector3f &rhs) {
        return *this = *this + rhs;
    }

    Vector3f operator*(f32 scalar) const {
        return Vector3f(x * scalar, y * scalar, z * scalar);
    }

    friend Vector3f operator*(f32 scalar, const Vector3f &rhs) {
        return rhs * scalar;
    }

    Vector3f &operator*=(f32 scalar) {
        return *this = *this * scalar;
    }

    Vector3f operator/(f32 scalar) const {
        return Vector3f(x / scalar, y / scalar, z / scalar);
    }

    Vector3f &operator/=(f32 scalar) {
        return *this = *this / scalar;
    }

    bool operator==(const Vector3f &rhs) const {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }

    bool operator!=(const Vector3f &rhs) const {
        return !(*this == rhs);
    }

    Vector3f cross(const EGG::Vector3f &rhs) const;
    f32 dot() const;
    f32 dot(const EGG::Vector3f &rhs) const;
    f32 ps_dot(const EGG::Vector3f &rhs) const;
    f32 length() const;
    f32 normalise();
    Vector3f maximize(const Vector3f &rhs) const;
    Vector3f minimize(const Vector3f &rhs) const;
    Vector3f proj(const Vector3f &rhs) const;
    Vector3f rej(const Vector3f &rhs) const;
    std::pair<Vector3f, Vector3f> projAndRej(const Vector3f &rhs);

    void read(Stream &stream);

    f32 x;
    f32 y;
    f32 z;

    static const Vector3f zero;
    static const Vector3f ex, ey, ez;
    static const Vector3f inf;
};

} // namespace EGG
