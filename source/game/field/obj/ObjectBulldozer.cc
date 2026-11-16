#include "ObjectBulldozer.hh"

#include "game/system/RaceManager.hh"
#include "game/system/map/MapdataGeoObj.hh"

#include <cstring>

namespace Kinoko::Field {

/// @addr{0x807FD938}
/// @copybrief ObjectKCL::ObjectKCL(const System::MapdataGeoObj &)
/// @details Caches the object's initial position and rotation to @ref m_initialPos and @ref
/// m_initialRot respectively. Computes @ref m_timeOffset by doubling param setting 4. Computes
/// @ref m_period as the max of `2` and param setting 3. Sets @ref m_restFrames based on
/// param setting 5. Computes @ref m_fullPeriod by summing @ref m_period and twice @ref
/// m_restFrames. Sets @ref m_amplitude based on param setting 2. Determines @ref m_left by
/// comparing the object's name to `bulldozer_left`. Computes @ref m_angFreq as `2 * pi` divided by
/// @ref m_period and @ref m_halfPeriod as half of @ref m_fullPeriod.
ObjectBulldozer::ObjectBulldozer(const System::MapdataGeoObj &params)
    : ObjectKCL(params),
      m_initialPos(pos()),
      m_initialRot(rot()),
      m_timeOffset(params.setting(3) * 2),
      m_period(std::max<u16>(2, params.setting(2))),
      m_restFrames(params.setting(4)),
      m_fullPeriod(m_period + m_restFrames * 2),
      m_amplitude(params.setting(1)),
      m_left(strcmp(getName(), "bulldozer_left") == 0),
      m_angFreq(F_TAU / static_cast<f32>(m_period)),
      m_halfPeriod(m_fullPeriod / 2) {}

/// @addr{0x807FE5F0}
/// @brief Default virtual destructor
ObjectBulldozer::~ObjectBulldozer() = default;

/// @addr{0x807FDC50}
/// @copybrief ObjectBase::calc()
/// @details Updates the position of the bulldozer based on its oscillation on the current frame.
/// The direction of oscillation is flipped depending on whether @ref m_left is `true`. Finally,
/// sets moving object velocity based on the change in position this frame.
void ObjectBulldozer::calc() {
    u32 timer = System::RaceManager::Instance()->timer();
    f32 posOffset = calcPosOffset(m_timeOffset + timer);
    EGG::Vector3f prevPos = pos();
    f32 xPos = m_left ? m_initialPos.x + posOffset : m_initialPos.x - posOffset;
    setPos(EGG::Vector3f(xPos, prevPos.y, prevPos.z));
    setMovingObjVel(pos() - prevPos);
}

/// @addr{0x807FE5E8}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectBulldozer::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x807FE4FC}
/// @copybrief ObjectKCL::colRadiusAdditionalLength()
/// @return The amplitude of the bulldozer's oscillation, @ref m_amplitude.
f32 ObjectBulldozer::colRadiusAdditionalLength() const {
    return static_cast<f32>(m_amplitude);
}

/// @addr{0x807FE364}
/// @copybrief ObjectKCL::initCollision()
/// @details Calls the base class @ref ObjectKCL::initCollision() and computes @ref m_kclMidpoint
/// based on the object's transform, rather than based on the return value of `getUpdatedMatrix()`.
void ObjectBulldozer::initCollision() {
    calcTransform();

    EGG::Matrix34f matInv;
    transform().ps_inverse(matInv);
    m_objColMgr->setMtx(transform());
    m_objColMgr->setInvMtx(matInv);
    m_objColMgr->setScale(getScaleY(0));

    EGG::Vector3f midpoint =
            (m_objColMgr->kclHighWorld() + m_objColMgr->kclLowWorld()).multInv(2.0f);

    ObjectKCL::initCollision();

    m_kclMidpoint = midpoint;
}

/// @addr{0x807FE534}
/// @copybrief ObjectKCL::getUpdatedMatrix()
/// @param timeOffset The time offset used to calculate the current frame's transformation
/// @return Const ref to the updated transformation matrix for the current frame, taking into
/// account the time offset.
/// @details Rotation stays constant. The position offset is based off the bulldozer's facing
/// direction and its oscillation, computed via @ref calcPosOffset().
const EGG::Matrix34f &ObjectBulldozer::getUpdatedMatrix(u32 timeOffset) {
    EGG::Vector3f pos = m_initialPos;
    u32 timer = System::RaceManager::Instance()->timer();
    f32 posOffset = calcPosOffset(m_timeOffset + timer - timeOffset);

    pos.x = m_left ? pos.x + posOffset : pos.x - posOffset;
    m_rtMat.makeRT(m_initialRot, pos);

    return m_rtMat;
}

/// @addr{0x807FDE5C}
/// @brief Computes the position offset from the bulldozer's initial position on the provided frame
/// @param t The frame during which to calculate the position offset
/// @return The position offset from the bulldozer's initial position on frame `t`.
/// @details Maps `t` to the frame within the bulldozer's oscillation period. Adjusts the resulting
/// frame based on where it falls relative to the two resting periods. This is then passed into a
/// `cosine` function with an angular frequency of @ref m_angFreq and an amplitude of @ref
/// m_amplitude.
f32 ObjectBulldozer::calcPosOffset(u32 t) const {
    u16 phase = t % m_fullPeriod;

    if (phase >= m_halfPeriod - m_restFrames) {
        if (phase < m_halfPeriod) {
            phase = m_period / 2;
        } else if (phase < m_fullPeriod - m_restFrames) {
            phase -= m_restFrames;
        } else {
            phase = m_period;
        }
    }

    return static_cast<f32>(m_amplitude) *
            (1.0f + EGG::Mathf::cos(m_angFreq * static_cast<f32>(phase))) * 0.5f;
}

} // namespace Kinoko::Field
