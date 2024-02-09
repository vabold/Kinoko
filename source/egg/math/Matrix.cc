#include "Matrix.hh"

#include "egg/math/Math.hh"

namespace EGG {

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

// Credit: elijah-thomas774/bba-wd
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
    mtx[2][2] = 1 - xx - yy;

    mtx[0][3] = 0.0f;
    mtx[1][3] = 0.0f;
    mtx[2][3] = 0.0f;
}

void Matrix34f::makeRT(const Vector3f &r, const Vector3f &t) {
    const f32 sin[3] = {Mathf::sin(r.x), Mathf::sin(r.y), Mathf::sin(r.z)};
    const f32 cos[3] = {Mathf::cos(r.x), Mathf::cos(r.y), Mathf::cos(r.z)};

    const f32 c0_c2 = cos[0] * cos[2];
    const f32 s0_s1 = sin[0] * sin[1];
    const f32 c0_s2 = cos[0] * sin[2];

    mtx[0][0] = (cos[1] * cos[2]);
    mtx[1][0] = (cos[1] * sin[2]);
    mtx[2][0] = (-sin[1]);

    mtx[0][1] = (s0_s1 * cos[2]) - c0_s2;
    mtx[1][1] = (s0_s1 * sin[2]) + c0_c2;
    mtx[2][1] = (sin[0] * cos[1]);

    mtx[0][2] = (c0_c2 * sin[1]) + (sin[0] * sin[2]);
    mtx[1][2] = (c0_s2 * sin[1]) - (sin[0] * cos[2]);
    mtx[2][2] = (cos[0] * cos[1]);

    mtx[0][3] = t.x;
    mtx[1][3] = t.y;
    mtx[2][3] = t.z;
}

void Matrix34f::makeR(const Vector3f &r) {
    const f32 sin[3] = {Mathf::sin(r.x), Mathf::sin(r.y), Mathf::sin(r.z)};
    const f32 cos[3] = {Mathf::cos(r.x), Mathf::cos(r.y), Mathf::cos(r.z)};

    const f32 c0_c2 = cos[0] * cos[2];
    const f32 s0_s1 = sin[0] * sin[1];
    const f32 c0_s2 = cos[0] * sin[2];

    mtx[0][0] = (cos[1] * cos[2]);
    mtx[1][0] = (cos[1] * sin[2]);
    mtx[2][0] = (-sin[1]);

    mtx[0][1] = (s0_s1 * cos[2]) - c0_s2;
    mtx[1][1] = (s0_s1 * sin[2]) + c0_c2;
    mtx[2][1] = (sin[0] * cos[1]);

    mtx[0][2] = (c0_c2 * sin[1]) + (sin[0] * sin[2]);
    mtx[1][2] = (c0_s2 * sin[1]) - (sin[0] * cos[2]);
    mtx[2][2] = (cos[0] * cos[1]);

    mtx[0][3] = 0.0f;
    mtx[1][3] = 0.0f;
    mtx[2][3] = 0.0f;
}

void Matrix34f::makeZero() {
    *this = Matrix34f::zero;
}

Matrix34f Matrix34f::multiplyTo(const Matrix34f &rhs) const {
    Matrix34f mat;

    mat(0, 0) = Mathf::fma(rhs(2, 0), mtx[0][2],
            Mathf::fma(rhs(1, 0), mtx[0][1], rhs(0, 0) * mtx[0][0]));
    mat(0, 1) = Mathf::fma(rhs(2, 1), mtx[0][2],
            Mathf::fma(rhs(1, 1), mtx[0][1], rhs(0, 1) * mtx[0][0]));

    mat(1, 0) = Mathf::fma(rhs(2, 0), mtx[1][2],
            Mathf::fma(rhs(1, 0), mtx[1][1], rhs(0, 0) * mtx[1][0]));

    mat(1, 1) = Mathf::fma(rhs(2, 1), mtx[1][2],
            Mathf::fma(rhs(1, 1), mtx[1][1], rhs(0, 1) * mtx[1][0]));

    mat(0, 2) = Mathf::fma(rhs(2, 2), mtx[0][2],
            Mathf::fma(rhs(1, 2), mtx[0][1], rhs(0, 2) * mtx[0][0]));
    mat(0, 3) = Mathf::fma(1.0f, mtx[0][3],
            Mathf::fma(rhs(2, 3), mtx[0][2],
                    Mathf::fma(rhs(1, 3), mtx[0][1], rhs(0, 3) * mtx[0][0])));
    mat(1, 2) = mtx[1][3] +
            Mathf::fma(rhs(2, 2), mtx[1][2],
                    Mathf::fma(rhs(1, 2), mtx[1][1], rhs(0, 2) * mtx[1][0]));
    mat(1, 3) = Mathf::fma(1.0f, mtx[1][3],
            Mathf::fma(rhs(2, 3), mtx[1][2],
                    Mathf::fma(rhs(1, 3), mtx[1][1], rhs(0, 3) * mtx[1][0])));
    mat(2, 0) = Mathf::fma(rhs(2, 0), mtx[2][2],
            Mathf::fma(rhs(1, 0), mtx[2][1], rhs(0, 0) * mtx[2][0]));
    mat(2, 1) = Mathf::fma(rhs(2, 1), mtx[2][2],
            Mathf::fma(rhs(1, 1), mtx[2][1], rhs(0, 1) * mtx[2][0]));
    mat(2, 2) = mtx[2][3] +
            Mathf::fma(rhs(2, 2), mtx[2][2],
                    Mathf::fma(rhs(1, 2), mtx[2][1], rhs(0, 2) * mtx[2][0]));
    mat(2, 3) = Mathf::fma(1.0f, mtx[2][3],
            Mathf::fma(rhs(2, 3), mtx[2][2],
                    Mathf::fma(rhs(1, 3), mtx[2][1], rhs(0, 3) * mtx[2][0])));

    return mat;
}

