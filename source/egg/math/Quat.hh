#pragma once

#include "egg/math/Vector.hh"

namespace EGG {

/// @brief A quaternion, used to represent 3D rotation.
/// @details A quaternion is comprised of a scalar (w) and a vector (v).
/// The vector is used to represent the axis of rotation, while
/// the scalar is used to represent the amount of rotation.
struct Quatf {
    Quatf();
    Quatf(f32 w_, const Vector3f &v_);
    Quatf(f32 w_, f32 x_, f32 y_, f32 z_);
    ~Quatf();

    Quatf &operator=(const Quatf &q) {
        w = q.w;
        v = q.v;

        return *this;
    }

    [[nodiscard]] Quatf operator+(const Quatf &rhs) const {
        return Quatf(w + rhs.w, v + rhs.v);
    }

    Quatf &operator+=(const Quatf &rhs) {
        return *this = *this + rhs;
    }

    [[nodiscard]] Quatf operator*(const Vector3f &vec) const {
        Vector3f cross = v.cross(vec);
        Vector3f scale = vec * w;
        return Quatf(-v.dot(vec), cross + scale);
    }

    [[nodiscard]] Quatf operator*(f32 scalar) const {
        return Quatf(w * scalar, v * scalar);
    }

    Quatf &operator*=(f32 scalar) {
        return *this = *this * scalar;
    }

    /// Though part of this is a vector cross/dot product, FP arithmetic is not associative or
    /// commutative. It has to be done in this order.
    [[nodiscard]] Quatf operator*(const Quatf &rhs) const {
        f32 _w = w * rhs.w - v.x * rhs.v.x - v.y * rhs.v.y - v.z * rhs.v.z;
        f32 _x = v.y * rhs.v.z + (v.x * rhs.w + w * rhs.v.x) - v.z * rhs.v.y;
        f32 _y = v.z * rhs.v.x + (v.y * rhs.w + w * rhs.v.y) - v.x * rhs.v.z;
        f32 _z = v.x * rhs.v.y + (v.z * rhs.w + w * rhs.v.z) - v.y * rhs.v.x;

        return Quatf(_w, _x, _y, _z);
    }

    Quatf &operator*=(const Quatf &q) {
        return *this = *this * q;
    }

    bool operator==(const Quatf &rhs) const {
        return w == rhs.w && v == rhs.v;
    }

    bool operator!=(const Quatf &rhs) const {
        return !(*this == rhs);
    }

    /// @brief A conversion function that allows for string representation of a quaternion.
    explicit operator std::string() const {
        return std::format("[0x{:08X}, 0x{:08X}, 0x{:08X}, 0x{:08X}] | [{}, {}, {}, {}]", f2u(v.x),
                f2u(v.y), f2u(v.z), f2u(w), v.x, v.y, v.z, w);
    }

    void setRPY(const Vector3f &rpy);
    void normalise();
    void makeVectorRotation(const Vector3f &from, const Vector3f &to);
    Quatf conjugate() const;
    Vector3f rotateVector(const Vector3f &vec) const;
    Vector3f rotateVectorInv(const Vector3f &vec) const;
    Quatf slerpTo(const Quatf &q2, f32 t) const;
    f32 dot() const;
    f32 dot(const Quatf &q) const;
    void setAxisRotation(f32 angle, const Vector3f &axis);
    Quatf multSwap(const Vector3f &v) const;
    Quatf multSwap(const Quatf &q) const;

    void read(Stream &stream);

    Vector3f v;
    f32 w;

    static const Quatf ident;
};

} // namespace EGG
