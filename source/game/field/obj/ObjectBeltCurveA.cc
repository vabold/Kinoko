#include "ObjectBeltCurveA.hh"

namespace Field {

/// @addr{0x807FC90C}
ObjectBeltCurveA::ObjectBeltCurveA(const System::MapdataGeoObj &params) : ObjectBelt(params) {
    constexpr EGG::Vector3f INITIAL_ROT = EGG::Vector3f(0.0f, 1.5707964f, 0.0f);

    m_startForward = params.setting(1) == 1;
    m_dirChange1Frame = params.setting(2) * 60;
    m_dirChange2Frame = params.setting(3) * 60;
    m_initMat.makeR(INITIAL_ROT);
}

/// @addr{0x807FD7BC}
ObjectBeltCurveA::~ObjectBeltCurveA() = default;

/// @addr{0x807FC9D4}
EGG::Vector3f ObjectBeltCurveA::calcRoadVelocity(u32 variant, const EGG::Vector3f &pos,
        s32 timeOffset) const {
    EGG::Vector3f posDelta = pos - m_pos;
    posDelta.y = 0.0f;

    EGG::Vector3f dir = m_initMat.ps_multVector(posDelta);
    bool forward;

    if (timeOffset <= m_dirChange1Frame) {
        forward = m_startForward;
    } else if (timeOffset <= m_dirChange2Frame) {
        forward = !m_startForward;
    } else {
        forward = m_startForward;
    }

    if (variant == 4) {
        f32 sign = forward ? 1.0f : -1.0f;
        return dir * sign * calcDirSwitchVelocity(timeOffset);
    } else if (variant == 5) {
        f32 sign = forward ? -1.0f : 1.0f;
        return dir * sign * calcDirSwitchVelocity(timeOffset);
    } else {
        return EGG::Vector3f::zero;
    }
}

/// @addr{0x807FD5A0}
f32 ObjectBeltCurveA::calcDirSwitchVelocity(u32 t) const {
    s32 change1Delta = static_cast<s32>(t) - static_cast<s32>(m_dirChange1Frame);
    s32 change2Delta = static_cast<s32>(t) - static_cast<s32>(m_dirChange2Frame);
    u32 change1DeltaAbs = static_cast<u32>(EGG::Mathf::abs(static_cast<f32>(change1Delta)));
    u32 change2DeltaAbs = static_cast<u32>(EGG::Mathf::abs(static_cast<f32>(change2Delta)));
    u32 delta = std::min(change1DeltaAbs, change2DeltaAbs);

    if (static_cast<f32>(delta) > 60.0f) {
        return 0.006f;
    } else {
        return 0.006f * static_cast<f32>(delta) / 60.0f;
    }
}

} // namespace Field
