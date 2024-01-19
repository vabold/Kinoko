#include "KartParam.hh"

#include "game/kart/KartParamFileManager.hh"

namespace Kart {

KartParam::KartParam(Character character, Vehicle vehicle, u8 playerIdx) {
    initStats(character, vehicle);
    initHitboxes(vehicle);
    m_playerIdx = playerIdx;
    m_isBike = vehicle >= Vehicle::Standard_Bike_S;
}

KartParam::~KartParam() = default;

const BSP &KartParam::bsp() const {
    return m_bsp;
}

u8 KartParam::playerIdx() const {
    return m_playerIdx;
}

bool KartParam::isBike() const {
    return m_isBike;
}

void KartParam::initStats(Character character, Vehicle vehicle) {
    auto *fileManager = KartParamFileManager::Instance();

    auto vehicleStream = fileManager->getVehicleStream(vehicle);
    auto driverStream = fileManager->getDriverStream(character);

    m_stats = Stats(vehicleStream);
    m_stats.applyCharacterBonus(driverStream);
}

void KartParam::initHitboxes(Vehicle vehicle) {
    auto *fileManager = KartParamFileManager::Instance();

    auto hitboxStream = fileManager->getHitboxStream(vehicle);
    m_bsp = BSP(hitboxStream);
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

BSP::BSP() = default;

BSP::BSP(EGG::RamStream &stream) {
    read(stream);
}

void BSP::read(EGG::RamStream &stream) {
    m_initialYPos = stream.read_f32();

    for (auto &hitbox : m_hitboxes) {
        hitbox.m_enable = stream.read_u16();
        stream.skip(2);
        hitbox.m_position.read(stream);
        hitbox.m_radius = stream.read_f32();
        hitbox.m_wallsOnly = stream.read_u16();
        hitbox.m_tireCollisionIdx = stream.read_u16();
    }

    m_cuboids[0].read(stream);
    m_cuboids[1].read(stream);
    m_angVel0Factor = stream.read_f32();
    _1a0 = stream.read_f32();

    for (auto &wheel : m_wheels) {
        wheel.m_enable = stream.read_u16();
        stream.skip(2);
        wheel.m_springStiffness = stream.read_f32();
        wheel.m_dampingFactor = stream.read_f32();
        wheel.m_maxTravel = stream.read_f32();
        wheel.m_relPosition.read(stream);
        wheel.m_xRot = stream.read_f32();
        wheel.m_wheelRadius = stream.read_f32();
        wheel.m_sphereRadius = stream.read_f32();
        wheel._28 = stream.read_u32();
    }

    m_rumbleHeight = stream.read_f32();
    m_rumbleSpeed = stream.read_f32();
}

} // namespace Kart
