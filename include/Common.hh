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

enum class Course {
    Mario_Circuit = 0,
    Moo_Moo_Meadows = 1,
    Mushroom_Gorge = 2,
    Grumble_Volcano = 3,
    Toads_Factory = 4,
    Coconut_Mall = 5,
    DK_Summit = 6,
    Wario_Gold_Mine = 7,
    Luigi_Circuit = 8,
    Daisy_Circuit = 9,
    Moonview_Highway = 10,
    Maple_Treeway = 11,
    Bowsers_Castle = 12,
    Rainbow_Road = 13,
    Dry_Dry_Ruins = 14,
    Koopa_Cape = 15,
    GCN_Peach_Beach = 16,
    GCN_Mario_Circuit = 17,
    GCN_Waluigi_Stadium = 18,
    GCN_DK_Mountain = 19,
    DS_Yoshi_Falls = 20,
    DS_Desert_Hills = 21,
    DS_Peach_Gardens = 22,
    DS_Delfino_Square = 23,
    SNES_Mario_Circuit_3 = 24,
    SNES_Ghost_Valley_2 = 25,
    N64_Mario_Raceway = 26,
    N64_Sherbet_Land = 27,
    N64_Bowsers_Castle = 28,
    N64_DKs_Jungle_Parkway = 29,
    GBA_Bowser_Castle_3 = 30,
    GBA_Shy_Guy_Beach = 31,
    Delfino_Pier = 32,
    Block_Plaza = 33,
    Chain_Chomp_Roulette = 34,
    Funky_Stadium = 35,
    Thwomp_Desert = 36,
    GCN_Cookie_Land = 37,
    DS_Twilight_House = 38,
    SNES_Battle_Course_4 = 39,
    GBA_Battle_Course_3 = 40,
    N64_Skyscraper = 41,
    Galaxy_Colosseum = 54,
    Win_Demo = 55,
    Lose_Demo = 56,
    Draw_Demo = 57,
    Ending_Demo = 58,
};

extern const char *const COURSE_NAMES[59];

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

#define K_PANIC(m, ...) \
    do { \
        K_LOG(m, ##__VA_ARGS__); \
        K_LOG("Exiting with code 1..."); \
        exit(1); \
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
