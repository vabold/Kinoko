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
static constexpr f32 HALF_PI = F_PI / 2.0f;

/// @brief Math functions and constants used in the base game.
namespace EGG::Mathf {

/// @brief This is used to mimic the Wii's floating-point unit.
/// @details This handles the edgecase where double-precision floating-point numbers are passed into
/// single-precision floating-point operands in assembly.
[[nodiscard]] static inline constexpr f64 force25Bit(f64 x) {
    u64 bits = std::bit_cast<u64>(x);
    bits = (bits & 0xfffffffff8000000ULL) + (bits & 0x8000000);
    return std::bit_cast<f64>(bits);
}

/// @brief Fused multiply-add operation.
/// @details We cannot use std::fma due to the Wii computing at 64-bit precision.
[[nodiscard]] static inline constexpr f32 fma(f32 x, f32 y, f32 z) {
    return static_cast<f32>(
            static_cast<f64>(x) * force25Bit(static_cast<f64>(y)) + static_cast<f64>(z));
}

/// @brief Fused multiply-subtract operation.
/// @details We cannot use std::fms due to the Wii computing at 64-bit precision.
[[nodiscard]] static inline constexpr f32 fms(f32 x, f32 y, f32 z) {
    return static_cast<f32>(
            static_cast<f64>(x) * force25Bit(static_cast<f64>(y)) - static_cast<f64>(z));
}

// sin/cos struct
struct SinCosEntry {
    f32 sinVal, cosVal, sinDt, cosDt;
};

// atan struct
struct AtanEntry {
    f32 atanVal, atanDt;
};

/// @addr{0x80248010}
static constexpr SinCosEntry sSinCosTbl[256 + 1] = {
        {0.000000f, 1.000000f, 0.024541f, -0.000301f},
        {0.024541f, 0.999699f, 0.024526f, -0.000903f},
        {0.049068f, 0.998795f, 0.024497f, -0.001505f},
        {0.073565f, 0.997290f, 0.024453f, -0.002106f},
        {0.098017f, 0.995185f, 0.024394f, -0.002705f},
        {0.122411f, 0.992480f, 0.024320f, -0.003303f},
        {0.146730f, 0.989177f, 0.024231f, -0.003899f},
        {0.170962f, 0.985278f, 0.024128f, -0.004492f},
        {0.195090f, 0.980785f, 0.024011f, -0.005083f},
        {0.219101f, 0.975702f, 0.023879f, -0.005671f},
        {0.242980f, 0.970031f, 0.023733f, -0.006255f},
        {0.266713f, 0.963776f, 0.023572f, -0.006836f},
        {0.290285f, 0.956940f, 0.023397f, -0.007412f},
        {0.313682f, 0.949528f, 0.023208f, -0.007984f},
        {0.336890f, 0.941544f, 0.023005f, -0.008551f},
        {0.359895f, 0.932993f, 0.022788f, -0.009113f},
        {0.382683f, 0.923880f, 0.022558f, -0.009670f},
        {0.405241f, 0.914210f, 0.022314f, -0.010220f},
        {0.427555f, 0.903989f, 0.022056f, -0.010765f},
        {0.449611f, 0.893224f, 0.021785f, -0.011303f},
        {0.471397f, 0.881921f, 0.021501f, -0.011834f},
        {0.492898f, 0.870087f, 0.021205f, -0.012358f},
        {0.514103f, 0.857729f, 0.020895f, -0.012875f},
        {0.534998f, 0.844854f, 0.020573f, -0.013384f},
        {0.555570f, 0.831470f, 0.020238f, -0.013885f},
        {0.575808f, 0.817585f, 0.019891f, -0.014377f},
        {0.595699f, 0.803208f, 0.019532f, -0.014861f},
        {0.615232f, 0.788346f, 0.019162f, -0.015336f},
        {0.634393f, 0.773010f, 0.018780f, -0.015802f},
        {0.653173f, 0.757209f, 0.018386f, -0.016258f},
        {0.671559f, 0.740951f, 0.017982f, -0.016704f},
        {0.689541f, 0.724247f, 0.017566f, -0.017140f},
        {0.707107f, 0.707107f, 0.017140f, -0.017566f},
        {0.724247f, 0.689541f, 0.016704f, -0.017982f},
        {0.740951f, 0.671559f, 0.016258f, -0.018386f},
        {0.757209f, 0.653173f, 0.015802f, -0.018780f},
        {0.773010f, 0.634393f, 0.015336f, -0.019162f},
        {0.788346f, 0.615232f, 0.014861f, -0.019532f},
        {0.803208f, 0.595699f, 0.014377f, -0.019891f},
        {0.817585f, 0.575808f, 0.013885f, -0.020238f},
        {0.831470f, 0.555570f, 0.013384f, -0.020573f},
        {0.844854f, 0.534998f, 0.012875f, -0.020895f},
        {0.857729f, 0.514103f, 0.012358f, -0.021205f},
        {0.870087f, 0.492898f, 0.011834f, -0.021501f},
        {0.881921f, 0.471397f, 0.011303f, -0.021785f},
        {0.893224f, 0.449611f, 0.010765f, -0.022056f},
        {0.903989f, 0.427555f, 0.010220f, -0.022314f},
        {0.914210f, 0.405241f, 0.009670f, -0.022558f},
        {0.923880f, 0.382683f, 0.009113f, -0.022788f},
        {0.932993f, 0.359895f, 0.008551f, -0.023005f},
        {0.941544f, 0.336890f, 0.007984f, -0.023208f},
        {0.949528f, 0.313682f, 0.007412f, -0.023397f},
        {0.956940f, 0.290285f, 0.006836f, -0.023572f},
        {0.963776f, 0.266713f, 0.006255f, -0.023733f},
        {0.970031f, 0.242980f, 0.005671f, -0.023879f},
        {0.975702f, 0.219101f, 0.005083f, -0.024011f},
        {0.980785f, 0.195090f, 0.004492f, -0.024128f},
        {0.985278f, 0.170962f, 0.003899f, -0.024231f},
        {0.989177f, 0.146730f, 0.003303f, -0.024320f},
        {0.992480f, 0.122411f, 0.002705f, -0.024394f},
        {0.995185f, 0.098017f, 0.002106f, -0.024453f},
        {0.997290f, 0.073565f, 0.001505f, -0.024497f},
        {0.998795f, 0.049068f, 0.000903f, -0.024526f},
        {0.999699f, 0.024541f, 0.000301f, -0.024541f},
        {1.000000f, 0.000000f, -0.000301f, -0.024541f},
        {0.999699f, -0.024541f, -0.000903f, -0.024526f},
        {0.998795f, -0.049068f, -0.001505f, -0.024497f},
        {0.997290f, -0.073565f, -0.002106f, -0.024453f},
        {0.995185f, -0.098017f, -0.002705f, -0.024394f},
        {0.992480f, -0.122411f, -0.003303f, -0.024320f},
        {0.989177f, -0.146730f, -0.003899f, -0.024231f},
        {0.985278f, -0.170962f, -0.004492f, -0.024128f},
        {0.980785f, -0.195090f, -0.005083f, -0.024011f},
        {0.975702f, -0.219101f, -0.005671f, -0.023879f},
        {0.970031f, -0.242980f, -0.006255f, -0.023733f},
        {0.963776f, -0.266713f, -0.006836f, -0.023572f},
        {0.956940f, -0.290285f, -0.007412f, -0.023397f},
        {0.949528f, -0.313682f, -0.007984f, -0.023208f},
        {0.941544f, -0.336890f, -0.008551f, -0.023005f},
        {0.932993f, -0.359895f, -0.009113f, -0.022788f},
        {0.923880f, -0.382683f, -0.009670f, -0.022558f},
        {0.914210f, -0.405241f, -0.010220f, -0.022314f},
        {0.903989f, -0.427555f, -0.010765f, -0.022056f},
        {0.893224f, -0.449611f, -0.011303f, -0.021785f},
        {0.881921f, -0.471397f, -0.011834f, -0.021501f},
        {0.870087f, -0.492898f, -0.012358f, -0.021205f},
        {0.857729f, -0.514103f, -0.012875f, -0.020895f},
        {0.844854f, -0.534998f, -0.013384f, -0.020573f},
        {0.831470f, -0.555570f, -0.013885f, -0.020238f},
        {0.817585f, -0.575808f, -0.014377f, -0.019891f},
        {0.803208f, -0.595699f, -0.014861f, -0.019532f},
        {0.788346f, -0.615232f, -0.015336f, -0.019162f},
        {0.773010f, -0.634393f, -0.015802f, -0.018780f},
        {0.757209f, -0.653173f, -0.016258f, -0.018386f},
        {0.740951f, -0.671559f, -0.016704f, -0.017982f},
        {0.724247f, -0.689541f, -0.017140f, -0.017566f},
        {0.707107f, -0.707107f, -0.017566f, -0.017140f},
        {0.689541f, -0.724247f, -0.017982f, -0.016704f},
        {0.671559f, -0.740951f, -0.018386f, -0.016258f},
        {0.653173f, -0.757209f, -0.018780f, -0.015802f},
        {0.634393f, -0.773010f, -0.019162f, -0.015336f},
        {0.615232f, -0.788346f, -0.019532f, -0.014861f},
        {0.595699f, -0.803208f, -0.019891f, -0.014377f},
        {0.575808f, -0.817585f, -0.020238f, -0.013885f},
        {0.555570f, -0.831470f, -0.020573f, -0.013384f},
        {0.534998f, -0.844854f, -0.020895f, -0.012875f},
        {0.514103f, -0.857729f, -0.021205f, -0.012358f},
        {0.492898f, -0.870087f, -0.021501f, -0.011834f},
        {0.471397f, -0.881921f, -0.021785f, -0.011303f},
        {0.449611f, -0.893224f, -0.022056f, -0.010765f},
        {0.427555f, -0.903989f, -0.022314f, -0.010220f},
        {0.405241f, -0.914210f, -0.022558f, -0.009670f},
        {0.382683f, -0.923880f, -0.022788f, -0.009113f},
        {0.359895f, -0.932993f, -0.023005f, -0.008551f},
        {0.336890f, -0.941544f, -0.023208f, -0.007984f},
        {0.313682f, -0.949528f, -0.023397f, -0.007412f},
        {0.290285f, -0.956940f, -0.023572f, -0.006836f},
        {0.266713f, -0.963776f, -0.023733f, -0.006255f},
        {0.242980f, -0.970031f, -0.023879f, -0.005671f},
        {0.219101f, -0.975702f, -0.024011f, -0.005083f},
        {0.195090f, -0.980785f, -0.024128f, -0.004492f},
        {0.170962f, -0.985278f, -0.024231f, -0.003899f},
        {0.146730f, -0.989177f, -0.024320f, -0.003303f},
        {0.122411f, -0.992480f, -0.024394f, -0.002705f},
        {0.098017f, -0.995185f, -0.024453f, -0.002106f},
        {0.073565f, -0.997290f, -0.024497f, -0.001505f},
        {0.049068f, -0.998795f, -0.024526f, -0.000903f},
        {0.024541f, -0.999699f, -0.024541f, -0.000301f},
        {0.000000f, -1.000000f, -0.024541f, 0.000301f},
        {-0.024541f, -0.999699f, -0.024526f, 0.000903f},
        {-0.049068f, -0.998795f, -0.024497f, 0.001505f},
        {-0.073565f, -0.997290f, -0.024453f, 0.002106f},
        {-0.098017f, -0.995185f, -0.024394f, 0.002705f},
        {-0.122411f, -0.992480f, -0.024320f, 0.003303f},
        {-0.146730f, -0.989177f, -0.024231f, 0.003899f},
        {-0.170962f, -0.985278f, -0.024128f, 0.004492f},
        {-0.195090f, -0.980785f, -0.024011f, 0.005083f},
        {-0.219101f, -0.975702f, -0.023879f, 0.005671f},
        {-0.242980f, -0.970031f, -0.023733f, 0.006255f},
        {-0.266713f, -0.963776f, -0.023572f, 0.006836f},
        {-0.290285f, -0.956940f, -0.023397f, 0.007412f},
        {-0.313682f, -0.949528f, -0.023208f, 0.007984f},
        {-0.336890f, -0.941544f, -0.023005f, 0.008551f},
        {-0.359895f, -0.932993f, -0.022788f, 0.009113f},
        {-0.382683f, -0.923880f, -0.022558f, 0.009670f},
        {-0.405241f, -0.914210f, -0.022314f, 0.010220f},
        {-0.427555f, -0.903989f, -0.022056f, 0.010765f},
        {-0.449611f, -0.893224f, -0.021785f, 0.011303f},
        {-0.471397f, -0.881921f, -0.021501f, 0.011834f},
        {-0.492898f, -0.870087f, -0.021205f, 0.012358f},
        {-0.514103f, -0.857729f, -0.020895f, 0.012875f},
        {-0.534998f, -0.844854f, -0.020573f, 0.013384f},
        {-0.555570f, -0.831470f, -0.020238f, 0.013885f},
        {-0.575808f, -0.817585f, -0.019891f, 0.014377f},
        {-0.595699f, -0.803208f, -0.019532f, 0.014861f},
        {-0.615232f, -0.788346f, -0.019162f, 0.015336f},
        {-0.634393f, -0.773010f, -0.018780f, 0.015802f},
        {-0.653173f, -0.757209f, -0.018386f, 0.016258f},
        {-0.671559f, -0.740951f, -0.017982f, 0.016704f},
        {-0.689541f, -0.724247f, -0.017566f, 0.017140f},
        {-0.707107f, -0.707107f, -0.017140f, 0.017566f},
        {-0.724247f, -0.689541f, -0.016704f, 0.017982f},
        {-0.740951f, -0.671559f, -0.016258f, 0.018386f},
        {-0.757209f, -0.653173f, -0.015802f, 0.018780f},
        {-0.773010f, -0.634393f, -0.015336f, 0.019162f},
        {-0.788346f, -0.615232f, -0.014861f, 0.019532f},
        {-0.803208f, -0.595699f, -0.014377f, 0.019891f},
        {-0.817585f, -0.575808f, -0.013885f, 0.020238f},
        {-0.831470f, -0.555570f, -0.013384f, 0.020573f},
        {-0.844854f, -0.534998f, -0.012875f, 0.020895f},
        {-0.857729f, -0.514103f, -0.012358f, 0.021205f},
        {-0.870087f, -0.492898f, -0.011834f, 0.021501f},
        {-0.881921f, -0.471397f, -0.011303f, 0.021785f},
        {-0.893224f, -0.449611f, -0.010765f, 0.022056f},
        {-0.903989f, -0.427555f, -0.010220f, 0.022314f},
        {-0.914210f, -0.405241f, -0.009670f, 0.022558f},
        {-0.923880f, -0.382683f, -0.009113f, 0.022788f},
        {-0.932993f, -0.359895f, -0.008551f, 0.023005f},
        {-0.941544f, -0.336890f, -0.007984f, 0.023208f},
        {-0.949528f, -0.313682f, -0.007412f, 0.023397f},
        {-0.956940f, -0.290285f, -0.006836f, 0.023572f},
        {-0.963776f, -0.266713f, -0.006255f, 0.023733f},
        {-0.970031f, -0.242980f, -0.005671f, 0.023879f},
        {-0.975702f, -0.219101f, -0.005083f, 0.024011f},
        {-0.980785f, -0.195090f, -0.004492f, 0.024128f},
        {-0.985278f, -0.170962f, -0.003899f, 0.024231f},
        {-0.989177f, -0.146730f, -0.003303f, 0.024320f},
        {-0.992480f, -0.122411f, -0.002705f, 0.024394f},
        {-0.995185f, -0.098017f, -0.002106f, 0.024453f},
        {-0.997290f, -0.073565f, -0.001505f, 0.024497f},
        {-0.998795f, -0.049068f, -0.000903f, 0.024526f},
        {-0.999699f, -0.024541f, -0.000301f, 0.024541f},
        {-1.000000f, -0.000000f, 0.000301f, 0.024541f},
        {-0.999699f, 0.024541f, 0.000903f, 0.024526f},
        {-0.998795f, 0.049068f, 0.001505f, 0.024497f},
        {-0.997290f, 0.073565f, 0.002106f, 0.024453f},
        {-0.995185f, 0.098017f, 0.002705f, 0.024394f},
        {-0.992480f, 0.122411f, 0.003303f, 0.024320f},
        {-0.989177f, 0.146730f, 0.003899f, 0.024231f},
        {-0.985278f, 0.170962f, 0.004492f, 0.024128f},
        {-0.980785f, 0.195090f, 0.005083f, 0.024011f},
        {-0.975702f, 0.219101f, 0.005671f, 0.023879f},
        {-0.970031f, 0.242980f, 0.006255f, 0.023733f},
        {-0.963776f, 0.266713f, 0.006836f, 0.023572f},
        {-0.956940f, 0.290285f, 0.007412f, 0.023397f},
        {-0.949528f, 0.313682f, 0.007984f, 0.023208f},
        {-0.941544f, 0.336890f, 0.008551f, 0.023005f},
        {-0.932993f, 0.359895f, 0.009113f, 0.022788f},
        {-0.923880f, 0.382683f, 0.009670f, 0.022558f},
        {-0.914210f, 0.405241f, 0.010220f, 0.022314f},
        {-0.903989f, 0.427555f, 0.010765f, 0.022056f},
        {-0.893224f, 0.449611f, 0.011303f, 0.021785f},
        {-0.881921f, 0.471397f, 0.011834f, 0.021501f},
        {-0.870087f, 0.492898f, 0.012358f, 0.021205f},
        {-0.857729f, 0.514103f, 0.012875f, 0.020895f},
        {-0.844854f, 0.534998f, 0.013384f, 0.020573f},
        {-0.831470f, 0.555570f, 0.013885f, 0.020238f},
        {-0.817585f, 0.575808f, 0.014377f, 0.019891f},
        {-0.803208f, 0.595699f, 0.014861f, 0.019532f},
        {-0.788346f, 0.615232f, 0.015336f, 0.019162f},
        {-0.773010f, 0.634393f, 0.015802f, 0.018780f},
        {-0.757209f, 0.653173f, 0.016258f, 0.018386f},
        {-0.740951f, 0.671559f, 0.016704f, 0.017982f},
        {-0.724247f, 0.689541f, 0.017140f, 0.017566f},
        {-0.707107f, 0.707107f, 0.017566f, 0.017140f},
        {-0.689541f, 0.724247f, 0.017982f, 0.016704f},
        {-0.671559f, 0.740951f, 0.018386f, 0.016258f},
        {-0.653173f, 0.757209f, 0.018780f, 0.015802f},
        {-0.634393f, 0.773010f, 0.019162f, 0.015336f},
        {-0.615232f, 0.788346f, 0.019532f, 0.014861f},
        {-0.595699f, 0.803208f, 0.019891f, 0.014377f},
        {-0.575808f, 0.817585f, 0.020238f, 0.013885f},
        {-0.555570f, 0.831470f, 0.020573f, 0.013384f},
        {-0.534998f, 0.844854f, 0.020895f, 0.012875f},
        {-0.514103f, 0.857729f, 0.021205f, 0.012358f},
        {-0.492898f, 0.870087f, 0.021501f, 0.011834f},
        {-0.471397f, 0.881921f, 0.021785f, 0.011303f},
        {-0.449611f, 0.893224f, 0.022056f, 0.010765f},
        {-0.427555f, 0.903989f, 0.022314f, 0.010220f},
        {-0.405241f, 0.914210f, 0.022558f, 0.009670f},
        {-0.382683f, 0.923880f, 0.022788f, 0.009113f},
        {-0.359895f, 0.932993f, 0.023005f, 0.008551f},
        {-0.336890f, 0.941544f, 0.023208f, 0.007984f},
        {-0.313682f, 0.949528f, 0.023397f, 0.007412f},
        {-0.290285f, 0.956940f, 0.023572f, 0.006836f},
        {-0.266713f, 0.963776f, 0.023733f, 0.006255f},
        {-0.242980f, 0.970031f, 0.023879f, 0.005671f},
        {-0.219101f, 0.975702f, 0.024011f, 0.005083f},
        {-0.195090f, 0.980785f, 0.024128f, 0.004492f},
        {-0.170962f, 0.985278f, 0.024231f, 0.003899f},
        {-0.146730f, 0.989177f, 0.024320f, 0.003303f},
        {-0.122411f, 0.992480f, 0.024394f, 0.002705f},
        {-0.098017f, 0.995185f, 0.024453f, 0.002106f},
        {-0.073565f, 0.997290f, 0.024497f, 0.001505f},
        {-0.049068f, 0.998795f, 0.024526f, 0.000903f},
        {-0.024541f, 0.999699f, 0.024541f, 0.000301f},
        {-0.000000f, 1.000000f, 0.024541f, -0.000301f},
};

/// @addr{0x80274148}
static constexpr AtanEntry sArcTanTbl[32 + 1] = {
        {0.000000000f, 1.272825321f},
        {1.272825321f, 1.270345790f},
        {2.543171111f, 1.265415586f},
        {3.808586697f, 1.258091595f},
        {5.066678293f, 1.248457103f},
        {6.315135396f, 1.236619467f},
        {7.551754863f, 1.222707202f},
        {8.774462065f, 1.206866624f},
        {9.981328688f, 1.189258212f},
        {11.170586901f, 1.170052841f},
        {12.340639741f, 1.149428034f},
        {13.490067775f, 1.127564381f},
        {14.617632156f, 1.104642222f},
        {15.722274378f, 1.080838675f},
        {16.803113053f, 1.056325088f},
        {17.859438141f, 1.031264918f},
        {18.890703059f, 1.005812061f},
        {19.896515121f, 0.980109621f},
        {20.876624742f, 0.954289072f},
        {21.830913814f, 0.928469801f},
        {22.759383615f, 0.902758952f},
        {23.662142567f, 0.877251558f},
        {24.539394125f, 0.852030871f},
        {25.391424996f, 0.827168886f},
        {26.218593881f, 0.802726967f},
        {27.021320848f, 0.778756582f},
        {27.800077430f, 0.755300081f},
        {28.555377511f, 0.732391496f},
        {29.287769007f, 0.710057351f},
        {29.997826358f, 0.688317453f},
        {30.686143811f, 0.667185647f},
        {31.353329458f, 0.646670542f},
        {32.000000000f, 0.626776175f},
};

/// @addr{0x80085110}
[[nodiscard]] static inline constexpr f32 SinFIdx(f32 fidx) {
    f32 abs_fidx = fabs(fidx);

    while (abs_fidx >= 65536.0f) {
        abs_fidx -= 65536.0f;
    }

    u16 idx = static_cast<u16>(abs_fidx);
    f32 r = abs_fidx - static_cast<f32>(idx);
    idx &= 0xFF;
    f32 val = sSinCosTbl[idx].sinVal + r * sSinCosTbl[idx].sinDt;
    return fidx < 0.0f ? -val : val;
}

/// @addr{0x80085180}
[[nodiscard]] static inline constexpr f32 CosFIdx(f32 fidx) {
    f32 abs_fidx = fabs(fidx);

    while (abs_fidx >= 65536.0f) {
        abs_fidx -= 65536.0f;
    }

    u16 idx = static_cast<u16>(abs_fidx);
    f32 r = abs_fidx - static_cast<f32>(idx);
    idx &= 0xFF;

    return sSinCosTbl[idx].cosVal + r * sSinCosTbl[idx].cosDt;
}

/// @addr{0x800851E0}
[[nodiscard]] static inline constexpr std::pair<f32, f32> SinCosFIdx(f32 fidx) {
    f32 abs_fidx = fabs(fidx);

    while (abs_fidx >= 65536.0f) {
        abs_fidx -= 65536.0f;
    }

    u16 idx = static_cast<u16>(abs_fidx);
    f32 r = abs_fidx - static_cast<f32>(idx);
    idx &= 0xFF;

    f32 cos = fma(sSinCosTbl[idx].cosDt, r, sSinCosTbl[idx].cosVal);
    f32 sin = fma(sSinCosTbl[idx].sinDt, r, sSinCosTbl[idx].sinVal);

    if (fidx < 0.0f) {
        sin = -sin;
    }

    return {sin, cos};
}

[[nodiscard]] static inline constexpr f32 AtanFIdx_(f32 x) {
    x *= 32.0f;
    u16 idx = static_cast<u16>(x);
    f32 r = x - static_cast<f32>(idx);
    return sArcTanTbl[idx].atanVal + r * sArcTanTbl[idx].atanDt;
}

/// @addr{0x800853C0}
[[nodiscard]] static inline constexpr f32 Atan2FIdx(f32 y, f32 x) {
    if (x == 0.0f && y == 0.0f) {
        return 0.0f;
    }

    if (x >= 0.0f) {
        if (y >= 0.0f) {
            if (x >= y) {
                return 0.0f + AtanFIdx_(y / x);
            } else {
                return 64.0f - AtanFIdx_(x / y);
            }
        } else {
            if (x >= -y) {
                return 0.0f - AtanFIdx_(-y / x);
            } else {
                return -64.0f + AtanFIdx_(x / -y);
            }
        }
    } else {
        if (y >= 0.0f) {
            if (-x >= y) {
                return 128.0f - AtanFIdx_(y / -x);
            } else {
                return 64.0f + AtanFIdx_(-x / y);
            }
        } else {
            if (-x >= -y) {
                return -128.0f + AtanFIdx_(-y / -x);
            } else {
                return -64.0f - AtanFIdx_(-x / -y);
            }
        }
    }
}

/// Takes in radians
/// @addr{0x8022F860}
[[nodiscard]] static inline constexpr f32 sin(f32 x) {
    return SinFIdx(x * RAD2FIDX);
}

/// Takes in radians
/// @addr{0x8022F86C}
[[nodiscard]] static inline constexpr f32 cos(f32 x) {
    return CosFIdx(x * RAD2FIDX);
}

/// @addr{0x8022F89C}
[[nodiscard]] static inline constexpr f32 asin(f32 x) {
    return ::asinl(x);
}

/// @addr{0x8022F8C0}
[[nodiscard]] static inline constexpr f32 acos(f32 x) {
    return ::acosl(x);
}

/// @addr{0x8022F8E4}
[[nodiscard]] static inline constexpr f32 atan2(f32 y, f32 x) {
    return Atan2FIdx(y, x) * FIDX2RAD;
}

[[nodiscard]] static inline constexpr f32 abs(f32 x) {
    return std::abs(x);
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

[[nodiscard]] static inline constexpr f64 frsqrte(const f64 val) {
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
        // then clear that bit to mimic a not-subnormal number!
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

[[nodiscard]] static inline constexpr f32 fres(const f32 val) {
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

/// CREDIT: Hanachan
/// @addr{0x80085040}
[[nodiscard]] static inline constexpr f32 frsqrt(f32 x) {
    // frsqrte instruction
    f64 est = frsqrte(x);

    // Newton-Raphson refinement
    f32 tmp0 = static_cast<f32>(est * force25Bit(est));
    f32 tmp1 = static_cast<f32>(est * static_cast<f64>(0.5f));
    f32 tmp2 =
            static_cast<f32>(static_cast<f64>(3.0f) - static_cast<f64>(tmp0) * static_cast<f64>(x));
    return tmp1 * tmp2;
}

/// @addr{0x8022F80C}
[[nodiscard]] static inline constexpr f32 sqrt(f32 x) {
    return x > 0.0f ? x * frsqrt(x) : 0.0f;
}

/// @addr{0x800867C0}
static inline constexpr u32 FindRootsQuadratic(f32 a, f32 b, f32 c, f32 &root1, f32 &root2) {
    constexpr f32 EPSILON = 0.0002f;

    if (b == 0.0f) {
        f32 x = -c / a;
        if (x > EPSILON) {
            root1 = x * frsqrt(x);
            root2 = -root1;
            return 2;
        }

        // Nintendo might've made a typo here?
        if (x >= -EPSILON) {
            root1 = 0.0f;
            return 1;
        }

        return 0;
    }

    f32 halfBOverA = b / (2.0f * a);
    f32 normalizedC = c / (halfBOverA * (a * halfBOverA));
    f32 normalizedDiscriminant = 1.0f - normalizedC;

    if (normalizedDiscriminant > EPSILON) {
        f32 sqrtNormalizedDiscriminant = normalizedDiscriminant * frsqrt(normalizedDiscriminant);
        root2 = (halfBOverA * normalizedC) / (-1.0f - sqrtNormalizedDiscriminant);
        root1 = halfBOverA * (-1.0f - sqrtNormalizedDiscriminant);
        return 2;
    }

    // Nintendo might've typo'd here?
    if (normalizedDiscriminant >= -EPSILON) {
        root1 = -halfBOverA;
        return 1;
    }

    return 0;
}

/// @brief Fused Newton-Raphson operation.
[[nodiscard]] static inline constexpr f32 finv(f32 x) {
    f32 inv = fres(x);
    f32 invDouble = inv + inv;
    f32 invSquare = inv * inv;
    return -fms(x, invSquare, invDouble);
}

/// @addr{0x80085070}
/// @brief Evaluates a cubic Hermite curve at a given parameter @p t.
/// @details Computes \f[H(t) = h_0(t)\,p_0 + h_1(t)\,p_1 + h_2(t)\,m_0 + h_3(t)\,m_1\f] where the
/// cubic Hermite basis functions are:
/// \f[h_0(\theta) = 2\theta^3 - 3\theta^2 + 1\f]
/// \f[h_1(\theta) = -2\theta^3 + 3\theta^2\f]
/// \f[h_2(\theta) = \theta^3 - 2\theta^2 + \theta\f]
/// \f[h_3(\theta) = \theta^3 - \theta^2\f]
///
/// @param p0 Start value.
/// @param m0 Start tangent.
/// @param p1 End value.
/// @param m1 End tangent.
/// @param t Interpolation parameter in the domain [0, 1]
/// @return Interpolated value at @p t.
[[nodiscard]] static inline constexpr f32 Hermite(f32 p0, f32 m0, f32 p1, f32 m1, f32 t) {
    f32 t2 = t * t;
    f32 t2_less_t = t2 - t;
    f32 h3 = t2_less_t * t;
    f32 f0 = 2.0f * h3;
    f32 h2 = h3 - t2_less_t;
    f32 h1 = t2 - f0;

    return h3 * m1 + (h2 * m0 + (p0 - h1 * p0 + h1 * p1));
}

} // namespace EGG::Mathf
