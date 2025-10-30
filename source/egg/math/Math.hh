#pragma once

#include <Common.hh>

#include <cmath>

static constexpr f32 F_PI = 3.1415927f;      ///< Floating point representation of pi
static constexpr f32 F_TAU = 6.2831855f;     ///< Floating point representation of tau/2pi
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
[[nodiscard]] std::pair<f32, f32> SinCosFIdx(f32 fidx);
[[nodiscard]] f32 AtanFIdx_(f32 fidx);
[[nodiscard]] f32 Atan2FIdx(f32 x, f32 y);

u32 FUN_800867C0(f32 a, f32 b, f32 c, f32 &root1, f32 &root2);

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

struct BaseAndDec32 {
    u32 base;
    s32 dec;
};

struct BaseAndDec64 {
    u64 base;
    s64 dec;
};

union c32 {
    constexpr c32(const f32 p) {
        f = p;
    }

    constexpr c32(const u32 p) {
        u = p;
    }

    u32 u;
    f32 f;
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

static constexpr std::array<BaseAndDec64, 32> RSQRTE_TABLE = {{
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

static constexpr std::array<BaseAndDec32, 32> FRES_TABLE = {{
        {0x00fff000UL, -0x3e1L},
        {0x00f07000UL, -0x3a7L},
        {0x00e1d400UL, -0x371L},
        {0x00d41000UL, -0x340L},
        {0x00c71000UL, -0x313L},
        {0x00bac400UL, -0x2eaL},
        {0x00af2000UL, -0x2c4L},
        {0x00a41000UL, -0x2a0L},
        {0x00999000UL, -0x27fL},
        {0x008f9400UL, -0x261L},
        {0x00861000UL, -0x245L},
        {0x007d0000UL, -0x22aL},
        {0x00745800UL, -0x212L},
        {0x006c1000UL, -0x1fbL},
        {0x00642800UL, -0x1e5L},
        {0x005c9400UL, -0x1d1L},
        {0x00555000UL, -0x1beL},
        {0x004e5800UL, -0x1acL},
        {0x0047ac00UL, -0x19bL},
        {0x00413c00UL, -0x18bL},
        {0x003b1000UL, -0x17cL},
        {0x00352000UL, -0x16eL},
        {0x002f5c00UL, -0x15bL},
        {0x0029f000UL, -0x15bL},
        {0x00248800UL, -0x143L},
        {0x001f7c00UL, -0x143L},
        {0x001a7000UL, -0x12dL},
        {0x0015bc00UL, -0x12dL},
        {0x00110800UL, -0x11aL},
        {0x000ca000UL, -0x11aL},
        {0x00083800UL, -0x108L},
        {0x00041800UL, -0x106L},
}};

[[nodiscard]] static inline f32 fres(const f32 val) {
    static constexpr u32 EXPONENT_SHIFT_F32 = 23;
    static constexpr u64 EXPONENT_SHIFT_F64 = 52;
    static constexpr u32 EXPONENT_MASK_F32 = 0x7f800000UL;
    static constexpr u64 EXPONENT_MASK_F64 = 0x7ff0000000000000ULL;
    static constexpr u32 MANTISSA_MASK_F32 = 0x007fffffUL;
    static constexpr u32 SIGN_MASK_F32 = 0x80000000UL;
    static constexpr u64 SIGN_MASK_F64 = 0x8000000000000000ULL;
    static constexpr u64 QUIET_BIT_F64 = 0x0008000000000000ULL;
    static constexpr c64 LARGEST_FLOAT(static_cast<u64>(0x47d0000000000000ULL));

    c64 bits(val);

    u32 mantissa = static_cast<u32>(
                           bits.u >> (EXPONENT_SHIFT_F64 - static_cast<u64>(EXPONENT_SHIFT_F32))) &
            MANTISSA_MASK_F32;
    s32 exponent = static_cast<s32>((bits.u & EXPONENT_MASK_F64) >> EXPONENT_SHIFT_F64) - 0x380;
    u32 sign = static_cast<u32>(bits.u >> 32) & SIGN_MASK_F32;

    if (exponent < -1) {
        bool nonzero = (bits.u & !SIGN_MASK_F64) != 0;

        if (nonzero) {
            // Create the largest normal number. It'll be a better approximation than infinity.
            c32 cresult(sign | (EXPONENT_MASK_F32 - (1 << EXPONENT_SHIFT_F32)) | MANTISSA_MASK_F32);
            return cresult.f;
        } else {
            return std::copysignf(std::numeric_limits<f32>::infinity(), val);
        }
    }

    if ((bits.u & EXPONENT_MASK_F64) >= LARGEST_FLOAT.u) {
        if (mantissa == 0 || (bits.u & EXPONENT_MASK_F64) != EXPONENT_MASK_F64) {
            // Infinity or a number which will flush to 0 -- return 0
            return std::copysignf(0.0f, val);
        } else if ((bits.u & QUIET_BIT_F64) != 0) {
            // QNaN -- Just return the original value
            return val;
        } else {
            // SNaN!!
            return std::numeric_limits<f32>::quiet_NaN();
        }
    }

    // Exponent doesn't matter for simple reciprocal because the exponent is a single multiplication
    u32 key = mantissa >> 18;
    s32 new_exp = 253 - exponent;
    const auto &entry = FRES_TABLE[key];

    // The result is given by an estimate and adjusted based on the original key that was computed
    u32 pre_shift =
            static_cast<u32>(entry.base + entry.dec * (static_cast<s32>((mantissa >> 8) & 0x3ff)));
    u32 new_mantissa = pre_shift >> 1;

    if (new_exp <= 0) {
        // Flush the denormal
        c32 cresult(sign);
        return cresult.f;
    } else {
        u32 temp = sign | static_cast<u32>(new_exp) << EXPONENT_SHIFT_F32 | new_mantissa;
        c32 cresult(temp);
        return cresult.f;
    }
}

/// @brief Fused Newton-Raphson operation.
[[nodiscard]] static inline f32 finv(f32 x) {
    f32 inv = fres(x);
    f32 invDouble = inv + inv;
    f32 invSquare = inv * inv;
    return -fms(x, invSquare, invDouble);
}

} // namespace EGG::Mathf
