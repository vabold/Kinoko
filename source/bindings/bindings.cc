#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include "bindings/KBindSystem.hh"
#include "game/system/KPadController.hh"
#include "egg/core/ExpHeap.hh"
#include "egg/core/SceneManager.hh"
#include "Common.hh"

namespace nb = nanobind;
using namespace nb::literals;

static void InitMemory();
static void FlushDenormalsToZero();

struct KinokoInitializer {
    KinokoInitializer() {
        FlushDenormalsToZero();
        InitMemory();
    }
};
static KinokoInitializer s_KinokoInitializer_instance;

// clang-format off
NB_MODULE(bindings, m) {
    m.doc() = "Python bindings for Kinoko";

    nb::enum_<Character>(m, "Character")
        .value("Mario", Character::Mario)
        .value("Baby_Peach", Character::Baby_Peach)
        .value("Waluigi", Character::Waluigi)
        .value("Bowser", Character::Bowser)
        .value("Baby_Daisy", Character::Baby_Daisy)
        .value("Dry_Bones", Character::Dry_Bones)
        .value("Baby_Mario", Character::Baby_Mario)
        .value("Luigi", Character::Luigi)
        .value("Peach", Character::Peach)
        .value("Yoshi", Character::Yoshi)
        .value("Donkey_Kong", Character::Donkey_Kong)
        .value("Wario", Character::Wario)
        .value("Baby_Luigi", Character::Baby_Luigi)
        .value("Toad", Character::Toad)
        .value("Koopa_Troopa", Character::Koopa_Troopa)
        .value("Daisy", Character::Daisy)
        .value("Toadette", Character::Toadette)
        .value("Birdo", Character::Birdo)
        .value("Diddy_Kong", Character::Diddy_Kong)
        .value("King_Boo", Character::King_Boo)
        .value("Bowser_Jr", Character::Bowser_Jr)
        .value("Dry_Bowser", Character::Dry_Bowser)
        .value("Funky_Kong", Character::Funky_Kong)
        .value("Rosalina", Character::Rosalina)
        .value("Small_Mii_Outfit_A_Male", Character::Small_Mii_Outfit_A_Male)
        .value("Small_Mii_Outfit_A_Female", Character::Small_Mii_Outfit_A_Female)
        .value("Small_Mii_Outfit_B_Male", Character::Small_Mii_Outfit_B_Male)
        .value("Small_Mii_Outfit_B_Female", Character::Small_Mii_Outfit_B_Female)
        .value("Small_Mii_Outfit_C_Male", Character::Small_Mii_Outfit_C_Male)
        .value("Small_Mii_Outfit_C_Female", Character::Small_Mii_Outfit_C_Female)
        .value("Medium_Mii_Outfit_A_Male", Character::Medium_Mii_Outfit_A_Male)
        .value("Medium_Mii_Outfit_A_Female", Character::Medium_Mii_Outfit_A_Female)
        .value("Medium_Mii_Outfit_B_Male", Character::Medium_Mii_Outfit_B_Male)
        .value("Medium_Mii_Outfit_B_Female", Character::Medium_Mii_Outfit_B_Female)
        .value("Medium_Mii_Outfit_C_Male", Character::Medium_Mii_Outfit_C_Male)
        .value("Medium_Mii_Outfit_C_Female", Character::Medium_Mii_Outfit_C_Female)
        .value("Large_Mii_Outfit_A_Male", Character::Large_Mii_Outfit_A_Male)
        .value("Large_Mii_Outfit_A_Female", Character::Large_Mii_Outfit_A_Female)
        .value("Large_Mii_Outfit_B_Male", Character::Large_Mii_Outfit_B_Male)
        .value("Large_Mii_Outfit_B_Female", Character::Large_Mii_Outfit_B_Female)
        .value("Large_Mii_Outfit_C_Male", Character::Large_Mii_Outfit_C_Male)
        .value("Large_Mii_Outfit_C_Female", Character::Large_Mii_Outfit_C_Female)
        .value("Medium_Mii", Character::Medium_Mii)
        .value("Small_Mii", Character::Small_Mii)
        .value("Large_Mii", Character::Large_Mii)
        .value("Max", Character::Max)
        .export_values();

    nb::enum_<Vehicle>(m, "Vehicle")
        .value("Standard_Kart_S", Vehicle::Standard_Kart_S)
        .value("Standard_Kart_M", Vehicle::Standard_Kart_M)
        .value("Standard_Kart_L", Vehicle::Standard_Kart_L)
        .value("Baby_Booster", Vehicle::Baby_Booster)
        .value("Classic_Dragster", Vehicle::Classic_Dragster)
        .value("Offroader", Vehicle::Offroader)
        .value("Mini_Beast", Vehicle::Mini_Beast)
        .value("Wild_Wing", Vehicle::Wild_Wing)
        .value("Flame_Flyer", Vehicle::Flame_Flyer)
        .value("Cheep_Charger", Vehicle::Cheep_Charger)
        .value("Super_Blooper", Vehicle::Super_Blooper)
        .value("Piranha_Prowler", Vehicle::Piranha_Prowler)
        .value("Tiny_Titan", Vehicle::Tiny_Titan)
        .value("Daytripper", Vehicle::Daytripper)
        .value("Jetsetter", Vehicle::Jetsetter)
        .value("Blue_Falcon", Vehicle::Blue_Falcon)
        .value("Sprinter", Vehicle::Sprinter)
        .value("Honeycoupe", Vehicle::Honeycoupe)
        .value("Standard_Bike_S", Vehicle::Standard_Bike_S)
        .value("Standard_Bike_M", Vehicle::Standard_Bike_M)
        .value("Standard_Bike_L", Vehicle::Standard_Bike_L)
        .value("Bullet_Bike", Vehicle::Bullet_Bike)
        .value("Mach_Bike", Vehicle::Mach_Bike)
        .value("Flame_Runner", Vehicle::Flame_Runner)
        .value("Bit_Bike", Vehicle::Bit_Bike)
        .value("Sugarscoot", Vehicle::Sugarscoot)
        .value("Wario_Bike", Vehicle::Wario_Bike)
        .value("Quacker", Vehicle::Quacker)
        .value("Zip_Zip", Vehicle::Zip_Zip)
        .value("Shooting_Star", Vehicle::Shooting_Star)
        .value("Magikruiser", Vehicle::Magikruiser)
        .value("Sneakster", Vehicle::Sneakster)
        .value("Spear", Vehicle::Spear)
        .value("Jet_Bubble", Vehicle::Jet_Bubble)
        .value("Dolphin_Dasher", Vehicle::Dolphin_Dasher)
        .value("Phantom", Vehicle::Phantom)
        .value("Max", Vehicle::Max)
        .export_values();

    nb::enum_<Course>(m, "Course")
        .value("Luigi_Circuit", Course::Luigi_Circuit)
        .value("Moo_Moo_Meadows", Course::Moo_Moo_Meadows)
        .value("Mushroom_Gorge", Course::Mushroom_Gorge)
        .value("Toads_Factory", Course::Toads_Factory)
        .value("Mario_Circuit", Course::Mario_Circuit)
        .value("Coconut_Mall", Course::Coconut_Mall)
        .value("DK_Summit", Course::DK_Summit)
        .value("Wario_Gold_Mine", Course::Wario_Gold_Mine)
        .value("Daisy_Circuit", Course::Daisy_Circuit)
        .value("Koopa_Cape", Course::Koopa_Cape)
        .value("Maple_Treeway", Course::Maple_Treeway)
        .value("Grumble_Volcano", Course::Grumble_Volcano)
        .value("Dry_Dry_Ruins", Course::Dry_Dry_Ruins)
        .value("Moonview_Highway", Course::Moonview_Highway)
        .value("Bowsers_Castle", Course::Bowsers_Castle)
        .value("Rainbow_Road", Course::Rainbow_Road)
        .value("GCN_Peach_Beach", Course::GCN_Peach_Beach)
        .value("DS_Yoshi_Falls", Course::DS_Yoshi_Falls)
        .value("SNES_Ghost_Valley_2", Course::SNES_Ghost_Valley_2)
        .value("N64_Mario_Raceway", Course::N64_Mario_Raceway)
        .value("N64_Sherbet_Land", Course::N64_Sherbet_Land)
        .value("GBA_Shy_Guy_Beach", Course::GBA_Shy_Guy_Beach)
        .value("DS_Delfino_Square", Course::DS_Delfino_Square)
        .value("GCN_Waluigi_Stadium", Course::GCN_Waluigi_Stadium)
        .value("DS_Desert_Hills", Course::DS_Desert_Hills)
        .value("GBA_Bowser_Castle_3", Course::GBA_Bowser_Castle_3)
        .value("N64_DKs_Jungle_Parkway", Course::N64_DKs_Jungle_Parkway)
        .value("GCN_Mario_Circuit", Course::GCN_Mario_Circuit)
        .value("SNES_Mario_Circuit_3", Course::SNES_Mario_Circuit_3)
        .value("DS_Peach_Gardens", Course::DS_Peach_Gardens)
        .value("GCN_DK_Mountain", Course::GCN_DK_Mountain)
        .value("N64_Bowsers_Castle", Course::N64_Bowsers_Castle)
        .export_values();

    nb::enum_<System::Trick>(m, "Trick")
        .value("Neutral", System::Trick::None) // None is reserved
        .value("Up", System::Trick::Up)
        .value("Down", System::Trick::Down)
        .value("Left", System::Trick::Left)
        .value("Right", System::Trick::Right)
        .export_values();

    nb::class_<KBindSystem>(m, "KBindSystem")
        .def(nb::init<>())
        .def("init", &KBindSystem::init)
        .def("step", &KBindSystem::step)
        .def("set_course", &KBindSystem::set_course, "course"_a)
        .def("set_player", &KBindSystem::set_player, "player_idx"_a, "character"_a, "vehicle"_a, "drift_is_auto"_a)
        .def("get_host_controller", &KBindSystem::get_host_controller, nb::rv_policy::reference)
        .def_static("create_instance", &KBindSystem::CreateInstance, nb::rv_policy::reference)
        .def_static("destroy_instance", &KBindSystem::DestroyInstance)
        .def_static("instance", &KBindSystem::Instance, nb::rv_policy::reference);

    nb::class_<System::KPadHostController>(m, "KPadHostController")
        .def("set_inputs", nb::overload_cast<u16, f32, f32, System::Trick>(&System::KPadHostController::setInputs),
             "buttons"_a, "stick_x"_a, "stick_y"_a, "trick"_a)
        .def("set_inputs_raw_stick", &System::KPadHostController::setInputsRawStick,
             "buttons"_a, "stick_x_raw"_a, "stick_y_raw"_a, "trick"_a)
        .def("set_inputs_raw_stick_zero_center", &System::KPadHostController::setInputsRawStickZeroCenter,
             "buttons"_a, "stick_x_raw"_a, "stick_y_raw"_a, "trick"_a);
}
// clang-format on

