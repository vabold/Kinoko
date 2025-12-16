#include "ObjectVolcanoRock.hh"

namespace Field {

/// @addr{0x8081A198}
ObjectVolcanoRock::ObjectVolcanoRock(const System::MapdataGeoObj &params)
    : ObjectKCL(params), m_initialPos(m_pos), m_initialRot(m_rot), m_phaseShift(params.setting(3)),
      m_zPeriod(std::max<s16>(params.setting(1), 2)),
      m_yPeriod(std::max<s16>(params.setting(4), 2)),
      m_zAmplitude(static_cast<f32>(params.setting(2))),
      m_yAmplitude(static_cast<f32>(params.setting(5))),
      m_zAngVel(6.2831855f / static_cast<f32>(m_zPeriod)),
      m_yAngVel(6.2831855f / static_cast<f32>(m_yPeriod)), m_variant(!!params.setting(0)) {
    m_pos = calcPos(0);
    m_flags.setBit(eFlags::Position);
}

/// @addr{0x8081A690}
ObjectVolcanoRock::~ObjectVolcanoRock() = default;

/// @addr{0x8081A370}
void ObjectVolcanoRock::calc() {
    EGG::Vector3f prevPos = m_pos;
    m_pos = calcPos(static_cast<s32>(System::RaceManager::Instance()->timer()));
    m_flags.setBit(eFlags::Position);
    setMovingObjVel(m_pos - prevPos);
}

/// @addr{0x8081A414}
EGG::Vector3f ObjectVolcanoRock::calcPos(s32 frame) {
    f32 tz = static_cast<f32>((frame + m_phaseShift) % m_zPeriod);
    f32 ty = static_cast<f32>(frame % m_yPeriod);

    auto zDisplacement = EGG::Vector3f::ez * EGG::Mathf::cos(m_zAngVel * tz) * m_zAmplitude;
    auto yDisplacement = EGG::Vector3f::ey * EGG::Mathf::cos(m_yAngVel * ty) * m_yAmplitude;

    calcTransform();

    return m_initialPos + m_transform.multVector33(zDisplacement + yDisplacement);
}

} // namespace Field
