#pragma once

#include "egg/math/Vector.hh"

namespace EGG {

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

    Quatf operator+(const Quatf &rhs) const {
        return Quatf(w + rhs.w, v + rhs.v);
    }

    Quatf &operator+=(const Quatf &rhs) {
        return *this = *this + rhs;
    }

    Quatf operator*(const Vector3f &vec) const {
        Vector3f cross = v.cross(vec);
        Vector3f scale = vec * w;
        return Quatf(-v.dot(vec), cross + scale);
    }

    Quatf operator*(f32 scalar) const {
        return Quatf(w * scalar, v * scalar);
    }

    Quatf &operator*=(f32 scalar) {
        return *this = *this * scalar;
    }

    Quatf operator*(const Quatf &rhs) const {
        Vector3f cross = v.cross(rhs.v);
        Vector3f scaleLhs = v * rhs.w;
        Vector3f scaleRhs = rhs.v * w;

        return Quatf(w * rhs.w - v.dot(rhs.v), cross + scaleRhs + scaleLhs);
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
