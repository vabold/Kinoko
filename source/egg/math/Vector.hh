#pragma once

#include "egg/math/Math.hh"

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
    constexpr ~Vector2f() = default;

    constexpr inline void set(f32 val) {
        x = y = val;
    }

    [[nodiscard]] constexpr Vector2f operator-() const {
        return Vector2f(-x, -y);
    }

    [[nodiscard]] constexpr Vector2f operator-(const Vector2f &rhs) const {
        return Vector2f(x - rhs.x, y - rhs.y);
    }

    [[nodiscard]] constexpr Vector2f operator+(const Vector2f &rhs) const {
        return Vector2f(x + rhs.x, y + rhs.y);
    }

    constexpr Vector2f &operator+=(const Vector2f &rhs) {
        return *this = *this + rhs;
    }

    [[nodiscard]] constexpr Vector2f operator*(const f32 scalar) const {
        return Vector2f(x * scalar, y * scalar);
    }

    constexpr Vector2f &operator*=(const f32 scalar) {
        return *this = *this * scalar;
    }

    friend constexpr Vector2f operator*(f32 scalar, const Vector2f &rhs) {
        return Vector2f(scalar * rhs.x, scalar * rhs.y);
    }

    [[nodiscard]] constexpr f32 cross(const Vector2f &rhs) const {
        return x * rhs.y - y * rhs.x;
    }

    [[nodiscard]] constexpr f32 dot(const Vector2f &rhs) const {
        return x * rhs.x + y * rhs.y;
    }

    [[nodiscard]] constexpr f32 dot() const {
        return x * x + y * y;
    }

    [[nodiscard]] constexpr f32 length() const {
        return dot() > std::numeric_limits<f32>::epsilon() ? Mathf::sqrt(dot()) : 0.0f;
    }

    /// @addr{0x80243A00}
    constexpr f32 normalise() {
        f32 len = length();
        if (len != 0.0f) {
            *this = *this * (1.0f / len);
        }

        return len;
    }

    /// @addr{0x80243A78}
    constexpr void normalise2() {
        f32 sqLen = dot();
        if (sqLen > std::numeric_limits<f32>::epsilon()) {
            *this *= EGG::Mathf::frsqrt(sqLen);
        }
    }

    /// @brief Initializes a Vector2f by reading 8 bytes from the stream.
    void read(Stream &stream) {
        x = stream.read_f32();
        y = stream.read_f32();
    }

    f32 x;
    f32 y;

    static const Vector2f zero;
    static const Vector2f ex, ey;
};

