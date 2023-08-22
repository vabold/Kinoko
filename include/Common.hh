#pragma once

#include <array>
#include <assert.h>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
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

enum class Vehicle {
    Standard_Kart_S = 0,
    Standard_Kart_M = 1,
    Standard_Kart_L = 2,
    Baby_Booster = 3,
    Classic_Dragster = 4,
    Offroader = 5,
    Mini_Beast = 6,
    Wild_Wing = 7,
    Flame_Flyer = 8,
    Cheep_Charger = 9,
    Super_Blooper = 10,
    Piranha_Prowler = 11,
    Tiny_Titan = 12,
    Daytripper = 13,
    Jetsetter = 14,
    Blue_Falcon = 15,
    Sprinter = 16,
    Honeycoupe = 17,
    Standard_Bike_S = 18,
    Standard_Bike_M = 19,
    Standard_Bike_L = 20,
    Bullet_Bike = 21,
    Mach_Bike = 22,
    Flame_Runner = 23,
    Bit_Bike = 24,
    Sugarscoot = 25,
    Wario_Bike = 26,
    Quacker = 27,
    Zip_Zip = 28,
    Shooting_Star = 29,
    Magikruiser = 30,
    Sneakster = 31,
    Spear = 32,
    Jet_Bubble = 33,
    Dolphin_Dasher = 34,
    Phantom = 35,
    Max = 36,
};

enum class Character {
    Mario = 0,
    Baby_Peach = 1,
    Waluigi = 2,
    Bowser = 3,
    Baby_Daisy = 4,
    Dry_Bones = 5,
    Baby_Mario = 6,
    Luigi = 7,
    Toad = 8,
    Donkey_Kong = 9,
    Yoshi = 10,
    Wario = 11,
    Baby_Luigi = 12,
    Toadette = 13,
    Koopa_Troopa = 14,
    Daisy = 15,
    Peach = 16,
    Birdo = 17,
    Diddy_Kong = 18,
    King_Boo = 19,
    Bowser_Jr = 20,
    Dry_Bowser = 21,
    Funky_Kong = 22,
    Rosalina = 23,
    Small_Mii_Outfit_A_Male = 24,
    Small_Mii_Outfit_A_Female = 25,
    Small_Mii_Outfit_B_Male = 26,
    Small_Mii_Outfit_B_Female = 27,
    Small_Mii_Outfit_C_Male = 28,
    Small_Mii_Outfit_C_Female = 29,
    Medium_Mii_Outfit_A_Male = 30,
    Medium_Mii_Outfit_A_Female = 31,
    Medium_Mii_Outfit_B_Male = 32,
    Medium_Mii_Outfit_B_Female = 33,
    Medium_Mii_Outfit_C_Male = 34,
    Medium_Mii_Outfit_C_Female = 35,
    Large_Mii_Outfit_A_Male = 36,
    Large_Mii_Outfit_A_Female = 37,
    Large_Mii_Outfit_B_Male = 38,
    Large_Mii_Outfit_B_Female = 39,
    Large_Mii_Outfit_C_Male = 40,
    Large_Mii_Outfit_C_Female = 41,
    Medium_Mii = 42,
    Small_Mii = 43,
    Large_Mii = 44,
    Peach_Biker_Outfit = 45,
    Daisy_Biker_Outfit = 46,
    Rosalina_Biker_Outfit = 47,
    Max = 48,
};

extern const char *const COURSE_NAMES[59];
extern const char *const VEHICLE_NAMES[36];

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

// Does not support f64
static inline f32 parsef(f32 val, std::endian endian) {
    return std::bit_cast<f32>(parse<u32>(std::bit_cast<u32>(val), endian));
}
