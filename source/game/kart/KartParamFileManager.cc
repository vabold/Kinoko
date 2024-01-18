#include "KartParamFileManager.hh"

#include "game/system/ResourceManager.hh"

namespace Kart {

void KartParamFileManager::clear() {
    m_kartParam.clear();
    m_driverParam.clear();
}

void KartParamFileManager::init() {
    m_kartParam.load("kartParam.bin");
    m_driverParam.load("driverParam.bin");
    if (!validate()) {
        K_PANIC("Parameter files could not be validated!");
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
            K_PANIC("Uh oh.");
        }

        idx = static_cast<s32>(character);
        break;
    }

    auto *file = reinterpret_cast<ParamFile *>(m_driverParam.m_file);
    assert(file);
    void *offset = &file->m_params[idx];
    u32 size = sizeof(KartParam::Stats);
    return EGG::RamStream(reinterpret_cast<u8 *>(offset), size);
}

EGG::RamStream KartParamFileManager::getVehicleStream(Vehicle vehicle) const {
    if (vehicle >= Vehicle::Max) {
        K_PANIC("Uh oh.");
    }

    s32 idx = static_cast<s32>(vehicle);
    auto *file = reinterpret_cast<ParamFile *>(m_kartParam.m_file);
    assert(file);
    void *offset = &file->m_params[idx];
    u32 size = sizeof(KartParam::Stats);
    return EGG::RamStream(reinterpret_cast<u8 *>(offset), size);
}

EGG::RamStream KartParamFileManager::getHitboxStream(Vehicle vehicle) const {
    if (vehicle >= Vehicle::Max) {
        K_PANIC("Uh oh.");
    }

    auto *resourceManager = System::ResourceManager::Instance();
    size_t size;

    auto *file = resourceManager->getBsp(vehicle, &size);
    assert(file);
    assert(size == sizeof(KartParam::BSP));
    return EGG::RamStream(reinterpret_cast<u8 *>(file), size);
}

KartParamFileManager *KartParamFileManager::CreateInstance() {
    assert(!s_instance);
    s_instance = new KartParamFileManager;
    return s_instance;
}

void KartParamFileManager::DestroyInstance() {
    assert(s_instance);
    delete s_instance;
    s_instance = nullptr;
}

KartParamFileManager *KartParamFileManager::Instance() {
    return s_instance;
}

KartParamFileManager::KartParamFileManager() {
    init();
}

KartParamFileManager::~KartParamFileManager() = default;

bool KartParamFileManager::validate() const {
    // Validate kartParam.bin
    if (!m_kartParam.m_file || m_kartParam.m_size == 0) {
        return false;
    }

    auto *file = reinterpret_cast<ParamFile *>(m_kartParam.m_file);
    if (m_kartParam.m_size != parse<u32>(file->m_count) * sizeof(KartParam::Stats) + 4) {
        return false;
    }

    // Validate driverParam.bin
    if (!m_driverParam.m_file || m_driverParam.m_size == 0) {
        return false;
    }

    file = reinterpret_cast<ParamFile *>(m_driverParam.m_file);
    if (m_driverParam.m_size != parse<u32>(file->m_count) * sizeof(KartParam::Stats) + 4) {
        return false;
    }

    return true;
}

void KartParamFileManager::FileInfo::clear() {
    m_file = nullptr;
    m_size = 0;
}

void KartParamFileManager::FileInfo::load(const char *filename) {
    auto *resourceManager = System::ResourceManager::Instance();
    m_file = resourceManager->getFile(filename, &m_size, 0);
}

KartParamFileManager *KartParamFileManager::s_instance = nullptr;

} // namespace Kart
