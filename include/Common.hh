/// @file Common.hh
/// @brief This header houses common data types such as our integral types and enums.

#pragma once

#include <Logger.hh>

#include <egg/core/Heap.hh>

#include <array>
#include <cassert>
#include <limits>
#include <type_traits>
#include <utility>

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

enum class WeightClass {
    Invalid = -1,
    Light = 0,
    Medium = 1,
    Heavy = 2,
};

enum class GroupID : u16 {
    None = 0,
    Race = 1,
    Gfx = 2,
    Kart = 3,
    Object = 4,
    Course = 5,
    UI = 6,
    Effect = 7,
    Sound = 8,
    Resource = 10,
    HomeMenu = 11,
    Item = 12,
    Net = 13,
};

static constexpr WeightClass CharacterToWeight(Character character) {
    switch (character) {
    case Character::Baby_Peach:
    case Character::Baby_Daisy... Character::Baby_Mario:
    case Character::Toad:
    case Character::Baby_Luigi... Character::Koopa_Troopa:
    case Character::Small_Mii_Outfit_A_Male... Character::Small_Mii_Outfit_C_Female:
    case Character::Small_Mii:
        return WeightClass::Light;
    case Character::Mario:
    case Character::Luigi:
    case Character::Yoshi:
    case Character::Daisy... Character::Diddy_Kong:
    case Character::Bowser_Jr:
    case Character::Medium_Mii_Outfit_A_Male... Character::Medium_Mii_Outfit_C_Female:
    case Character::Medium_Mii:
    case Character::Peach_Biker_Outfit:
    case Character::Daisy_Biker_Outfit:
        return WeightClass::Medium;
    case Character::Waluigi:
    case Character::Bowser:
    case Character::Donkey_Kong:
    case Character::Wario:
    case Character::King_Boo:
    case Character::Dry_Bowser... Character::Rosalina:
    case Character::Large_Mii_Outfit_A_Male... Character::Large_Mii_Outfit_C_Female:
    case Character::Large_Mii:
    case Character::Rosalina_Biker_Outfit:
        return WeightClass::Heavy;
    default:
        return WeightClass::Invalid;
    }
}

static constexpr WeightClass VehicleToWeight(Vehicle vehicle) {
    switch (vehicle) {
    case Vehicle::Standard_Kart_S:
    case Vehicle::Baby_Booster:
    case Vehicle::Mini_Beast:
    case Vehicle::Cheep_Charger:
    case Vehicle::Tiny_Titan:
    case Vehicle::Blue_Falcon:
    case Vehicle::Standard_Bike_S:
    case Vehicle::Bullet_Bike:
    case Vehicle::Bit_Bike:
    case Vehicle::Quacker:
    case Vehicle::Magikruiser:
    case Vehicle::Jet_Bubble:
        return WeightClass::Light;
    case Vehicle::Standard_Kart_M:
    case Vehicle::Classic_Dragster:
    case Vehicle::Wild_Wing:
    case Vehicle::Super_Blooper:
    case Vehicle::Daytripper:
    case Vehicle::Sprinter:
    case Vehicle::Standard_Bike_M:
    case Vehicle::Mach_Bike:
    case Vehicle::Sugarscoot:
    case Vehicle::Zip_Zip:
    case Vehicle::Sneakster:
    case Vehicle::Dolphin_Dasher:
        return WeightClass::Medium;
    case Vehicle::Standard_Kart_L:
    case Vehicle::Offroader:
    case Vehicle::Flame_Flyer:
    case Vehicle::Piranha_Prowler:
    case Vehicle::Jetsetter:
    case Vehicle::Honeycoupe:
    case Vehicle::Standard_Bike_L:
    case Vehicle::Flame_Runner:
    case Vehicle::Wario_Bike:
    case Vehicle::Shooting_Star:
    case Vehicle::Spear:
    case Vehicle::Phantom:
        return WeightClass::Heavy;
    default:
        return WeightClass::Invalid;
    }
}

