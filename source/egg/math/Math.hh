#pragma once

#include <Common.hh>

#define F_PI 3.1415927f
#define DEG2RAD (F_PI / 180.0f)
#define DEG2FIDX (256.0f / 360.0f)
#define RAD2FIDX (128.0f / F_PI)

namespace EGG::Mathf {

f32 sqrt(f32 x);
f32 frsqrt(f32 x);

f32 SinFIdx(f32 fidx);
f32 CosFIdx(f32 fidx);
f32 sin(f32 x);
f32 cos(f32 x);

// sin/cos struct
struct SinCosEntry {
    f32 sinVal, cosVal, sinDt, cosDt;
};

// frsqrte matching
struct BaseAndDec {
    int m_base;
    int m_dec;
};

union c64 {
    c64(const f64 p) {
        f = p;
    }

    u64 _hex() const {
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

static inline f64 frsqrte(const f64 val) {
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
    input.u |= static_cast<uint64_t>(entry.m_base - entry.m_dec * (i % 2048)) << 26;

    return input.f;
}

} // namespace EGG::Mathf
