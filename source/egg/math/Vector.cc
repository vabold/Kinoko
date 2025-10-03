#include "Vector.hh"

namespace EGG {

/// @addr{0x80243A00}
f32 Vector2f::normalise() {
    f32 len = length();
    if (len != 0.0f) {
        *this = *this * (1.0f / len);
    }

    return len;
}

/// @addr{0x80243A78}
void Vector2f::normalise2() {
    f32 sqLen = dot();
    if (sqLen > std::numeric_limits<f32>::epsilon()) {
        *this *= EGG::Mathf::frsqrt(sqLen);
    }
}

/// @brief Initializes a Vector2f by reading 8 bytes from the stream.
void Vector2f::read(Stream &stream) {
    x = stream.read_f32();
    y = stream.read_f32();
}

/// @brief Paired-singles dot product implementation.
f32 Vector3f::ps_dot() const {
    return ps_dot(*this);
}

/// @addr{0x8019ACAC}
/// @brief Paired-singles dot product implementation.
f32 Vector3f::ps_dot(const Vector3f &rhs) const {
    f32 y_ = y * rhs.y;
    f32 xy = Mathf::fma(x, rhs.x, y_);
    return xy + z * rhs.z;
}

/// @brief Differs from ps_dot due to variation in which operands are fused.
f32 Vector3f::ps_squareMag() const {
    f32 x_ = x * x;
    f32 zx = Mathf::fma(z, z, x_);
    return zx + y * y;
}

/// @addr{0x80243ADC}
/// @brief Normalizes the vector and returns the original length.
/// @return (optional) The length of the vector before normalisation.
f32 Vector3f::normalise() {
    f32 len = 0.0f;

    if (squaredLength() > std::numeric_limits<f32>::epsilon()) {
        len = length();
        *this *= (1.0f / len);
    }

    return len;
}

/// @addr{0x80243B6C}
void Vector3f::normalise2() {
    f32 sqLen = squaredLength();
    if (sqLen > std::numeric_limits<f32>::epsilon()) {
        *this *= Mathf::frsqrt(sqLen);
    }
}

/// @addr{0x80085580}
/// @brief Returns a vector whose elements are the max of the elements of both vectors.
Vector3f Vector3f::maximize(const Vector3f &rhs) const {
    Vector3f out;

    out.x = x > rhs.x ? x : rhs.x;
    out.y = y > rhs.y ? y : rhs.y;
    out.z = z > rhs.z ? z : rhs.z;

    return out;
}

/// @addr{0x800855C0}
/// @brief Returns a vector whose elements are the min of the elements of both vectors.
Vector3f Vector3f::minimize(const Vector3f &rhs) const {
    Vector3f out;

    out.x = x < rhs.x ? x : rhs.x;
    out.y = y < rhs.y ? y : rhs.y;
    out.z = z < rhs.z ? z : rhs.z;

    return out;
}

/// @addr{0x8019ADE0}
/// @brief Paired-singles impl. of @ref sqDistance.
f32 Vector3f::ps_sqDistance(const Vector3f &rhs) const {
    const EGG::Vector3f diff = *this - rhs;
    return diff.ps_dot();
}

/// @brief Calculates the orthogonal vector, based on the plane defined by this vector and rhs.
/// @addr{0x805AE9EC}
Vector3f Vector3f::perpInPlane(const EGG::Vector3f &rhs, bool normalise) const {
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
void Vector3f::read(Stream &stream) {
    x = stream.read_f32();
    y = stream.read_f32();
    z = stream.read_f32();
}

} // namespace EGG
