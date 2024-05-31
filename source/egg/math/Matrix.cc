#include "Matrix.hh"

#include "egg/math/Math.hh"

namespace EGG {

using namespace Mathf;

Matrix34f::Matrix34f() {
    makeZero();
}

Matrix34f::Matrix34f(f32 _e00, f32 _e01, f32 _e02, f32 _e03, f32 _e10, f32 _e11, f32 _e12, f32 _e13,
        f32 _e20, f32 _e21, f32 _e22, f32 _e23) {
    mtx[0][0] = _e00;
    mtx[0][1] = _e01;
    mtx[0][2] = _e02;
    mtx[0][3] = _e03;
    mtx[1][0] = _e10;
    mtx[1][1] = _e11;
    mtx[1][2] = _e12;
    mtx[1][3] = _e13;
    mtx[2][0] = _e20;
    mtx[2][1] = _e21;
    mtx[2][2] = _e22;
    mtx[2][3] = _e23;
}

Matrix34f::~Matrix34f() = default;

/// @addr{0x80230118}
/// @brief Sets matrix from rotation and position.
void Matrix34f::makeQT(const Quatf &q, const Vector3f &t) {
    f32 yy = 2.0f * q.v.y * q.v.y;
    f32 zz = 2.0f * q.v.z * q.v.z;
    f32 xx = 2.0f * q.v.x * q.v.x;
    f32 xy = 2.0f * q.v.x * q.v.y;
    f32 xz = 2.0f * q.v.x * q.v.z;
    f32 yz = 2.0f * q.v.y * q.v.z;
    f32 wz = 2.0f * q.w * q.v.z;
    f32 wx = 2.0f * q.w * q.v.x;
    f32 wy = 2.0f * q.w * q.v.y;

    mtx[0][0] = 1.0f - yy - zz;
    mtx[0][1] = xy - wz;
    mtx[0][2] = xz + wy;

    mtx[1][0] = xy + wz;
    mtx[1][1] = 1.0f - xx - zz;
    mtx[1][2] = yz - wx;

    mtx[2][0] = xz - wy;
    mtx[2][1] = yz + wx;
    mtx[2][2] = 1.0f - xx - yy;

    mtx[0][3] = t.x;
    mtx[1][3] = t.y;
    mtx[2][3] = t.z;
}

/// @addr{0x8023030C}
/// @brief Sets rotation matrix from quaternion.
void Matrix34f::makeQ(const Quatf &q) {
    f32 yy = 2.0f * q.v.y * q.v.y;
    f32 zz = 2.0f * q.v.z * q.v.z;
    f32 xx = 2.0f * q.v.x * q.v.x;
    f32 xy = 2.0f * q.v.x * q.v.y;
    f32 xz = 2.0f * q.v.x * q.v.z;
    f32 yz = 2.0f * q.v.y * q.v.z;
    f32 wz = 2.0f * q.w * q.v.z;
    f32 wx = 2.0f * q.w * q.v.x;
    f32 wy = 2.0f * q.w * q.v.y;

    mtx[0][0] = 1.0f - yy - zz;
    mtx[0][1] = xy - wz;
    mtx[0][2] = xz + wy;

    mtx[1][0] = xy + wz;
    mtx[1][1] = 1.0f - xx - zz;
    mtx[1][2] = yz - wx;

    mtx[2][0] = xz - wy;
    mtx[2][1] = yz + wx;
    mtx[2][2] = 1.0f - xx - yy;

    mtx[0][3] = 0.0f;
    mtx[1][3] = 0.0f;
    mtx[2][3] = 0.0f;
}

/// @addr{0x8022FE14}
/// @brief Sets rotation-translation matrix.
void Matrix34f::makeRT(const Vector3f &r, const Vector3f &t) {
    EGG::Vector3f s = EGG::Vector3f(sin(r.x), sin(r.y), sin(r.z));
    EGG::Vector3f c = EGG::Vector3f(cos(r.x), cos(r.y), cos(r.z));

    const f32 c0_c2 = c.x * c.z;
    const f32 s0_s1 = s.x * s.y;
    const f32 c0_s2 = c.x * s.z;

    mtx[0][0] = (c.y * c.z);
    mtx[1][0] = (c.y * s.z);
    mtx[2][0] = (-s.y);

    mtx[0][1] = (s0_s1 * c.z) - c0_s2;
    mtx[1][1] = (s0_s1 * s.z) + c0_c2;
    mtx[2][1] = (s.x * c.y);

    mtx[0][2] = (c0_c2 * s.y) + (s.x * s.z);
    mtx[1][2] = (c0_s2 * s.y) - (s.x * c.z);
    mtx[2][2] = (c.x * c.y);

    mtx[0][3] = t.x;
    mtx[1][3] = t.y;
    mtx[2][3] = t.z;
}

/// @addr{0x8022FF98}
/// @brief Sets 3x3 rotation matrix from a vector of Euler angles.
void Matrix34f::makeR(const Vector3f &r) {
    EGG::Vector3f s = EGG::Vector3f(sin(r.x), sin(r.y), sin(r.z));
    EGG::Vector3f c = EGG::Vector3f(cos(r.x), cos(r.y), cos(r.z));

    const f32 c0_c2 = c.x * c.z;
    const f32 s0_s1 = s.x * s.y;
    const f32 c0_s2 = c.x * s.z;

    mtx[0][0] = (c.y * c.z);
    mtx[1][0] = (c.y * s.z);
    mtx[2][0] = (-s.y);

    mtx[0][1] = (s0_s1 * c.z) - c0_s2;
    mtx[1][1] = (s0_s1 * s.z) + c0_c2;
    mtx[2][1] = (s.x * c.y);

    mtx[0][2] = (c0_c2 * s.y) + (s.x * s.z);
    mtx[1][2] = (c0_s2 * s.y) - (s.x * c.z);
    mtx[2][2] = (c.x * c.y);

    mtx[0][3] = 0.0f;
    mtx[1][3] = 0.0f;
    mtx[2][3] = 0.0f;
}

/// @brief Zeroes every element of the matrix.
void Matrix34f::makeZero() {
    *this = Matrix34f::zero;
}

/// @addr{0x802303BC}
/// @brief Rotates the matrix about an axis.
void Matrix34f::setAxisRotation(f32 angle, const EGG::Vector3f &axis) {
    EGG::Quatf q;
    q.setAxisRotation(angle, axis);
    makeQ(q);
}

/// @addr{0x80230410}
/// @brief Multiplies two matrices.
Matrix34f Matrix34f::multiplyTo(const Matrix34f &rhs) const {
    Matrix34f mat;

    mat[0, 0] = fma(rhs[2, 0], mtx[0][2], fma(rhs[1, 0], mtx[0][1], rhs[0, 0] * mtx[0][0]));
    mat[0, 1] = fma(rhs[2, 1], mtx[0][2], fma(rhs[1, 1], mtx[0][1], rhs[0, 1] * mtx[0][0]));
    mat[1, 0] = fma(rhs[2, 0], mtx[1][2], fma(rhs[1, 0], mtx[1][1], rhs[0, 0] * mtx[1][0]));
    mat[1, 1] = fma(rhs[2, 1], mtx[1][2], fma(rhs[1, 1], mtx[1][1], rhs[0, 1] * mtx[1][0]));
    mat[0, 2] = fma(rhs[2, 2], mtx[0][2], fma(rhs[1, 2], mtx[0][1], rhs[0, 2] * mtx[0][0]));
    mat[0, 3] = fma(1.0f, mtx[0][3],
            fma(rhs[2, 3], mtx[0][2], fma(rhs[1, 3], mtx[0][1], rhs[0, 3] * mtx[0][0])));
    mat[1, 2] = fma(rhs[2, 2], mtx[1][2], fma(rhs[1, 2], mtx[1][1], rhs[0, 2] * mtx[1][0]));
    mat[1, 3] = fma(1.0f, mtx[1][3],
            fma(rhs[2, 3], mtx[1][2], fma(rhs[1, 3], mtx[1][1], rhs[0, 3] * mtx[1][0])));
    mat[2, 0] = fma(rhs[2, 0], mtx[2][2], fma(rhs[1, 0], mtx[2][1], rhs[0, 0] * mtx[2][0]));
    mat[2, 1] = fma(rhs[2, 1], mtx[2][2], fma(rhs[1, 1], mtx[2][1], rhs[0, 1] * mtx[2][0]));
    mat[2, 2] = fma(rhs[2, 2], mtx[2][2], fma(rhs[1, 2], mtx[2][1], rhs[0, 2] * mtx[2][0]));
    mat[2, 3] = fma(1.0f, mtx[2][3],
            fma(rhs[2, 3], mtx[2][2], fma(rhs[1, 3], mtx[2][1], rhs[0, 3] * mtx[2][0])));

    return mat;
}

/// @brief Multiplies a vector by a matrix.
Vector3f Matrix34f::multVector(const Vector3f &vec) const {
    Vector3f ret;

    ret.x = mtx[0][0] * vec.x + mtx[0][3] + mtx[0][1] * vec.y + mtx[0][2] * vec.z;
    ret.y = mtx[1][0] * vec.x + mtx[1][3] + mtx[1][1] * vec.y + mtx[1][2] * vec.z;
    ret.z = mtx[2][0] * vec.x + mtx[2][3] + mtx[2][1] * vec.y + mtx[2][2] * vec.z;

    return ret;
}

/// @addr{0x802303F8}
/// @brief Paired-singles impl. of @ref multVector.
Vector3f Matrix34f::ps_multVector(const Vector3f &vec) const {
    Vector3f ret;

    ret.x = fma(mtx[0][2], vec.z, mtx[0][0] * vec.x) + fma(mtx[0][3], 1.0f, mtx[0][1] * vec.y);
    ret.y = fma(mtx[1][2], vec.z, mtx[1][0] * vec.x) + fma(mtx[1][3], 1.0f, mtx[1][1] * vec.y);
    ret.z = fma(mtx[2][2], vec.z, mtx[2][0] * vec.x) + fma(mtx[2][3], 1.0f, mtx[2][1] * vec.y);

    return ret;
}

/// @brief Multiplies a 3x3 matrix by a vector.
Vector3f Matrix34f::multVector33(const Vector3f &vec) const {
    Vector3f ret;

    ret.x = mtx[0][0] * vec.x + mtx[0][1] * vec.y + mtx[0][2] * vec.z;
    ret.y = mtx[1][0] * vec.x + mtx[1][1] * vec.y + mtx[1][2] * vec.z;
    ret.z = mtx[2][0] * vec.x + mtx[2][1] * vec.y + mtx[2][2] * vec.z;

    return ret;
}

/// @addr{0x8022F90C}
/// @brief Inverts the 3x3 portion of the 3x4 matrix.
/// @details Unlike a typical matrix inversion, if the determinant is 0, then this function returns
/// the identity matrix.
Matrix34f Matrix34f::inverseTo33() const {
    f32 determinant = ((((mtx[2][1] * (mtx[0][2] * mtx[1][0])) +
                                ((mtx[2][2] * (mtx[0][0] * mtx[1][1])) +
                                        (mtx[2][0] * (mtx[0][1] * mtx[1][2])))) -
                               (mtx[0][2] * (mtx[2][0] * mtx[1][1]))) -
                              (mtx[2][2] * (mtx[1][0] * mtx[0][1]))) -
            (mtx[1][2] * (mtx[0][0] * mtx[2][1]));

    if (determinant == 0.0f) {
        return Matrix34f::ident;
    }

    f32 invDet = 1.0f / determinant;

    Matrix34f ret;

    ret[0, 2] = (mtx[0][1] * mtx[1][2] - mtx[1][1] * mtx[0][2]) * invDet;
    ret[1, 2] = -(mtx[0][0] * mtx[1][2] - mtx[0][2] * mtx[1][0]) * invDet;
    ret[2, 1] = -(mtx[0][0] * mtx[2][1] - mtx[2][0] * mtx[0][1]) * invDet;
    ret[2, 2] = (mtx[0][0] * mtx[1][1] - mtx[1][0] * mtx[0][1]) * invDet;
    ret[2, 0] = (mtx[1][0] * mtx[2][1] - mtx[2][0] * mtx[1][1]) * invDet;
    ret[0, 0] = (mtx[1][1] * mtx[2][2] - mtx[2][1] * mtx[1][2]) * invDet;
    ret[0, 1] = -(mtx[0][1] * mtx[2][2] - mtx[2][1] * mtx[0][2]) * invDet;
    ret[1, 0] = -(mtx[1][0] * mtx[2][2] - mtx[2][0] * mtx[1][2]) * invDet;
    ret[1, 1] = (mtx[0][0] * mtx[2][2] - mtx[2][0] * mtx[0][2]) * invDet;

    return ret;
}

/// @brief Transposes the 3x3 portion of the matrix.
Matrix34f Matrix34f::transpose() const {
    Matrix34f ret = *this;

    ret[0, 1] = mtx[1][0];
    ret[0, 2] = mtx[2][0];
    ret[1, 0] = mtx[0][1];
    ret[1, 2] = mtx[2][1];
    ret[2, 0] = mtx[0][2];
    ret[2, 1] = mtx[1][2];

    return ret;
}

/// @addr{0x80384370}
const Matrix34f Matrix34f::ident(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.0f);

const Matrix34f Matrix34f::zero(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f);

} // namespace EGG