inline constexpr Vector2f Vector2f::zero = Vector2f(0.0f, 0.0f); ///< @addr{0x80386F78}
inline constexpr Vector2f Vector2f::ex = Vector2f(1.0f, 0.0f);   ///< @addr{0x80386F80}
inline constexpr Vector2f Vector2f::ey = Vector2f(0.0f, 1.0f);   ///< @addr{0x80386F88}

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
    constexpr ~Vector3f() = default;

    constexpr inline void setZero() {
        set(0.0f);
    }

    constexpr inline void set(f32 val) {
        x = y = z = val;
    }

    [[nodiscard]] constexpr Vector3f operator-() const {
        return Vector3f(-x, -y, -z);
    }

    [[nodiscard]] constexpr Vector3f operator-(const Vector3f &rhs) const {
        return Vector3f(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    constexpr Vector3f &operator-=(const Vector3f &rhs) {
        return *this = *this - rhs;
    }

    [[nodiscard]] constexpr Vector3f operator+(const Vector3f &rhs) const {
        return Vector3f(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    constexpr Vector3f &operator+=(const Vector3f &rhs) {
        return *this = *this + rhs;
    }

    [[nodiscard]] constexpr Vector3f operator+(f32 val) const {
        return Vector3f(x + val, y + val, z + val);
    }

    constexpr Vector3f &operator+=(f32 val) {
        return *this = *this + val;
    }

    [[nodiscard]] constexpr Vector3f operator*(const Vector3f &rhs) const {
        return Vector3f(x * rhs.x, y * rhs.y, z * rhs.z);
    }

    [[nodiscard]] constexpr Vector3f operator*(f32 scalar) const {
        return Vector3f(x * scalar, y * scalar, z * scalar);
    }

    [[nodiscard]] friend constexpr Vector3f operator*(f32 scalar, const Vector3f &rhs) {
        return rhs * scalar;
    }

    constexpr Vector3f &operator*=(f32 scalar) {
        return *this = *this * scalar;
    }

    [[nodiscard]] constexpr Vector3f operator/(f32 scalar) const {
        return Vector3f(x / scalar, y / scalar, z / scalar);
    }

    constexpr Vector3f &operator/=(f32 scalar) {
        return *this = *this / scalar;
    }

    [[nodiscard]] constexpr bool operator==(const Vector3f &rhs) const {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }

    [[nodiscard]] constexpr bool operator!=(const Vector3f &rhs) const {
        return !(*this == rhs);
    }

    /// @brief Allows for copy construction from a vector to a string.
    [[nodiscard]] explicit operator std::string() const {
        return std::format("[0x{:08X}, 0x{:08X}, 0x{:08X}] | [{}, {}, {}]", f2u(x), f2u(y), f2u(z),
                x, y, z);
    }

    /// @addr{0x80214968}
    [[nodiscard]] constexpr Vector3f cross(const Vector3f &rhs) const {
        return Vector3f(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
    }

    /// @brief The dot product between the vector and itself.
    [[nodiscard]] constexpr f32 squaredLength() const {
        return x * x + y * y + z * z;
    }

    /// @brief The dot product between two vectors.
    [[nodiscard]] constexpr f32 dot(const Vector3f &rhs) const {
        return x * rhs.x + y * rhs.y + z * rhs.z;
    }

    /// @brief The square root of the vector's dot product.
    [[nodiscard]] constexpr f32 length() const {
        return Mathf::sqrt(squaredLength());
    }

    /// @addr{0x805AEB88}
    /// @brief The projection of this vector onto rhs.
    [[nodiscard]] constexpr Vector3f proj(const Vector3f &rhs) const {
        return rhs * rhs.dot(*this);
    }

    /// @addr{0x805AEBD0}
    /// @brief The rejection of this vector onto rhs.
    [[nodiscard]] constexpr Vector3f rej(const Vector3f &rhs) const {
        return *this - proj(rhs);
    }

    /// @addr{0x805AEC24}
    [[nodiscard]] constexpr std::pair<Vector3f, Vector3f> projAndRej(const Vector3f &rhs) const {
        return std::pair(proj(rhs), rej(rhs));
    }

    /// @brief Returns the absolute value of each element of the vector.
    [[nodiscard]] constexpr Vector3f abs() const {
        return Vector3f(Mathf::abs(x), Mathf::abs(y), Mathf::abs(z));
    }

    /// @brief The square of the distance between two vectors.
    [[nodiscard]] constexpr f32 sqDistance(const Vector3f &rhs) const {
        const EGG::Vector3f diff = *this - rhs;
        return diff.squaredLength();
    }

    /// @addr{0x806A62A4}
    /// @brief Multiplies a vector by the inverse of val.
    [[nodiscard]] constexpr EGG::Vector3f multInv(f32 val) const {
        return *this * (1.0f / val);
    }

    /// @brief Paired-singles dot product implementation.
    [[nodiscard]] constexpr f32 ps_dot() const {
        return ps_dot(*this);
    }

    /// @addr{0x8019ACAC}
    /// @brief Paired-singles dot product implementation.
    [[nodiscard]] constexpr f32 ps_dot(const Vector3f &rhs) const {
        f32 y_ = y * rhs.y;
        f32 xy = Mathf::fma(x, rhs.x, y_);
        return xy + z * rhs.z;
    }

    /// @brief Differs from ps_dot due to variation in which operands are fused.
    [[nodiscard]] constexpr f32 ps_squareMag() const {
        f32 x_ = x * x;
        f32 zx = Mathf::fma(z, z, x_);
        return zx + y * y;
    }

    /// @addr{0x80243ADC}
    /// @brief Normalizes the vector and returns the original length.
    /// @return (optional) The length of the vector before normalisation.
    constexpr f32 normalise() {
        f32 len = 0.0f;

        if (squaredLength() > std::numeric_limits<f32>::epsilon()) {
            len = length();
            *this *= (1.0f / len);
        }

        return len;
    }

    /// @addr{0x80243B6C}
    constexpr void normalise2() {
        f32 sqLen = squaredLength();
        if (sqLen > std::numeric_limits<f32>::epsilon()) {
            *this *= Mathf::frsqrt(sqLen);
        }
    }

    /// @addr{0x80085580}
    /// @brief Returns a vector whose elements are the max of the elements of both vectors.
    [[nodiscard]] constexpr Vector3f maximize(const Vector3f &rhs) const {
        Vector3f out;

        out.x = x > rhs.x ? x : rhs.x;
        out.y = y > rhs.y ? y : rhs.y;
        out.z = z > rhs.z ? z : rhs.z;

        return out;
    }

    /// @addr{0x800855C0}
    /// @brief Returns a vector whose elements are the min of the elements of both vectors.
    [[nodiscard]] constexpr Vector3f minimize(const Vector3f &rhs) const {
        Vector3f out;

        out.x = x < rhs.x ? x : rhs.x;
        out.y = y < rhs.y ? y : rhs.y;
        out.z = z < rhs.z ? z : rhs.z;

        return out;
    }

    /// @addr{0x8019ADE0}
    /// @brief Paired-singles impl. of @ref sqDistance.
    [[nodiscard]] constexpr f32 ps_sqDistance(const Vector3f &rhs) const {
        const EGG::Vector3f diff = *this - rhs;
        return diff.ps_dot();
    }

    /// @brief Calculates the orthogonal vector, based on the plane defined by this vector and rhs.
    /// @addr{0x805AE9EC}
    [[nodiscard]] constexpr Vector3f perpInPlane(const EGG::Vector3f &rhs, bool normalise) const {
        if (Mathf::abs(dot(rhs)) == 1.0f) {
            return EGG::Vector3f::zero;
        }

        f32 _x = (rhs.z * x - rhs.x * z) * rhs.z - (rhs.x * y - rhs.y * x) * rhs.y;
        f32 _y = (rhs.x * y - rhs.y * x) * rhs.x - (rhs.y * z - rhs.z * y) * rhs.z;
        f32 _z = (rhs.y * z - rhs.z * y) * rhs.y - (rhs.z * x - rhs.x * z) * rhs.x;

        EGG::Vector3f ret(_x, _y, _z);

        if (normalise) {
            ret.normalise();
        }

        return ret;
    }

    /// @brief Initializes a Vector3f by reading 12 bytes from the stream.
    void read(Stream &stream) {
        x = stream.read_f32();
        y = stream.read_f32();
        z = stream.read_f32();
    }

    f32 x;
    f32 y;
    f32 z;

    static const Vector3f zero;
    static const Vector3f unit;
    static const Vector3f ex, ey, ez;
    static const Vector3f inf;
};

inline constexpr Vector3f Vector3f::zero = Vector3f(0.0f, 0.0f, 0.0f); ///< @addr{0x80384BA0}
inline constexpr Vector3f Vector3f::unit = Vector3f(1.0f, 1.0f, 1.0f);
inline constexpr Vector3f Vector3f::ex = Vector3f(1.0f, 0.0f, 0.0f); ///< @addr{0x80384BB8}
inline constexpr Vector3f Vector3f::ey = Vector3f(0.0f, 1.0f, 0.0f); ///< @addr{0x80384BD0}
inline constexpr Vector3f Vector3f::ez = Vector3f(0.0f, 0.0f, 1.0f); ///< @addr{0x80384BE8}

/// @addr{0x809C3C04}
inline constexpr Vector3f Vector3f::inf = Vector3f(std::numeric_limits<f32>::infinity(),
        std::numeric_limits<f32>::infinity(), std::numeric_limits<f32>::infinity());

} // namespace EGG
