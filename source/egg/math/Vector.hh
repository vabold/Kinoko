#pragma once

#include "egg/util/Stream.hh"

#include <format>

namespace EGG {

/// @brief A 2D float vector.
struct Vector2f {
#ifdef BUILD_DEBUG
    constexpr Vector2f()
        : x(std::numeric_limits<f32>::signaling_NaN()),
          y(std::numeric_limits<f32>::signaling_NaN()) {}
#else
    constexpr Vector2f() = default;
#endif
    constexpr Vector2f(f32 x_, f32 y_) : x(x_), y(y_) {}
    ~Vector2f();

    inline void set(f32 val) {
        x = y = val;
    }

    [[nodiscard]] Vector2f operator-() const {
        return Vector2f(-x, -y);
    }

    [[nodiscard]] Vector2f operator-(const Vector2f &rhs) const {
        return Vector2f(x - rhs.x, y - rhs.y);
    }

    [[nodiscard]] Vector2f operator+(const Vector2f &rhs) const {
        return Vector2f(x + rhs.x, y + rhs.y);
    }

    Vector2f &operator+=(const Vector2f &rhs) {
        return *this = *this + rhs;
    }

    [[nodiscard]] Vector2f operator*(const f32 scalar) const {
        return Vector2f(x * scalar, y * scalar);
    }

    Vector2f &operator*=(const f32 scalar) {
        return *this = *this * scalar;
    }

    friend Vector2f operator*(f32 scalar, const Vector2f &rhs) {
        return Vector2f(scalar * rhs.x, scalar * rhs.y);
    }

    [[nodiscard]] f32 cross(const Vector2f &rhs) const;
    [[nodiscard]] f32 dot(const Vector2f &rhs) const;
    [[nodiscard]] f32 dot() const;
    [[nodiscard]] f32 length() const;
    f32 normalise();

    void read(Stream &stream);

    f32 x;
    f32 y;

    static const Vector2f zero;
    static const Vector2f ex, ey;
};

/// @brief A 3D float vector.
struct Vector3f {
#ifdef BUILD_DEBUG
    constexpr Vector3f()
        : x(std::numeric_limits<f32>::signaling_NaN()),
          y(std::numeric_limits<f32>::signaling_NaN()),
          z(std::numeric_limits<f32>::signaling_NaN()) {}
#else
    constexpr Vector3f() = default;
#endif
    constexpr Vector3f(f32 x_, f32 y_, f32 z_) : x(x_), y(y_), z(z_) {}

    // NOTE: Defining the destructor in the header ensures the struct is trivially destructible
    ~Vector3f() = default;

    inline void setZero() {
        set(0.0f);
    }

    inline void set(f32 val) {
        x = y = z = val;
    }

    [[nodiscard]] Vector3f operator-() const {
        return Vector3f(-x, -y, -z);
    }

    [[nodiscard]] Vector3f operator-(const Vector3f &rhs) const {
        return Vector3f(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    Vector3f &operator-=(const Vector3f &rhs) {
        return *this = *this - rhs;
    }

    [[nodiscard]] Vector3f operator+(const Vector3f &rhs) const {
        return Vector3f(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    Vector3f &operator+=(const Vector3f &rhs) {
        return *this = *this + rhs;
    }

    [[nodiscard]] Vector3f operator+(f32 val) const {
        return Vector3f(x + val, y + val, z + val);
    }

    Vector3f &operator+=(f32 val) {
        return *this = *this + val;
    }

    [[nodiscard]] Vector3f operator*(const Vector3f &rhs) const {
        return Vector3f(x * rhs.x, y * rhs.y, z * rhs.z);
    }

    [[nodiscard]] Vector3f operator*(f32 scalar) const {
        return Vector3f(x * scalar, y * scalar, z * scalar);
    }

    [[nodiscard]] friend Vector3f operator*(f32 scalar, const Vector3f &rhs) {
        return rhs * scalar;
    }

    Vector3f &operator*=(f32 scalar) {
        return *this = *this * scalar;
    }

    [[nodiscard]] Vector3f operator/(f32 scalar) const {
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

    /// @brief Allows for copy construction from a vector to a string.
    explicit operator std::string() const {
        return std::format("[0x{:08X}, 0x{:08X}, 0x{:08X}] | [{}, {}, {}]", f2u(x), f2u(y), f2u(z),
                x, y, z);
    }

    [[nodiscard]] Vector3f cross(const EGG::Vector3f &rhs) const;
    [[nodiscard]] f32 dot() const;
    [[nodiscard]] f32 dot(const EGG::Vector3f &rhs) const;
    [[nodiscard]] f32 ps_dot() const;
    [[nodiscard]] f32 ps_dot(const EGG::Vector3f &rhs) const;
    [[nodiscard]] f32 ps_squareMag() const;
    [[nodiscard]] f32 length() const;
    f32 normalise();
    [[nodiscard]] Vector3f maximize(const Vector3f &rhs) const;
    [[nodiscard]] Vector3f minimize(const Vector3f &rhs) const;
    [[nodiscard]] Vector3f proj(const Vector3f &rhs) const;
    [[nodiscard]] Vector3f rej(const Vector3f &rhs) const;
    [[nodiscard]] std::pair<Vector3f, Vector3f> projAndRej(const Vector3f &rhs) const;
    [[nodiscard]] f32 sqDistance(const Vector3f &rhs) const;
    [[nodiscard]] f32 ps_sqDistance(const Vector3f &rhs) const;
    [[nodiscard]] Vector3f abs() const;
    [[nodiscard]] Vector3f perpInPlane(const EGG::Vector3f &rhs, bool normalise) const;

    void read(Stream &stream);

    f32 x;
    f32 y;
    f32 z;

    static const Vector3f zero;
    static const Vector3f ex, ey, ez;
    static const Vector3f inf;
};

} // namespace EGG