Vector3f Matrix34f::multVector(const Vector3f &vec) const {
    Vector3f ret;

    ret.x = mtx[0][2] * vec.z + mtx[0][0] * vec.x + mtx[0][3] + mtx[0][1] * vec.y;
    ret.y = mtx[1][2] * vec.z + mtx[1][0] * vec.x + mtx[1][3] + mtx[1][1] * vec.y;
    ret.z = mtx[2][2] * vec.z + mtx[2][0] * vec.x + mtx[2][3] + mtx[2][1] * vec.y;

    return ret;
}

Vector3f Matrix34f::ps_multVector(const Vector3f &vec) const {
    Vector3f ret;

    ret.x = Mathf::fma(mtx[0][2], vec.z, mtx[0][0] * vec.x) +
            Mathf::fma(mtx[0][3], 1.0f, mtx[0][1] * vec.y);
    ret.y = Mathf::fma(mtx[1][2], vec.z, mtx[1][0] * vec.x) +
            Mathf::fma(mtx[1][3], 1.0f, mtx[1][1] * vec.y);
    ret.z = Mathf::fma(mtx[2][2], vec.z, mtx[2][0] * vec.x) +
            Mathf::fma(mtx[2][3], 1.0f, mtx[2][1] * vec.y);

    return ret;
}

Vector3f Matrix34f::multVector33(const Vector3f &vec) const {
    Vector3f ret;

    ret.x = mtx[0][2] * vec.z + mtx[0][0] * vec.x + mtx[0][1] * vec.y;

    ret.y = mtx[1][2] * vec.z + mtx[1][0] * vec.x + mtx[1][1] * vec.y;
    ret.z = mtx[2][2] * vec.z + mtx[2][0] * vec.x + mtx[2][1] * vec.y;

    return ret;
}

Matrix34f Matrix34f::inverseTo() const {
    f32 determinant = (((mtx[2][1] * mtx[0][2] * mtx[1][0] + mtx[2][2] * mtx[0][0] * mtx[1][1] +
                                mtx[2][0] * mtx[0][1] * mtx[1][2]) -
                               mtx[0][2] * mtx[2][0] * mtx[1][1]) -
                              mtx[2][2] * mtx[1][0] * mtx[0][1]) -
            mtx[1][2] * mtx[0][0] * mtx[2][1];

    if (determinant == 0.0f) {
        return Matrix34f::ident;
    }

    f32 invDet = 1.0f / determinant;

    Matrix34f ret;

    ret(0, 2) = (mtx[0][1] * mtx[1][2] - mtx[1][1] * mtx[0][2]) * invDet;
    ret(1, 2) = -(mtx[0][0] * mtx[1][2] - mtx[0][2] * mtx[1][0]) * invDet;
    ret(2, 1) = -(mtx[0][0] * mtx[2][1] - mtx[2][0] * mtx[0][1]) * invDet;
    ret(2, 2) = (mtx[0][0] * mtx[1][1] - mtx[1][0] * mtx[0][1]) * invDet;
    ret(2, 0) = (mtx[1][0] * mtx[2][1] - mtx[2][0] * mtx[1][1]) * invDet;
    ret(0, 0) = (mtx[1][1] * mtx[2][2] - mtx[2][1] * mtx[1][2]) * invDet;
    ret(0, 1) = -(mtx[0][1] * mtx[2][2] - mtx[2][1] * mtx[0][2]) * invDet;
    ret(1, 0) = -(mtx[1][0] * mtx[2][2] - mtx[2][0] * mtx[1][2]) * invDet;
    ret(1, 1) = (mtx[0][0] * mtx[2][2] - mtx[2][0] * mtx[0][2]) * invDet;

    return ret;
}

void Matrix34f::setAxisRotation(f32 angle, const EGG::Vector3f &axis) {
    EGG::Quatf q;
    q.setAxisRotation(angle, axis);
    makeQ(q);
}

const Matrix34f Matrix34f::ident(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.0f);

const Matrix34f Matrix34f::zero(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f);

} // namespace EGG
