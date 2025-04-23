#pragma once

#include "egg/math/Quat.hh"

namespace EGG {

/// @brief A 3 x 4 matrix.
class Matrix34f {
public:
    Matrix34f();
    Matrix34f(f32 _e00, f32 _e01, f32 _e02, f32 _e03, f32 _e10, f32 _e11, f32 _e12, f32 _e13,
            f32 _e20, f32 _e21, f32 _e22, f32 _e23);
    ~Matrix34f() = default;

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
    void makeS(const Vector3f &s);

    /// @brief Zeroes every element of the matrix.
    void makeZero() {
        *this = Matrix34f::zero;
    }

    void makeOrthonormalBasis(const Vector3f &v0, const Vector3f &v1);
    void setAxisRotation(f32 angle, const Vector3f &axis);
    void mulRow33(size_t rowIdx, const Vector3f &row);
    void setBase(size_t col, const Vector3f &base);

    [[nodiscard]] Matrix34f multiplyTo(const Matrix34f &rhs) const;
    [[nodiscard]] Vector3f multVector(const Vector3f &vec) const;
    [[nodiscard]] Vector3f ps_multVector(const Vector3f &vec) const;
    [[nodiscard]] Vector3f multVector33(const Vector3f &vec) const;
    [[nodiscard]] Vector3f ps_multVector33(const Vector3f &vec) const;
    void inverseTo33(Matrix34f &out) const;
    bool ps_inverse(Matrix34f &out) const;
    [[nodiscard]] Matrix34f transpose() const;

    [[nodiscard]] Vector3f translation() const {
        return Vector3f(mtx[0][3], mtx[1][3], mtx[2][3]);
    }

    /// @addr{0x80537B80}
    /// @brief Get a particular column from a matrix.
    [[nodiscard]] Vector3f base(size_t col) const {
        return Vector3f(mtx[0][col], mtx[1][col], mtx[2][col]);
    }

    static const Matrix34f ident;
    static const Matrix34f zero;

private:
    union {
        std::array<std::array<f32, 4>, 3> mtx;
        std::array<f32, 12> a;
    };
};

} // namespace EGG
