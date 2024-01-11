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

    void operator*=(const Quatf &q) {
        Vector3f cross = v.cross(q.v);
        Vector3f scaleLhs = v * q.w;
        Vector3f scaleRhs = q.v * w;

        w = w * q.w - v.dot(q.v);
        v = cross + scaleRhs + scaleLhs;
    }

    void setRPY(const Vector3f &rpy);
    Quatf conjugate() const;
    Vector3f rotateVector(const Vector3f &vec) const;

    Vector3f v;
    f32 w;
};

} // namespace EGG
