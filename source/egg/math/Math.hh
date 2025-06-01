#pragma once

#include <Common.hh>

#include <cmath>

static constexpr f32 F_PI = 3.1415927f;      ///< Floating point representation of pi
static constexpr f32 F_TAU = 6.283185f;      ///< Floating point representation of tau/2pi
static constexpr f32 DEG2RAD = 0.017453292f; ///< F_PI / 180.0f. Double precision and casted down.
static constexpr f32 DEG2RAD360 = 0.034906585f; ///< F_PI / 360.0f. Double precision, casted down.
static constexpr f32 RAD2DEG = 57.2957795f; ///< 180.0f / F_PI. Double precision and casted down.
static constexpr f32 DEG2FIDX = 256.0f / 360.0f; ///< Degrees to fixed index
static constexpr f32 RAD2FIDX = 128.0f / F_PI;   ///< Radians to fixed index
static constexpr f32 FIDX2RAD = F_PI / 128.0f;   ///< Fixed index to radians

/// @brief Math functions and constants used in the base game.
namespace EGG::Mathf {

[[nodiscard]] f32 frsqrt(f32 x);

/// @addr{0x8022F80C}
[[nodiscard]] static inline f32 sqrt(f32 x) {
    return x > 0.0f ? frsqrt(x) * x : 0.0f;
}

[[nodiscard]] f32 SinFIdx(f32 fidx);
[[nodiscard]] f32 CosFIdx(f32 fidx);
[[nodiscard]] f32 AtanFIdx_(f32 fidx);
[[nodiscard]] f32 Atan2FIdx(f32 x, f32 y);

/// Takes in radians
/// @addr{0x8022F860}
[[nodiscard]] static inline f32 sin(f32 x) {
    return SinFIdx(x * RAD2FIDX);
}

/// Takes in radians
/// @addr{0x8022F86C}
[[nodiscard]] static inline f32 cos(f32 x) {
    return CosFIdx(x * RAD2FIDX);
}

/// @addr{0x8022F8C0}
[[nodiscard]] static inline f32 acos(f32 x) {
    return ::acosl(x);
}

/// @addr{0x8022F8E4}
[[nodiscard]] static inline f32 atan2(f32 y, f32 x) {
    return Atan2FIdx(y, x) * FIDX2RAD;
}

[[nodiscard]] static inline f32 abs(f32 x) {
    return std::abs(x);
}

/// @brief This is used to mimic the Wii's floating-point unit.
/// @details This handles the edgecase where double-precision floating-point numbers are passed into
/// single-precision floating-point operands in assembly.
[[nodiscard]] static inline f64 force25Bit(f64 x) {
    u64 bits = std::bit_cast<u64>(x);
    bits = (bits & 0xfffffffff8000000ULL) + (bits & 0x8000000);
    return std::bit_cast<f64>(bits);
}

/// @brief Fused multiply-add operation.
/// @details We cannot use std::fma due to the Wii computing at 64-bit precision.
[[nodiscard]] static inline f32 fma(f32 x, f32 y, f32 z) {
    return static_cast<f32>(
            static_cast<f64>(x) * force25Bit(static_cast<f64>(y)) + static_cast<f64>(z));
}

/// @brief Fused multiply-subtract operation.
/// @details We cannot use std::fms due to the Wii computing at 64-bit precision.
[[nodiscard]] static inline f32 fms(f32 x, f32 y, f32 z) {
    return static_cast<f32>(
            static_cast<f64>(x) * force25Bit(static_cast<f64>(y)) - static_cast<f64>(z));
}

// frsqrte matching courtesy of Geotale, with reference to https://achurch.org/cpu-tests/ppc750cl.s
struct BaseAndDec {
    u64 base;
    s64 dec;
};

union c64 {
    constexpr c64(const f64 p) {
        f = p;
    }

    constexpr c64(const u64 p) {
        u = p;
    }

