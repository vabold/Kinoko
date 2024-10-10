#pragma once

#include <Logger.hh>

#include <cstdint>

// We can't include Common.hh, because we have a cyclic dependency
// Instead, we redefine the types for use in memory code
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

static inline uintptr_t GetAddrNum(const void *p) {
    return reinterpret_cast<uintptr_t>(p);
}

static inline void *GetAddrPtr(uintptr_t n) {
    return reinterpret_cast<void *>(n);
}

static inline void *AddOffset(const void *p, size_t offset) {
    return GetAddrPtr(GetAddrNum(p) + offset);
}

static inline void *SubOffset(const void *p, size_t offset) {
    return GetAddrPtr(GetAddrNum(p) - offset);
}

static inline uintptr_t RoundUp(uintptr_t value, uintptr_t alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
}

static inline void *RoundUp(void *ptr, uintptr_t alignment) {
    return GetAddrPtr(RoundUp(GetAddrNum(ptr), alignment));
}

static inline uintptr_t RoundDown(uintptr_t value, uintptr_t alignment) {
    return value & ~(alignment - 1);
}

static inline void *RoundDown(void *ptr, uintptr_t alignment) {
    return GetAddrPtr(RoundDown(GetAddrNum(ptr), alignment));
}
