#pragma once

#include <array>
#include <assert.h>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <float.h>
#include <limits>

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

// Allow 40MiB in allocatable memory
#define MAX_ALLOC_SIZE 0x2800000

// CREDIT: MKW-SP
// Hack required to print preprocessor macro
#define K_TOSTRING(x) #x
#define K_TOSTRING2(x) K_TOSTRING(x)

// CREDIT: MKW-SP
// Better console logging
#define K_LOG(m, ...) \
    do { \
        printf("[" __FILE_NAME__ ":" K_TOSTRING2(__LINE__) "] " m "\n", ##__VA_ARGS__); \
    } while (0)

static_assert(FLT_EPSILON == 1.0f / 8388608.0f);

// Form data into integral value
template <typename T>
static inline T form(const u8 *data) {
  static_assert(std::is_integral<T>::value);
  T result = 0;
  for (size_t i = 0; i < sizeof(T); ++i) {
    result = (result << 8) | data[i];
  }
  return result;
}

// Consistent file parsing with byte-swappable values
template <typename T>
static inline T parse(T val, std::endian endian) {
    static_assert(
            std::endian::native == std::endian::big || std::endian::native == std::endian::little);
    return endian == std::endian::native ? val : std::byteswap(val);
}
