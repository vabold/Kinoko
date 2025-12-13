#pragma once

#include "egg/math/Math.hh"
#include "egg/math/Vector.hh"

namespace EGG {

/// @brief A quaternion, used to represent 3D rotation.
/// @details A quaternion is comprised of a scalar (w) and a vector (v).
/// The vector is used to represent the axis of rotation, while
/// the scalar is used to represent the amount of rotation.
struct Quatf {
#ifdef BUILD_DEBUG
    constexpr Quatf() : w(std::numeric_limits<f32>::signaling_NaN()) {}
#else
    constexpr Quatf() = default;
#endif
    constexpr Quatf(const Quatf &q) = default;
    constexpr Quatf(f32 w_, const Vector3f &v_) : v(v_), w(w_) {}
    constexpr Quatf(f32 w_, f32 x_, f32 y_, f32 z_) : v(x_, y_, z_), w(w_) {}
    constexpr ~Quatf() = default;

    constexpr Quatf &operator=(const Quatf &q) {
        w = q.w;
        v = q.v;

        return *this;
    }

    [[nodiscard]] constexpr Quatf operator+(const Quatf &rhs) const {
        return Quatf(w + rhs.w, v + rhs.v);
    }

    constexpr Quatf &operator+=(const Quatf &rhs) {
        return *this = *this + rhs;
    }

    [[nodiscard]] constexpr Quatf operator*(const Vector3f &vec) const {
        Vector3f cross = v.cross(vec);
        Vector3f scale = vec * w;
        return Quatf(-v.dot(vec), cross + scale);
    }

    [[nodiscard]] constexpr Quatf operator*(f32 scalar) const {
        return Quatf(w * scalar, v * scalar);
    }

    constexpr Quatf &operator*=(f32 scalar) {
        return *this = *this * scalar;
    }

    /// Though part of this is a vector cross/dot product, FP arithmetic is not associative or
    /// commutative. It has to be done in this order.
    [[nodiscard]] constexpr Quatf operator*(const Quatf &rhs) const {
        f32 _w = w * rhs.w - v.x * rhs.v.x - v.y * rhs.v.y - v.z * rhs.v.z;
        f32 _x = v.y * rhs.v.z + (v.x * rhs.w + w * rhs.v.x) - v.z * rhs.v.y;
        f32 _y = v.z * rhs.v.x + (v.y * rhs.w + w * rhs.v.y) - v.x * rhs.v.z;
        f32 _z = v.x * rhs.v.y + (v.z * rhs.w + w * rhs.v.z) - v.y * rhs.v.x;

        return Quatf(_w, _x, _y, _z);
    }

    constexpr Quatf &operator*=(const Quatf &q) {
        return *this = *this * q;
    }

    [[nodiscard]] constexpr bool operator==(const Quatf &rhs) const {
        return w == rhs.w && v == rhs.v;
    }

    [[nodiscard]] constexpr bool operator!=(const Quatf &rhs) const {
        return !(*this == rhs);
    }

    /// @brief A conversion function that allows for string representation of a quaternion.
    [[nodiscard]] explicit operator std::string() const {
        return std::format("[0x{:08X}, 0x{:08X}, 0x{:08X}, 0x{:08X}] | [{}, {}, {}, {}]", f2u(v.x),
                f2u(v.y), f2u(v.z), f2u(w), v.x, v.y, v.z, w);
    }

    /// @addr{0x80239E10}
    /// @brief Sets roll, pitch, and yaw.
    constexpr void setRPY(const Vector3f &rpy) {
        *this = FromRPY(rpy.x, rpy.y, rpy.z);
    }

    /// @brief Helper function to avoid unnecessary Vector3f construction.
    constexpr void setRPY(f32 r, f32 p, f32 y) {
        const f32 cy = Mathf::cos(y * 0.5f);
        const f32 cp = Mathf::cos(p * 0.5f);
        const f32 cr = Mathf::cos(r * 0.5f);
        const f32 sy = Mathf::sin(y * 0.5f);
        const f32 sp = Mathf::sin(p * 0.5f);
        const f32 sr = Mathf::sin(r * 0.5f);

        w = cy * cp * cr + sy * sp * sr;
        v.x = cy * cp * sr - sy * sp * cr;
        v.y = cy * sp * cr + sy * cp * sr;
        v.z = sy * cp * cr - cy * sp * sr;
    }

    /// @addr{0x8023A168}
    /// @brief Scales the quaternion to a unit length.
    constexpr void normalise() {
        f32 len = squaredNorm() > std::numeric_limits<f32>::epsilon() ? norm() : 0.0f;

        if (len != 0.0f) {
            f32 inv = 1.0f / len;
            w *= inv;
            v *= inv;
        }
    }

    /// @addr{0x8023A788}
    /// @brief Captures rotation between two vectors.
    constexpr void makeVectorRotation(const Vector3f &from, const Vector3f &to) {
        f32 t0 = std::max(0.0f, (from.dot(to) + 1) * 2.0f);
        t0 = Mathf::sqrt(t0);

        if (t0 <= std::numeric_limits<f32>::epsilon()) {
            *this = Quatf::ident;
        } else {
            const f32 inv = 1.0f / t0;
            w = t0 * 0.5f;
            v = from.cross(to) * inv;
        }
    }

    /// @brief Computes \f$conj(a+bi+cj+dk) = a-bi-cj-dk\f$
    [[nodiscard]] constexpr Quatf conjugate() const {
        return Quatf(w, -v);
    }

