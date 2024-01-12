#pragma once

#include "egg/math/Vector.hh"

namespace EGG {

struct Quatf {
    Quatf();
    Quatf(f32 w_, const Vector3f &v_);
    ~Quatf();

    Quatf operator*(const Vector3f &vec) const {
        Vector3f cross = v.cross(vec);
        Vector3f scale = vec * w;
        return Quatf(-v.dot(vec), cross + scale);
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

    void setRPY(const Vector3f &rpy);
    Quatf conjugate() const;
    Vector3f rotateVector(const Vector3f &vec) const;

    Vector3f v;
    f32 w;
};

} // namespace EGG
