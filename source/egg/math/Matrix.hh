#pragma once

#include "egg/math/Quat.hh"

namespace EGG {

/// @brief A 3 x 4 matrix.
class Matrix34f {
public:
    Matrix34f();
    Matrix34f(f32 _e00, f32 _e01, f32 _e02, f32 _e03, f32 _e10, f32 _e11, f32 _e12, f32 _e13,
            f32 _e20, f32 _e21, f32 _e22, f32 _e23);
    ~Matrix34f();

    bool operator==(const Matrix34f &rhs) const {
        return mtx == rhs.mtx;
    }

    /// @brief Accesses the matrix element at the specified row and column.
    [[nodiscard]] f32 &operator[](size_t row, size_t col) {
        return mtx[row][col];
    }

    /// @brief Accesses the matrix element at the specified row and column.
    [[nodiscard]] f32 operator[](size_t row, size_t col) const {
        return mtx[row][col];
    }

    void makeQT(const Quatf &q, const Vector3f &t);
    void makeQ(const Quatf &q);
    void makeRT(const Vector3f &r, const Vector3f &t);
    void makeR(const Vector3f &r);
    void makeZero();
    void setAxisRotation(f32 angle, const Vector3f &axis);

    [[nodiscard]] Matrix34f multiplyTo(const Matrix34f &rhs) const;
    [[nodiscard]] Vector3f multVector(const Vector3f &vec) const;
    [[nodiscard]] Vector3f ps_multVector(const Vector3f &vec) const;
    [[nodiscard]] Vector3f multVector33(const Vector3f &vec) const;
    [[nodiscard]] Matrix34f inverseTo33() const;
    [[nodiscard]] Matrix34f transpose() const;

    static const Matrix34f ident;
    static const Matrix34f zero;

private:
    union {
        std::array<std::array<f32, 4>, 3> mtx;
        std::array<f32, 12> a;
    };
};

} // namespace EGG
