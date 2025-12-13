#pragma once

#include "egg/math/Quat.hh"

namespace EGG {

using namespace Mathf;

/// @brief A 3 x 4 matrix.
class Matrix34f {
public:
#ifdef BUILD_DEBUG
    constexpr Matrix34f() {
        a.fill(std::numeric_limits<f32>::signaling_NaN());
    }
#else
    constexpr Matrix34f() = default;
#endif

    constexpr Matrix34f(f32 _e00, f32 _e01, f32 _e02, f32 _e03, f32 _e10, f32 _e11, f32 _e12,
            f32 _e13, f32 _e20, f32 _e21, f32 _e22, f32 _e23)
        : mtx{{_e00, _e01, _e02, _e03, _e10, _e11, _e12, _e13, _e20, _e21, _e22, _e23}} {}

    constexpr ~Matrix34f() = default;

    constexpr bool operator==(const Matrix34f &rhs) const {
        return mtx == rhs.mtx;
    }

    /// @brief Accesses the matrix element at the specified row and column.
    [[nodiscard]] constexpr f32 &operator[](size_t row, size_t col) {
        return mtx[row][col];
    }

    /// @brief Accesses the matrix element at the specified row and column.
    [[nodiscard]] constexpr f32 operator[](size_t row, size_t col) const {
        return mtx[row][col];
    }

