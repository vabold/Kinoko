#include "Quat.hh"

#include "egg/math/Math.hh"

namespace EGG {

Quatf::Quatf() = default;

Quatf::Quatf(f32 w_, const Vector3f &v_) : v(v_), w(w_) {}

Quatf::~Quatf() = default;

void Quatf::setRPY(const Vector3f &rpy) {
    const f32 cz = Mathf::cos(rpy.z * 0.5f);
    const f32 cy = Mathf::cos(rpy.y * 0.5f);
    const f32 cx = Mathf::cos(rpy.x * 0.5f);
    const f32 sz = Mathf::sin(rpy.z * 0.5f);
    const f32 sy = Mathf::sin(rpy.y * 0.5f);
    const f32 sx = Mathf::sin(rpy.x * 0.5f);

    w = cz * cy * cx + sz * sy * sx;
    v.x = cz * cy * sx - sz * sy * cx;
    v.y = cz * sy * cx + sz * cy * sx;
    v.z = sz * cy * cx - cz * sy * sx;
}

Quatf Quatf::conjugate() const {
    return Quatf(w, -v);
}

Vector3f Quatf::rotateVector(const EGG::Vector3f &vec) const {
    Quatf conj = conjugate();
    Quatf res = *this * vec;
    res *= conj;
    return res.v;
}

} // namespace EGG
