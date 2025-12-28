#include "ObjectCrane.hh"

namespace Field {

/// @addr{0x807FE658}
ObjectCrane::ObjectCrane(const System::MapdataGeoObj &params)
    : ObjectKCL(params), m_startPos(m_pos), m_xPeriod(std::max<s16>(2, params.setting(1))),
      m_yPeriod(std::max<s16>(2, params.setting(4))), m_xAmplitude(params.setting(2)),
      m_yAmplitude(params.setting(5)), m_xFreq(2.0f * F_PI / static_cast<f32>(m_xPeriod)),
      m_yFreq(2.0f * F_PI / static_cast<f32>(m_yPeriod)) {
    m_xt = params.setting(3);
    m_yt = 0;
}

/// @addr{0x807FEB28}
ObjectCrane::~ObjectCrane() = default;

/// @addr{0x807FE7EC}
void ObjectCrane::calc() {
    const EGG::Vector3f prevPos = m_pos;

    f32 xDelta = EGG::Mathf::cos(m_xFreq * static_cast<f32>(m_xt));
    EGG::Vector3f scaledX = EGG::Vector3f::ex * xDelta * static_cast<f32>(m_xAmplitude);

    f32 yDelta = EGG::Mathf::cos(m_yFreq * static_cast<f32>(m_yt));
    EGG::Vector3f scaledY = EGG::Vector3f::ey * yDelta * static_cast<f32>(m_yAmplitude);

    calcTransform();

    m_pos = m_startPos + m_transform.multVector33(scaledX + scaledY);
    m_flags.setBit(eFlags::Position);

    if (m_yt++ > m_yPeriod) {
        m_yt = 0;
    }

    if (m_xt++ > m_xPeriod) {
        m_xt = 0;
    }

    setMovingObjVel(m_pos - prevPos);
}

} // namespace Field
