#include "Quat.hh"

#include "egg/math/Math.hh"

namespace EGG {

Quatf::Quatf() : w(1.0f) {}

Quatf::Quatf(f32 w_, const Vector3f &v_) : v(v_), w(w_) {}

Quatf::Quatf(f32 w_, f32 x_, f32 y_, f32 z_) : v(x_, y_, z_), w(w_) {}

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

void Quatf::normalise() {
    f32 len = dot() > FLT_EPSILON ? Mathf::sqrt(dot()) : 0.0f;

    if (len != 0.0f) {
        f32 inv = 1.0f / len;
        w *= inv;
        v *= inv;
    }
}

Quatf Quatf::conjugate() const {
    return Quatf(w, -v);
}

Vector3f Quatf::rotateVector(const Vector3f &vec) const {
    Quatf conj = conjugate();
    Quatf res = *this * vec;
    res *= conj;
    return res.v;
}

Vector3f Quatf::rotateVectorInv(const Vector3f &vec) const {
    Quatf conj = conjugate();
    Quatf res = conj * vec;
    res *= *this;
    return res.v;
}

Quatf Quatf::slerpTo(const Quatf &q1, f32 t) const {
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

f32 Quatf::dot() const {
    return w * w + v.dot();
}

f32 Quatf::dot(const Quatf &q) const {
    return w * q.w + v.dot(q.v);
}

void Quatf::setAxisRotation(f32 angle, const EGG::Vector3f &axis) {
    const f32 half_angle = angle * 0.5f;
    const f32 cos = Mathf::cos(half_angle);
    const f32 sin = Mathf::sin(half_angle);

    w = cos;
    v = axis * sin;
}

void Quatf::read(Stream &stream) {
    v.read(stream);
    w = stream.read_f32();
}

const Quatf Quatf::ident = Quatf(1.0f, Vector3f::zero);

} // namespace EGG
