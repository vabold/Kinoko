#include "KartParamFileManager.hh"

namespace Kart {

/// @addr{0x80591C9C}
void KartParamFileManager::clear() {
    m_kartParam.clear();
    m_driverParam.clear();
    m_bikeDispParam.clear();
}

/// @addr{0x805919F4}
/// @brief Loads and validates the kart parameter files.
void KartParamFileManager::init() {
    m_kartParam.load("kartParam.bin");
    m_driverParam.load("driverParam.bin");
    m_bikeDispParam.load("bikePartsDispParam.bin");
    if (!validate()) {
        PANIC("Parameter files could not be validated!");
    }
}

EGG::RamStream KartParamFileManager::getDriverStream(Character character) const {
    s32 idx = -1;
    switch (character) {
    case Character::Small_Mii_Outfit_A_Male:
    case Character::Small_Mii_Outfit_A_Female:
    case Character::Small_Mii_Outfit_B_Male:
    case Character::Small_Mii_Outfit_B_Female:
    case Character::Small_Mii_Outfit_C_Male:
    case Character::Small_Mii_Outfit_C_Female:
    case Character::Small_Mii:
        idx = 23;
        break;
    case Character::Medium_Mii_Outfit_A_Male:
    case Character::Medium_Mii_Outfit_A_Female:
    case Character::Medium_Mii_Outfit_B_Male:
    case Character::Medium_Mii_Outfit_B_Female:
    case Character::Medium_Mii_Outfit_C_Male:
    case Character::Medium_Mii_Outfit_C_Female:
    case Character::Medium_Mii:
        idx = 24;
        break;
    case Character::Large_Mii_Outfit_A_Male:
    case Character::Large_Mii_Outfit_A_Female:
    case Character::Large_Mii_Outfit_B_Male:
    case Character::Large_Mii_Outfit_B_Female:
    case Character::Large_Mii_Outfit_C_Male:
    case Character::Large_Mii_Outfit_C_Female:
    case Character::Large_Mii:
        idx = 25;
        break;
    default:
        if (character > Character::Rosalina) {
            PANIC("Uh oh.");
        }

        idx = static_cast<s32>(character);
        break;
    }

    auto *file = reinterpret_cast<ParamFile<KartParam::Stats> *>(m_driverParam.file);
    ASSERT(file);
    return EGG::RamStream(&file->params[idx], sizeof(KartParam::Stats));
}

EGG::RamStream KartParamFileManager::getVehicleStream(Vehicle vehicle) const {
    if (vehicle >= Vehicle::Max) {
        PANIC("Uh oh.");
    }

    s32 idx = static_cast<s32>(vehicle);
    auto *file = reinterpret_cast<ParamFile<KartParam::Stats> *>(m_kartParam.file);
    ASSERT(file);
    return EGG::RamStream(&file->params[idx], sizeof(KartParam::Stats));
}

EGG::RamStream KartParamFileManager::getHitboxStream(Vehicle vehicle) const {
    if (vehicle >= Vehicle::Max) {
        PANIC("Uh oh.");
    }

    auto *resourceManager = System::ResourceManager::Instance();
    size_t size;

    auto *file = resourceManager->getBsp(vehicle, &size);
    ASSERT(file);
    ASSERT(size == sizeof(BSP));
    return EGG::RamStream(file, size);
}

EGG::RamStream KartParamFileManager::getBikeDispParamsStream(Vehicle vehicle) const {
    if (vehicle < Vehicle::Standard_Bike_S || vehicle >= Vehicle::Max) {
        PANIC("Uh oh.");
    }

    // We need to index at the correct offset
    constexpr u32 KART_MAX = 18;
    s32 idx = static_cast<s32>(vehicle) - KART_MAX;

    auto *file = reinterpret_cast<ParamFile<KartParam::BikeDisp> *>(m_bikeDispParam.file);
    ASSERT(file);
    return EGG::RamStream(&file->params[idx], sizeof(KartParam::BikeDisp));
}

KartParamFileManager *KartParamFileManager::CreateInstance() {
    ASSERT(!s_instance);
    s_instance = new KartParamFileManager;
    return s_instance;
}

void KartParamFileManager::DestroyInstance() {
    ASSERT(s_instance);
    auto *instance = s_instance;
    s_instance = nullptr;
    delete instance;
}

KartParamFileManager::KartParamFileManager() {
    init();
}

KartParamFileManager::~KartParamFileManager() {
    if (s_instance) {
        s_instance = nullptr;
        WARN("KartParamFileManager instance not explicitly handled!");
    }
}

/// @brief Performs a few checks to make sure the files were loaded successfully.
bool KartParamFileManager::validate() const {
    // Validate kartParam.bin
    if (!m_kartParam.file || m_kartParam.size == 0) {
        return false;
    }

    auto *kartFile = reinterpret_cast<ParamFile<KartParam::Stats> *>(m_kartParam.file);
    if (m_kartParam.size != parse<u32>(kartFile->count) * sizeof(KartParam::Stats) + 4) {
        return false;
    }

    // Validate driverParam.bin
    if (!m_driverParam.file || m_driverParam.size == 0) {
        return false;
    }

    auto *driverFile = reinterpret_cast<ParamFile<KartParam::Stats> *>(m_driverParam.file);
    if (m_driverParam.size != parse<u32>(driverFile->count) * sizeof(KartParam::Stats) + 4) {
        return false;
    }

    // Validate bikePartsDispParam.bin
    if (!m_bikeDispParam.file || m_bikeDispParam.size == 0) {
        return false;
    }

    auto *bikeDispFile = reinterpret_cast<ParamFile<KartParam::BikeDisp> *>(m_bikeDispParam.file);
    if (m_bikeDispParam.size != parse<u32>(bikeDispFile->count) * sizeof(KartParam::BikeDisp) + 4) {
        return false;
    }

    return true;
}

KartParamFileManager *KartParamFileManager::s_instance = nullptr;

} // namespace Kart