#if defined(__arm64__) || defined(__aarch64__)
static void FlushDenormalsToZero() {
    uint64_t fpcr;
    asm("mrs %0,   fpcr" : "=r"(fpcr));
    asm("msr fpcr, %0" ::"r"(fpcr | (1 << 24)));
}
#elif defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#include <immintrin.h>

static void FlushDenormalsToZero() {
    _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
}
#endif

static void *s_memorySpace = nullptr;
static EGG::Heap *s_rootHeap = nullptr;

static void InitMemory() {
    constexpr size_t MEMORY_SPACE_SIZE = 0x8000000; // Arbitrary number
    Abstract::Memory::MEMiHeapHead::OptFlag opt;
    opt.setBit(Abstract::Memory::MEMiHeapHead::eOptFlag::ZeroFillAlloc);

#ifdef BUILD_DEBUG
    opt.setBit(Abstract::Memory::MEMiHeapHead::eOptFlag::DebugFillAlloc);
#endif

    s_memorySpace = malloc(MEMORY_SPACE_SIZE);
    s_rootHeap = EGG::ExpHeap::create(s_memorySpace, MEMORY_SPACE_SIZE, opt);
    s_rootHeap->setName("EGGRoot");
    s_rootHeap->becomeCurrentHeap();

    EGG::SceneManager::SetRootHeap(s_rootHeap);
}