    /// @addr{0x8023A2D0}
    /// @brief Rotates a vector based on the quat.
    [[nodiscard]] constexpr Vector3f rotateVector(const Vector3f &vec) const {
        Quatf conj = conjugate();
        Quatf res = *this * vec;
        Quatf ret;

        ret.v.x = (res.v.y * conj.v.z + (res.v.x * conj.w + res.w * conj.v.x)) - res.v.z * conj.v.y;
        ret.v.y = (res.v.z * conj.v.x + (res.v.y * conj.w + res.w * conj.v.y)) - res.v.x * conj.v.z;
        ret.v.z = (res.v.x * conj.v.y + (res.v.z * conj.w + res.w * conj.v.z)) - res.v.y * conj.v.x;

        return ret.v;
    }

    /// @addr{0x8023A404}
    /// @brief Rotates a vector on the inverse quat.
    [[nodiscard]] constexpr Vector3f rotateVectorInv(const Vector3f &vec) const {
        Quatf conj = conjugate();
        Quatf res = conj * vec;
        Quatf ret;

        ret.v.x = (res.v.y * v.z + (res.v.x * w + res.w * v.x)) - res.v.z * v.y;
        ret.v.y = (res.v.z * v.x + (res.v.y * w + res.w * v.y)) - res.v.x * v.z;
        ret.v.z = (res.v.x * v.y + (res.v.z * w + res.w * v.z)) - res.v.y * v.x;

        return ret.v;
    }

    /// @addr{0x8023A5C4}
    /// @brief Performs spherical linear interpolation.
    /// @details Slerp is a method in which you can create smooth rotations between two quaternions.
    [[nodiscard]] constexpr Quatf slerpTo(const Quatf &q1, f32 t) const {
        f32 dot_ = std::max(-1.0f, std::min(1.0f, dot(q1)));
        bool bDot = dot_ < 0.0f;
        dot_ = Mathf::abs(dot_);

        f32 acos = Mathf::acos(dot_);
        f32 sin = Mathf::sin(acos);

        f32 s;
        if (Mathf::abs(sin) < 0.00001f) {
            s = 1.0f - t;
        } else {
            f32 invSin = 1.0f / sin;
            f32 tmp0 = t * acos;
            s = invSin * Mathf::sin(acos - tmp0);
            t = invSin * Mathf::sin(tmp0);
        }

        if (bDot) {
            t = -t;
        }

        return Quatf(s * w + t * q1.w, s * v + t * q1.v);
    }

    /// @addr{0x8023A138}
    /// @brief Computes \f$this \cdot this = w^2 + x^2 + y^2 + z^2\f$
    [[nodiscard]] constexpr f32 squaredNorm() const {
        return w * w + v.squaredLength();
    }

    [[nodiscard]] constexpr f32 norm() const {
        return Mathf::sqrt(squaredNorm());
    }

    /// @brief Computes \f$this \cdot rhs = w \times rhs.w + x \times rhs.x + y \times rhs.y + z
    /// \times rhs.z\f$
    [[nodiscard]] constexpr f32 dot(const Quatf &q) const {
        return w * q.w + v.dot(q.v);
    }

    /// @addr{0x8023A0A0}
    /// @brief Set the quat given angle and axis.
    constexpr void setAxisRotation(f32 angle, const EGG::Vector3f &axis) {
        const f32 half_angle = angle * 0.5f;
        const f32 c = Mathf::cos(half_angle);
        const f32 s = Mathf::sin(half_angle);

        w = c;
        v = axis * s;
    }

    [[nodiscard]] constexpr Quatf multSwap(const Vector3f &vec) const {
        f32 _w = -(v.dot(vec));
        f32 _x = (w * vec.x + v.y * vec.z) - v.z * vec.y;
        f32 _y = (w * vec.y + v.z * vec.x) - v.x * vec.z;
        f32 _z = (w * vec.z + v.x * vec.y) - v.y * vec.x;

        return Quatf(_w, _x, _y, _z);
    }

    [[nodiscard]] constexpr Quatf multSwap(const Quatf &q) const {
        f32 _w = ((w * q.w - v.x * q.v.x) - v.y * q.v.y) - v.z * q.v.z;
        f32 _x = (v.y * q.v.z + (v.x * q.w + w * q.v.x)) - v.z * q.v.y;
        f32 _y = (v.z * q.v.x + (v.y * q.w + w * q.v.y)) - v.x * q.v.z;
        f32 _z = (v.x * q.v.y + (v.z * q.w + w * q.v.z)) - v.y * q.v.x;

        return Quatf(_w, _x, _y, _z);
    }

    void read(Stream &stream) {
        v.read(stream);
        w = stream.read_f32();
    }

    [[nodiscard]] static constexpr Quatf FromRPY(const EGG::Vector3f &rpy) {
        Quatf ret;
        ret.setRPY(rpy);
        return ret;
    }

    /// @brief Helper function to avoid unnecessary Vector3f construction.
    [[nodiscard]] static constexpr Quatf FromRPY(f32 r, f32 p, f32 y) {
        Quatf ret;
        ret.setRPY(r, p, y);
        return ret;
    }

    Vector3f v;
    f32 w;

    static const Quatf ident;
};

inline constexpr Quatf Quatf::ident = Quatf(1.0f, Vector3f::zero);

} // namespace EGG
