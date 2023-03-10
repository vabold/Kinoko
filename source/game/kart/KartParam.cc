#include "KartParam.hh"

#include "source/game/system/ResourceManager.hh"

namespace Kart {

KartParam::KartParam(Character character, Vehicle vehicle) {
    initStats(character, vehicle);
    initHitboxes(vehicle);
}

KartParam::~KartParam() = default;

void KartParam::initStats(Character character, Vehicle vehicle) {
    auto *fileManager = KartParamFileManager::Instance();
    Stats characterStats{};
    Stats vehicleStats{};

    fileManager->stats(characterStats, character);
    fileManager->stats(vehicleStats, vehicle);

    m_stats = characterStats + vehicleStats;
}

// TODO: Add BSP parsing
void KartParam::initHitboxes(Vehicle vehicle) {
    (void)vehicle;
}

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

void KartParamFileManager::stats(KartParam::Stats &stats, Character character) {
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
    auto &rawStats = file->m_params[idx];
    stats.read(rawStats);
}

void KartParamFileManager::stats(KartParam::Stats &stats, Vehicle vehicle) {
    if (vehicle >= Vehicle::Max) {
        K_PANIC("Uh oh.");
    }

    s32 idx = static_cast<s32>(vehicle);
    auto *file = reinterpret_cast<ParamFile *>(m_kartParam.m_file);
    auto &rawStats = file->m_params[idx];
    stats.read(rawStats);
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

KartParamFileManager::KartParamFileManager() = default;

KartParamFileManager::~KartParamFileManager() = default;

bool KartParamFileManager::validate() {
    // Validate kartParam.bin
    if (!m_kartParam.m_file || m_kartParam.m_size == 0) {
        return false;
    }

    auto *file = reinterpret_cast<ParamFile *>(m_kartParam.m_file);
    if (m_kartParam.m_size !=
            parse<u32>(file->m_count, std::endian::big) * sizeof(KartParam::Stats) + 4) {
        return false;
    }

    // Validate driverParam.bin
    if (!m_driverParam.m_file || m_driverParam.m_size == 0) {
        return false;
    }

    file = reinterpret_cast<ParamFile *>(m_driverParam.m_file);
    if (m_driverParam.m_size !=
            parse<u32>(file->m_count, std::endian::big) * sizeof(KartParam::Stats) + 4) {
        return false;
    }

    return true;
}

KartParam::Stats KartParam::Stats::operator+(const Stats &ext) {
    Stats stats = *this;
    stats.m_weight += ext.m_weight;
    stats.m_speed += ext.m_speed;
    stats.m_turningSpeed += ext.m_turningSpeed;
    stats.m_accelerationStandardA[0] += ext.m_accelerationStandardA[0];
    stats.m_accelerationStandardA[1] += ext.m_accelerationStandardA[1];
    stats.m_accelerationStandardA[2] += ext.m_accelerationStandardA[2];
    stats.m_accelerationStandardA[3] += ext.m_accelerationStandardA[3];
    stats.m_accelerationStandardT[0] += ext.m_accelerationStandardT[0];
    stats.m_accelerationStandardT[1] += ext.m_accelerationStandardT[1];
    stats.m_accelerationStandardT[2] += ext.m_accelerationStandardT[2];
    stats.m_accelerationDriftA[0] += ext.m_accelerationDriftA[0];
    stats.m_accelerationDriftA[1] += ext.m_accelerationDriftA[1];
    stats.m_accelerationDriftT[0] += ext.m_accelerationDriftT[0];
    stats.m_handlingManualTightness += ext.m_handlingManualTightness;
    stats.m_handlingAutomaticTightness += ext.m_handlingAutomaticTightness;
    stats.m_handlingReactivity += ext.m_handlingReactivity;
    stats.m_driftManualTightness += ext.m_driftManualTightness;
    stats.m_driftAutomaticTightness += ext.m_driftAutomaticTightness;
    stats.m_driftReactivity += ext.m_driftReactivity;
    stats.m_driftOutsideTargetAngle += ext.m_driftOutsideTargetAngle;
    stats.m_driftOutsideDecrement += ext.m_driftOutsideDecrement;

    for (size_t i = 0; i < sizeof(m_kclSpeed) / sizeof(f32); ++i) {
        stats.m_kclSpeed[i] += ext.m_kclSpeed[i];
    }

    for (size_t i = 0; i < sizeof(m_kclRot) / sizeof(f32); ++i) {
        stats.m_kclRot[i] += ext.m_kclRot[i];
    }

    return stats;
}

void KartParam::Stats::read(Stats &raw) {
    m_body = static_cast<Body>(parse<s32>(static_cast<s32>(raw.m_body), std::endian::big));
    m_driftType =
            static_cast<DriftType>(parse<s32>(static_cast<s32>(raw.m_driftType), std::endian::big));
    m_weightClass = static_cast<WeightClass>(
            parse<s32>(static_cast<s32>(raw.m_weightClass), std::endian::big));
    _00c = parsef(raw._00c, std::endian::big);
    m_weight = parsef(raw.m_weight, std::endian::big);
    m_bumpDeviationLevel = parsef(raw.m_bumpDeviationLevel, std::endian::big);
    m_speed = parsef(raw.m_speed, std::endian::big);
    m_turningSpeed = parsef(raw.m_turningSpeed, std::endian::big);
    m_tilt = parsef(raw.m_tilt, std::endian::big);
    m_accelerationStandardA[0] = parsef(raw.m_accelerationStandardA[0], std::endian::big);
    m_accelerationStandardA[1] = parsef(raw.m_accelerationStandardA[1], std::endian::big);
    m_accelerationStandardA[2] = parsef(raw.m_accelerationStandardA[2], std::endian::big);
    m_accelerationStandardA[3] = parsef(raw.m_accelerationStandardA[3], std::endian::big);
    m_accelerationStandardT[0] = parsef(raw.m_accelerationStandardT[0], std::endian::big);
    m_accelerationStandardT[1] = parsef(raw.m_accelerationStandardT[1], std::endian::big);
    m_accelerationStandardT[2] = parsef(raw.m_accelerationStandardT[2], std::endian::big);
    m_accelerationDriftA[0] = parsef(raw.m_accelerationDriftA[0], std::endian::big);
    m_accelerationDriftA[1] = parsef(raw.m_accelerationDriftA[1], std::endian::big);
    m_accelerationDriftT[0] = parsef(raw.m_accelerationDriftT[0], std::endian::big);
    m_handlingManualTightness = parsef(raw.m_handlingManualTightness, std::endian::big);
    m_handlingAutomaticTightness = parsef(raw.m_handlingAutomaticTightness, std::endian::big);
    m_handlingReactivity = parsef(raw.m_handlingReactivity, std::endian::big);
    m_driftManualTightness = parsef(raw.m_driftManualTightness, std::endian::big);
    m_driftAutomaticTightness = parsef(raw.m_driftAutomaticTightness, std::endian::big);
    m_driftReactivity = parsef(raw.m_driftReactivity, std::endian::big);
    m_driftOutsideTargetAngle = parsef(raw.m_driftOutsideTargetAngle, std::endian::big);
    m_driftOutsideDecrement = parsef(raw.m_driftOutsideDecrement, std::endian::big);
    m_miniTurbo = parse<u32>(raw.m_miniTurbo, std::endian::big);

    for (size_t i = 0; i < sizeof(m_kclSpeed) / sizeof(f32); ++i) {
        m_kclSpeed[i] = parsef(raw.m_kclSpeed[i], std::endian::big);
    }
    for (size_t i = 0; i < sizeof(m_kclRot) / sizeof(f32); ++i) {
        m_kclRot[i] = parsef(raw.m_kclRot[i], std::endian::big);
    }

    m_itemUnk170 = parsef(raw.m_itemUnk170, std::endian::big);
    m_itemUnk174 = parsef(raw.m_itemUnk174, std::endian::big);
    m_itemUnk178 = parsef(raw.m_itemUnk178, std::endian::big);
    m_itemUnk17c = parsef(raw.m_itemUnk17c, std::endian::big);
    m_maxNormalAcceleration = parsef(raw.m_maxNormalAcceleration, std::endian::big);
    m_megaScale = parsef(raw.m_megaScale, std::endian::big);
    m_wheelDistance = parsef(raw.m_wheelDistance, std::endian::big);
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