    /// @addr{0x80230118}
    /// @brief Sets matrix from rotation and position.
    constexpr void makeQT(const Quatf &q, const Vector3f &t) {
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
    constexpr void makeQ(const Quatf &q) {
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
    constexpr void makeRT(const Vector3f &r, const Vector3f &t) {
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
    constexpr void makeR(const Vector3f &r) {
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

    /// @addr{0x80230280}
    constexpr void makeS(const Vector3f &s) {
        makeZero();
        mtx[0][0] = s.x;
        mtx[1][1] = s.y;
        mtx[2][2] = s.z;
    }

    /// @addr{0x802302C4}
    constexpr void makeT(const Vector3f &t) {
        mtx[0][0] = 1.0f;
        mtx[0][1] = 0.0f;
        mtx[0][2] = 0.0f;
        mtx[1][0] = 0.0f;
        mtx[1][1] = 1.0f;
        mtx[1][2] = 0.0f;
        mtx[2][0] = 0.0f;
        mtx[2][1] = 0.0f;
        mtx[2][2] = 1.0f;
        mtx[0][3] = t.x;
        mtx[1][3] = t.y;
        mtx[2][3] = t.z;
    }

    /// @brief Zeroes every element of the matrix.
    constexpr void makeZero() {
        *this = zero;
    }

    /// @addr{0x805AE7B4}
    /// @brief Sets a 3x3 orthonormal basis for a local coordinate system.
    /// @details In a vector's orthogonal space, there are infinitely many vector pairs orthogonal
    /// to each other. To specify a particular pair and create a local coordinate system, a
    /// reference up vector from an existing coordinate system is provided.
    /// @param forward The forward axis of the local coordinate system.
    /// @param up The reference up axis from an existing coordinate system.
    constexpr void makeOrthonormalBasis(const Vector3f &forward, const Vector3f &up) {
        Vector3f x = up.cross(forward);
        x.normalise();
        Vector3f y = forward.cross(x);
        y.normalise();

        setBase(0, x);
        setBase(1, y);
        setBase(2, forward);
    }

    /// @addr{0x802303BC}
    /// @brief Rotates the matrix about an axis.
    constexpr void setAxisRotation(f32 angle, const EGG::Vector3f &axis) {
        EGG::Quatf q;
        q.setAxisRotation(angle, axis);
        makeQ(q);
    }

    /// @brief Multiplies one row of a 3x3 matrix by a vector.
    constexpr void mulRow33(size_t rowIdx, const Vector3f &row) {
        mtx[rowIdx][0] *= row.x;
        mtx[rowIdx][1] *= row.y;
        mtx[rowIdx][2] *= row.z;
    }

    /// @brief Sets one column of a matrix.
    constexpr void setBase(size_t col, const Vector3f &base) {
        mtx[0][col] = base.x;
        mtx[1][col] = base.y;
        mtx[2][col] = base.z;
    }

    /// @addr{0x80230410} @addr{0x80199D64}
    /// @brief Multiplies two matrices.
    [[nodiscard]] constexpr Matrix34f multiplyTo(const Matrix34f &rhs) const {
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
    [[nodiscard]] constexpr Vector3f multVector(const Vector3f &vec) const {
        Vector3f ret;

        ret.x = mtx[0][0] * vec.x + mtx[0][3] + mtx[0][1] * vec.y + mtx[0][2] * vec.z;
        ret.y = mtx[1][0] * vec.x + mtx[1][3] + mtx[1][1] * vec.y + mtx[1][2] * vec.z;
        ret.z = mtx[2][0] * vec.x + mtx[2][3] + mtx[2][1] * vec.y + mtx[2][2] * vec.z;

        return ret;
    }

    /// @addr{0x802303F8}
    /// @brief Paired-singles impl. of @ref multVector.
    [[nodiscard]] constexpr Vector3f ps_multVector(const Vector3f &vec) const {
        Vector3f ret;

        ret.x = fma(mtx[0][2], vec.z, mtx[0][0] * vec.x) + fma(mtx[0][3], 1.0f, mtx[0][1] * vec.y);
        ret.y = fma(mtx[1][2], vec.z, mtx[1][0] * vec.x) + fma(mtx[1][3], 1.0f, mtx[1][1] * vec.y);
        ret.z = fma(mtx[2][2], vec.z, mtx[2][0] * vec.x) + fma(mtx[2][3], 1.0f, mtx[2][1] * vec.y);

        return ret;
    }

    /// @brief Multiplies a 3x3 matrix by a vector.
    /// @addr{0x8059A4F8}
    [[nodiscard]] constexpr Vector3f multVector33(const Vector3f &vec) const {
        Vector3f ret;

        ret.x = mtx[0][2] * vec.z + (mtx[0][0] * vec.x + mtx[0][1] * vec.y);
        ret.y = mtx[1][2] * vec.z + (mtx[1][0] * vec.x + mtx[1][1] * vec.y);
        ret.z = mtx[2][2] * vec.z + (mtx[2][0] * vec.x + mtx[2][1] * vec.y);

        return ret;
    }

    /// @addr{0x8019A970}
    /// @brief Paired-singles impl. of @ref multVector33.
    [[nodiscard]] constexpr Vector3f ps_multVector33(const Vector3f &vec) const {
        Vector3f ret;

        ret.x = fma(mtx[0][2], vec.z, mtx[0][0] * vec.x + mtx[0][1] * vec.y);
        ret.y = fma(mtx[1][2], vec.z, mtx[1][0] * vec.x + mtx[1][1] * vec.y);
        ret.z = fma(mtx[2][2], vec.z, mtx[2][0] * vec.x + mtx[2][1] * vec.y);

        return ret;
    }

    /// @addr{0x8022F90C}
    /// @brief Inverts the 3x3 portion of the 3x4 matrix.
    /// @details Unlike a typical matrix inversion, if the determinant is 0, then this function
    /// returns the identity matrix.
    constexpr void inverseTo33(Matrix34f &out) const {
        f32 determinant = ((((mtx[2][1] * (mtx[0][2] * mtx[1][0])) +
                                    ((mtx[2][2] * (mtx[0][0] * mtx[1][1])) +
                                            (mtx[2][0] * (mtx[0][1] * mtx[1][2])))) -
                                   (mtx[0][2] * (mtx[2][0] * mtx[1][1]))) -
                                  (mtx[2][2] * (mtx[1][0] * mtx[0][1]))) -
                (mtx[1][2] * (mtx[0][0] * mtx[2][1]));

        if (determinant == 0.0f) {
            out = ident;
            return;
        }

        f32 invDet = 1.0f / determinant;

        out[0, 2] = (mtx[0][1] * mtx[1][2] - mtx[1][1] * mtx[0][2]) * invDet;
        out[1, 2] = -(mtx[0][0] * mtx[1][2] - mtx[0][2] * mtx[1][0]) * invDet;
        out[2, 1] = -(mtx[0][0] * mtx[2][1] - mtx[2][0] * mtx[0][1]) * invDet;
        out[2, 2] = (mtx[0][0] * mtx[1][1] - mtx[1][0] * mtx[0][1]) * invDet;
        out[2, 0] = (mtx[1][0] * mtx[2][1] - mtx[2][0] * mtx[1][1]) * invDet;
        out[0, 0] = (mtx[1][1] * mtx[2][2] - mtx[2][1] * mtx[1][2]) * invDet;
        out[0, 1] = -(mtx[0][1] * mtx[2][2] - mtx[2][1] * mtx[0][2]) * invDet;
        out[1, 0] = -(mtx[1][0] * mtx[2][2] - mtx[2][0] * mtx[1][2]) * invDet;
        out[1, 1] = (mtx[0][0] * mtx[2][2] - mtx[2][0] * mtx[0][2]) * invDet;
    }

    /// @addr{0x80199FC8}
    /// @warn The out reference will not be initialized if the matrix is singular.
    /// @return Whether or not the matrix is invertible.
    constexpr bool ps_inverse(Matrix34f &out) const {
        f32 fVar14 = fms(mtx[0][1], mtx[1][2], mtx[1][1] * mtx[0][2]);
        f32 fVar15 = fms(mtx[1][1], mtx[2][2], mtx[2][1] * mtx[1][2]);
        f32 fVar13 = fms(mtx[2][1], mtx[0][2], mtx[0][1] * mtx[2][2]);
        f32 determinant = fma(mtx[2][0], fVar14, fma(mtx[1][0], fVar13, mtx[0][0] * fVar15));

        if (determinant == 0.0f) {
            return false;
        }

        f32 invDet = EGG::Mathf::finv(determinant);

        out[0, 0] = fVar15 * invDet;
        out[0, 1] = fVar13 * invDet;
        out[1, 0] = fms(mtx[1][2], mtx[2][0], mtx[2][2] * mtx[1][0]) * invDet;
        out[1, 1] = fms(mtx[2][2], mtx[0][0], mtx[0][2] * mtx[2][0]) * invDet;
        out[2, 0] = fms(mtx[1][0], mtx[2][1], mtx[1][1] * mtx[2][0]) * invDet;
        out[2, 1] = fms(mtx[0][1], mtx[2][0], mtx[0][0] * mtx[2][1]) * invDet;
        out[2, 2] = fms(mtx[0][0], mtx[1][1], mtx[0][1] * mtx[1][0]) * invDet;
        out[0, 2] = fVar14 * invDet;
        out[0, 3] = -fma(out[0, 2], mtx[2][3], fma(out[0, 1], mtx[1][3], out[0, 0] * mtx[0][3]));
        out[1, 2] = fms(mtx[0][2], mtx[1][0], mtx[1][2] * mtx[0][0]) * invDet;
        out[1, 3] = -fma(out[1, 2], mtx[2][3], fma(out[1, 1], mtx[1][3], out[1, 0] * mtx[0][3]));
        out[2, 3] = -fma(out[2, 2], mtx[2][3], fma(out[2, 1], mtx[1][3], out[2, 0] * mtx[0][3]));

        return true;
    }

    /// @brief Transposes the 3x3 portion of the matrix.
    [[nodiscard]] constexpr Matrix34f transpose() const {
        Matrix34f ret = *this;

        ret[0, 1] = mtx[1][0];
        ret[0, 2] = mtx[2][0];
        ret[1, 0] = mtx[0][1];
        ret[1, 2] = mtx[2][1];
        ret[2, 0] = mtx[0][2];
        ret[2, 1] = mtx[1][2];

        return ret;
    }

    [[nodiscard]] constexpr Vector3f translation() const {
        return Vector3f(mtx[0][3], mtx[1][3], mtx[2][3]);
    }

    /// @addr{0x80537B80}
    /// @brief Get a particular column from a matrix.
    [[nodiscard]] constexpr Vector3f base(size_t col) const {
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

/// @addr{0x80384370}
inline constexpr Matrix34f Matrix34f::ident(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f);

inline constexpr Matrix34f Matrix34f::zero(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f);

} // namespace EGG
