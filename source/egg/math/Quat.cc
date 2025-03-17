#include "Quat.hh"

namespace EGG {

/// @addr{0x80239E10}
/// @brief Sets roll, pitch, and yaw.
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

/// @addr{0x8023A168}
/// @brief Scales the quaternion to a unit length.
void Quatf::normalise() {
    f32 len = squaredNorm() > std::numeric_limits<f32>::epsilon() ? norm() : 0.0f;

    if (len != 0.0f) {
        f32 inv = 1.0f / len;
        w *= inv;
        v *= inv;
    }
}

/// @addr{0x8023A788}
/// @brief Captures rotation between two vectors.
void Quatf::makeVectorRotation(const Vector3f &from, const Vector3f &to) {
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

/// @addr{0x8023A2D0}
/// @brief Rotates a vector based on the quat.
Vector3f Quatf::rotateVector(const Vector3f &vec) const {
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
Vector3f Quatf::rotateVectorInv(const Vector3f &vec) const {
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

/// @addr{0x8023A0A0}
/// @brief Set the quat given angle and axis.
void Quatf::setAxisRotation(f32 angle, const EGG::Vector3f &axis) {
    const f32 half_angle = angle * 0.5f;
    const f32 c = Mathf::cos(half_angle);
    const f32 s = Mathf::sin(half_angle);

    w = c;
    v = axis * s;
}

Quatf Quatf::multSwap(const Vector3f &vec) const {
    f32 _w = -(v.dot(vec));
    f32 _x = (w * vec.x + v.y * vec.z) - v.z * vec.y;
    f32 _y = (w * vec.y + v.z * vec.x) - v.x * vec.z;
    f32 _z = (w * vec.z + v.x * vec.y) - v.y * vec.x;

    return Quatf(_w, _x, _y, _z);
}

Quatf Quatf::multSwap(const Quatf &q) const {
    f32 _w = ((w * q.w - v.x * q.v.x) - v.y * q.v.y) - v.z * q.v.z;
    f32 _x = (v.y * q.v.z + (v.x * q.w + w * q.v.x)) - v.z * q.v.y;
    f32 _y = (v.z * q.v.x + (v.y * q.w + w * q.v.y)) - v.x * q.v.z;
    f32 _z = (v.x * q.v.y + (v.z * q.w + w * q.v.z)) - v.y * q.v.x;

    return Quatf(_w, _x, _y, _z);
}

void Quatf::read(Stream &stream) {
    v.read(stream);
    w = stream.read_f32();
}

const Quatf Quatf::ident = Quatf(1.0f, Vector3f::zero);

} // namespace EGG
