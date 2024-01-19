#pragma once

#include "egg/math/Quat.hh"

namespace EGG {

class Matrix34f {
public:
    Matrix34f();
    Matrix34f(f32 _e00, f32 _e01, f32 _e02, f32 _e03, f32 _e10, f32 _e11, f32 _e12, f32 _e13,
            f32 _e20, f32 _e21, f32 _e22, f32 _e23);
    ~Matrix34f();

    f32 &operator()(int i, int j) {
        return mtx[i][j];
    }

    // Q for Quaternion, T for translation
    void makeQT(const Quatf &q, const Vector3f &t);
    void makeZero();

    static const Matrix34f ident;

private:
    union {
        std::array<std::array<f32, 4>, 3> mtx;
        std::array<f32, 12> a;
    };
};

} // namespace EGG
