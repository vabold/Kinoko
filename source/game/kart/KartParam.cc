#include "KartParam.hh"

#include "game/kart/KartParamFileManager.hh"

namespace Kart {

KartParam::KartParam(Character character, Vehicle vehicle) {
    initStats(character, vehicle);
    initHitboxes(vehicle);
}

KartParam::~KartParam() = default;

void KartParam::initStats(Character character, Vehicle vehicle) {
    auto *fileManager = KartParamFileManager::Instance();

    auto vehicleStream = fileManager->getVehicleStream(vehicle);
    auto driverStream = fileManager->getDriverStream(character);

    m_stats = Stats(vehicleStream);
    m_stats.applyCharacterBonus(driverStream);
}

// TODO: Add BSP parsing
void KartParam::initHitboxes(Vehicle vehicle) {
    (void)vehicle;
}

KartParam::Stats::Stats() = default;

KartParam::Stats::Stats(EGG::RamStream &stream) {
    read(stream);
}

void KartParam::Stats::read(EGG::RamStream &stream) {
    m_body = static_cast<Body>(stream.read_s32());
    m_driftType = static_cast<DriftType>(stream.read_s32());
    m_weightClass = static_cast<WeightClass>(stream.read_s32());
    _00c = stream.read_f32();
    m_weight = stream.read_f32();
    m_bumpDeviationLevel = stream.read_f32();
    m_speed = stream.read_f32();
    m_turningSpeed = stream.read_f32();
    m_tilt = stream.read_f32();
    m_accelerationStandardA[0] = stream.read_f32();
    m_accelerationStandardA[1] = stream.read_f32();
    m_accelerationStandardA[2] = stream.read_f32();
    m_accelerationStandardA[3] = stream.read_f32();
    m_accelerationStandardT[0] = stream.read_f32();
    m_accelerationStandardT[1] = stream.read_f32();
    m_accelerationStandardT[2] = stream.read_f32();
    m_accelerationDriftA[0] = stream.read_f32();
    m_accelerationDriftA[1] = stream.read_f32();
    m_accelerationDriftT[0] = stream.read_f32();
    m_handlingManualTightness = stream.read_f32();
    m_handlingAutomaticTightness = stream.read_f32();
    m_handlingReactivity = stream.read_f32();
    m_driftManualTightness = stream.read_f32();
    m_driftAutomaticTightness = stream.read_f32();
    m_driftReactivity = stream.read_f32();
    m_driftOutsideTargetAngle = stream.read_f32();
    m_driftOutsideDecrement = stream.read_f32();
    m_miniTurbo = stream.read_u32();

    for (size_t i = 0; i < m_kclSpeed.size(); ++i) {
        m_kclSpeed[i] = stream.read_f32();
    }
    for (size_t i = 0; i < m_kclRot.size(); ++i) {
        m_kclRot[i] = stream.read_f32();
    }

    m_itemUnk170 = stream.read_f32();
    m_itemUnk174 = stream.read_f32();
    m_itemUnk178 = stream.read_f32();
    m_itemUnk17c = stream.read_f32();
    m_maxNormalAcceleration = stream.read_f32();
    m_megaScale = stream.read_f32();
    m_wheelDistance = stream.read_f32();
}

void KartParam::Stats::applyCharacterBonus(EGG::RamStream &stream) {
    stream.skip(0x10);
    m_weight += stream.read_f32();

    stream.skip(0x4);
    m_speed += stream.read_f32();
    m_turningSpeed += stream.read_f32();

    stream.skip(0x4);
    m_accelerationStandardA[0] += stream.read_f32();
    m_accelerationStandardA[1] += stream.read_f32();
    m_accelerationStandardA[2] += stream.read_f32();
    m_accelerationStandardA[3] += stream.read_f32();
    m_accelerationStandardA[0] += stream.read_f32();
    m_accelerationStandardA[1] += stream.read_f32();
    m_accelerationStandardA[2] += stream.read_f32();
    m_accelerationDriftA[0] += stream.read_f32();
    m_accelerationDriftA[1] += stream.read_f32();
    m_accelerationDriftT[0] += stream.read_f32();
    m_handlingManualTightness += stream.read_f32();
    m_handlingAutomaticTightness += stream.read_f32();
    m_handlingReactivity += stream.read_f32();
    m_driftManualTightness += stream.read_f32();
    m_driftAutomaticTightness += stream.read_f32();
    m_driftReactivity += stream.read_f32();
    m_driftOutsideTargetAngle += stream.read_f32();
    m_driftOutsideDecrement += stream.read_f32();
    m_miniTurbo += stream.read_u32();

    for (size_t i = 0; i < m_kclSpeed.size(); ++i) {
        m_kclSpeed[i] += stream.read_f32();
    }

    for (size_t i = 0; i < m_kclRot.size(); ++i) {
        m_kclRot[i] += stream.read_f32();
    }
}

} // namespace Kart
