#pragma once

#include <Common.hh>

static constexpr f32 F_PI = 3.1415927f;      ///< Floating point representation of pi
static constexpr f32 DEG2RAD = 0.017453292f; ///< F_PI / 180.0f. Double precision and casted down.
static constexpr f32 DEG2RAD360 = 0.034906585f; ///< F_PI / 360.0f. Double precision, casted down.
static constexpr f32 RAD2DEG = 57.2957795f; ///< 180.0f / F_PI. Double precision and casted down.
static constexpr f32 DEG2FIDX = 256.0f / 360.0f; ///< Degrees to fixed index
static constexpr f32 RAD2FIDX = 128.0f / F_PI;   ///< Radians to fixed index
static constexpr f32 FIDX2RAD = F_PI / 128.0f;   ///< Fixed index to radians

/// @brief Math functions and constants used in the base game.
namespace EGG::Mathf {

[[nodiscard]] f32 sqrt(f32 x);
[[nodiscard]] f32 frsqrt(f32 x);

[[nodiscard]] f32 SinFIdx(f32 fidx);
[[nodiscard]] f32 CosFIdx(f32 fidx);
[[nodiscard]] std::pair<f32, f32> SinCosFIdx(f32 fidx);
[[nodiscard]] f32 AtanFIdx_(f32 fidx);
[[nodiscard]] f32 Atan2FIdx(f32 x, f32 y);
[[nodiscard]] f32 sin(f32 x);
[[nodiscard]] f32 cos(f32 x);
[[nodiscard]] f32 acos(f32 x);
[[nodiscard]] f32 atan2(f32 x, f32 y);

[[nodiscard]] f32 abs(f32 x);

[[nodiscard]] f32 fma(f32 x, f32 y, f32 z);

[[nodiscard]] f64 force25Bit(f64 x);

// frsqrte matching
struct BaseAndDec {
    int base;
    int dec;
};

union c64 {
    c64(const f64 p) {
        f = p;
    }

    [[nodiscard]] u64 _hex() const {
        return u;
    }

    u64 u;
    f64 f;
};

static const std::array<BaseAndDec, 32> frsqrte_expected = {{
        {0x3ffa000, 0x7a4},
        {0x3c29000, 0x700},
        {0x38aa000, 0x670},
        {0x3572000, 0x5f2},
        {0x3279000, 0x584},
        {0x2fb7000, 0x524},
        {0x2d26000, 0x4cc},
        {0x2ac0000, 0x47e},
        {0x2881000, 0x43a},
        {0x2665000, 0x3fa},
        {0x2468000, 0x3c2},
        {0x2287000, 0x38e},
        {0x20c1000, 0x35e},
        {0x1f12000, 0x332},
        {0x1d79000, 0x30a},
        {0x1bf4000, 0x2e6},
        {0x1a7e800, 0x568},
        {0x17cb800, 0x4f3},
        {0x1552800, 0x48d},
        {0x130c000, 0x435},
        {0x10f2000, 0x3e7},
        {0x0eff000, 0x3a2},
        {0x0d2e000, 0x365},
        {0x0b7c000, 0x32e},
        {0x09e5000, 0x2fc},
        {0x0867000, 0x2d0},
        {0x06ff000, 0x2a8},
        {0x05ab800, 0x283},
        {0x046a000, 0x261},
        {0x0339800, 0x243},
        {0x0218800, 0x226},
        {0x0105800, 0x20b},
}};

[[nodiscard]] static inline f64 frsqrte(const f64 val) {
    c64 input(val);

    u64 mantissa = input._hex() & ((1LL << 52) - 1);
    const u64 sign = input._hex() & (1ULL << 63);
    u64 exponent = input._hex() & (0x7FFLL << 52);

    // Special case 0
    if (mantissa == 0 && exponent == 0) {
        return sign ? -std::numeric_limits<f64>::infinity() : std::numeric_limits<f64>::infinity();
    }

    // Special case NaN-ish numbers
    if (exponent == (0x7FFLL << 52)) {
        if (mantissa == 0) {
            if (sign) {
                return std::numeric_limits<f64>::quiet_NaN();
            }

            return 0.0;
        }

        return 0.0 + val;
    }

    // Negative numbers return NaN
    if (sign) {
        return std::numeric_limits<f64>::quiet_NaN();
    }

    if (!exponent) {
        // "Normalize" denormal values
        do {
            exponent -= 1LL << 52;
            mantissa <<= 1;
        } while (!(mantissa & (1LL << 52)));
        mantissa &= (1LL << 52) - 1;
        exponent += 1LL << 52;
    }

    const bool odd_exponent = !(exponent & (1LL << 52));
    exponent = ((0x3FFLL << 52) - ((exponent - (0x3FELL << 52)) / 2)) & (0x7FFLL << 52);
    input.u = sign | exponent;

    const int i = static_cast<int>(mantissa >> 37);
    const int index = i / 2048 + (odd_exponent ? 16 : 0);
    const auto &entry = frsqrte_expected[index];
    input.u |= static_cast<uint64_t>(entry.base - entry.dec * (i % 2048)) << 26;

    return input.f;
}

} // namespace EGG::Mathf