static constexpr const char *COURSE_NAMES[59] = {
        "castle_course",
        "farm_course",
        "kinoko_course",
        "volcano_course",
        "factory_course",
        "shopping_course",
        "boardcross_course",
        "truck_course",
        "beginner_course",
        "senior_course",
        "ridgehighway_course",
        "treehouse_course",
        "koopa_course",
        "rainbow_course",
        "desert_course",
        "water_course",
        "old_peach_gc",
        "old_mario_gc",
        "old_waluigi_gc",
        "old_donkey_gc",
        "old_falls_ds",
        "old_desert_ds",
        "old_garden_ds",
        "old_town_ds",
        "old_mario_sfc",
        "old_obake_sfc",
        "old_mario_64",
        "old_sherbet_64",
        "old_koopa_64",
        "old_donkey_64",
        "old_koopa_gba",
        "old_heyho_gba",
        "venice_battle",
        "block_battle",
        "casino_battle",
        "skate_battle",
        "sand_battle",
        "old_CookieLand_gc",
        "old_House_ds",
        "old_battle4_sfc",
        "old_battle3_gba",
        "old_matenro_64",
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        "ring_mission",
        "winningrun_demo",
        "loser_demo",
        "draw_dmeo",
        "ending_demo",
};

static constexpr const char *VEHICLE_NAMES[36] = {
        "sdf_kart",
        "mdf_kart",
        "ldf_kart",
        "sa_kart",
        "ma_kart",
        "la_kart",
        "sb_kart",
        "mb_kart",
        "lb_kart",
        "sc_kart",
        "mc_kart",
        "lc_kart",
        "sd_kart",
        "md_kart",
        "ld_kart",
        "se_kart",
        "me_kart",
        "le_kart",
        "sdf_bike",
        "mdf_bike",
        "ldf_bike",
        "sa_bike",
        "ma_bike",
        "la_bike",
        "sb_bike",
        "mb_bike",
        "lb_bike",
        "sc_bike",
        "mc_bike",
        "lc_bike",
        "sd_bike",
        "md_bike",
        "ld_bike",
        "se_bike",
        "me_bike",
        "le_bike",
};

STATIC_ASSERT(std::numeric_limits<f32>::epsilon() == 1.0f / 8388608.0f);
STATIC_ASSERT(
        std::endian::native == std::endian::big || std::endian::native == std::endian::little);

/// @brief Helper template which uses function overloading and implict up-casting to determine
/// whether or not a class is derived from a templated base class (i.e. MapdataPointInfoAccessor
/// derives from MapdataAccessorBase). See: https://en.cppreference.com/w/cpp/language/sfinae
template <template <typename...> class Base, typename Derived>
struct is_derived_from_template {
private:
    template <typename... Ts>
    static std::true_type test(const Base<Ts...> *);

    static std::false_type test(...);

public:
    static constexpr bool value = decltype(test(std::declval<Derived *>()))::value;
};

template <template <typename...> class Base, typename Derived>
inline constexpr bool is_derived_from_template_v = is_derived_from_template<Base, Derived>::value;

template <typename T>
concept IntegralType = std::is_integral_v<T>;

template <typename T>
concept ParseableType = std::is_integral_v<T> ||
        (std::is_floating_point_v<T> && (sizeof(T) == 4 || sizeof(T) == 8));

// Form data into integral value
template <IntegralType T>
static inline T form(const u8 *data) {
    T result = 0;
    for (size_t i = 0; i < sizeof(T); ++i) {
        result = (result << 8) | data[i];
    }
    return result;
}

// Consistent file parsing with byte-swappable values
template <ParseableType T>
static inline constexpr T parse(T val, std::endian endian = std::endian::big) {
    if constexpr (std::is_integral_v<T>) {
        return endian == std::endian::native ? val : std::byteswap(val);
    } else {
        if constexpr (sizeof(T) == 4) {
            return std::bit_cast<T>(parse<u32>(std::bit_cast<u32>(val), endian));
        } else {
            return std::bit_cast<T>(parse<u64>(std::bit_cast<u64>(val), endian));
        }
    }
}

// Helper function to allow hex representation of f32
static inline constexpr u32 f2u(f32 val) {
    return std::bit_cast<u32>(val);
}
