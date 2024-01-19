#include "Matrix.hh"

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

void Matrix34f::makeQT(const Quatf &q, const Vector3f &t) {
    mtx[0][0] = 1.0f - 2.0f * (q.v.y * q.v.y + q.v.z * q.v.z);
    mtx[0][1] = 2.0f * (q.v.x * q.v.y - q.w * q.v.z);
    mtx[0][2] = 2.0f * (q.v.x * q.v.z + q.w * q.v.y);
    mtx[0][3] = t.x;
    mtx[1][0] = 2.0f * (q.v.x * q.v.y + q.w * q.v.y);
    mtx[1][1] = 1.0 - 2.0f * (q.v.x * q.v.x + q.v.z * q.v.z);
    mtx[1][2] = 2.0f * (q.v.y * q.v.z - q.w * q.v.x);
    mtx[1][3] = t.y;
    mtx[2][0] = 2.0f * (q.v.x * q.v.z - q.w * q.v.y);
    mtx[2][1] = 2.0f * (q.v.y * q.v.z + q.w * q.v.x);
    mtx[2][2] = 1.0 - 2.0f * (q.v.x * q.v.x + q.v.y * q.v.y);
    mtx[2][3] = t.z;
}

void Matrix34f::makeZero() {
    for (auto &n : a) {
        n = 0.0f;
    }
}

const Matrix34f Matrix34f::ident(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.0f);

} // namespace EGG