    u64 u;
    f64 f;
};

static constexpr u64 EXPONENT_SHIFT_F64 = 52;
static constexpr u64 MANTISSA_MASK_F64 = 0x000fffffffffffffULL;
static constexpr u64 EXPONENT_MASK_F64 = 0x7ff0000000000000ULL;
static constexpr u64 SIGN_MASK_F64 = 0x8000000000000000ULL;

static constexpr std::array<BaseAndDec, 32> RSQRTE_TABLE = {{
        {0x69fa000000000ULL, -0x15a0000000LL},
        {0x5f2e000000000ULL, -0x13cc000000LL},
        {0x554a000000000ULL, -0x1234000000LL},
        {0x4c30000000000ULL, -0x10d4000000LL},
        {0x43c8000000000ULL, -0x0f9c000000LL},
        {0x3bfc000000000ULL, -0x0e88000000LL},
        {0x34b8000000000ULL, -0x0d94000000LL},
        {0x2df0000000000ULL, -0x0cb8000000LL},
        {0x2794000000000ULL, -0x0bf0000000LL},
        {0x219c000000000ULL, -0x0b40000000LL},
        {0x1bfc000000000ULL, -0x0aa0000000LL},
        {0x16ae000000000ULL, -0x0a0c000000LL},
        {0x11a8000000000ULL, -0x0984000000LL},
        {0x0ce6000000000ULL, -0x090c000000LL},
        {0x0862000000000ULL, -0x0898000000LL},
        {0x0416000000000ULL, -0x082c000000LL},
        {0xffe8000000000ULL, -0x1e90000000LL},
        {0xf0a4000000000ULL, -0x1c00000000LL},
        {0xe2a8000000000ULL, -0x19c0000000LL},
        {0xd5c8000000000ULL, -0x17c8000000LL},
        {0xc9e4000000000ULL, -0x1610000000LL},
        {0xbedc000000000ULL, -0x1490000000LL},
        {0xb498000000000ULL, -0x1330000000LL},
        {0xab00000000000ULL, -0x11f8000000LL},
        {0xa204000000000ULL, -0x10e8000000LL},
        {0x9994000000000ULL, -0x0fe8000000LL},
        {0x91a0000000000ULL, -0x0f08000000LL},
        {0x8a1c000000000ULL, -0x0e38000000LL},
        {0x8304000000000ULL, -0x0d78000000LL},
        {0x7c48000000000ULL, -0x0cc8000000LL},
        {0x75e4000000000ULL, -0x0c28000000LL},
        {0x6fd0000000000ULL, -0x0b98000000LL},
}};

[[nodiscard]] static inline f64 frsqrte(const f64 val) {
    c64 bits(val);

    u64 mantissa = bits.u & MANTISSA_MASK_F64;
    s64 exponent = bits.u & EXPONENT_MASK_F64;
    bool sign = (bits.u & SIGN_MASK_F64) != 0;

    // Handle 0 case
    if (mantissa == 0 && exponent == 0) {
        return std::copysign(std::numeric_limits<f64>::infinity(), bits.f);
    }

    // Handle NaN-like
    if (exponent == EXPONENT_MASK_F64) {
        if (mantissa == 0) {
            return sign ? std::numeric_limits<f64>::quiet_NaN() : 0.0;
        }

        return val;
    }

    // Handle negative inputs
    if (sign) {
        return std::numeric_limits<f64>::quiet_NaN();
    }

    if (exponent == 0) {
        // Shift so one bit goes to where the exponent would be,
        // then clear that bit to mimick a not-subnormal number!
        // Aka, if there are 12 leading zeroes, shift left once
        u32 shift = std::countl_zero(mantissa) - static_cast<u32>(63 - EXPONENT_SHIFT_F64);

        mantissa <<= shift;
        mantissa &= MANTISSA_MASK_F64;
        // The shift is subtracted by 1 because denormals by default
        // are offset by 1 (exponent 0 doesn't have implied 1 bit)
        exponent -= static_cast<s64>(shift - 1) << EXPONENT_SHIFT_F64;
    }

    // In reality this doesn't get the full exponent -- Only the least significant bit
    // Only that's needed because square roots of higher exponent bits simply multiply the
    // result by 2!!
    u32 key = static_cast<u32>((static_cast<u64>(exponent) | mantissa) >> 37);
    u64 new_exp =
            (static_cast<u64>((0xbfcLL << EXPONENT_SHIFT_F64) - exponent) >> 1) & EXPONENT_MASK_F64;

    // Remove the bits relating to anything higher than the LSB of the exponent
    const auto &entry = RSQRTE_TABLE[0x1f & (key >> 11)];

    // The result is given by an estimate then an adjustment based on the original
    // key that was computed
    u64 new_mantissa = static_cast<u64>(entry.base + entry.dec * static_cast<s64>(key & 0x7ff));

    return c64(new_exp | new_mantissa).f;
}

} // namespace EGG::Mathf
