#include "game/field/obj/ObjectBulldozer.hh"

#include "game/system/RaceManager.hh"

#include <cstring>

namespace Field {

/// @addr{0x807FD938}
ObjectBulldozer::ObjectBulldozer(const System::MapdataGeoObj &params)
    : ObjectKCL(params), m_initialPos(m_pos), m_initialRot(m_rot),
      m_timeOffset(params.setting(3) * 2), m_periodDenom(std::max<s16>(2, params.setting(2))),
      m_restFrames(params.setting(4)), m_fullPeriod(m_periodDenom + m_restFrames * 2),
      m_amplitude(params.setting(1)), m_left(strcmp(getName(), "bulldozer_left") == 0),
      m_period(F_TAU / static_cast<f32>(m_periodDenom)), m_halfPeriod(m_fullPeriod / 2) {}

/// @addr{0x807FE5F0}
ObjectBulldozer::~ObjectBulldozer() = default;

/// @addr{0x807FDC50}
void ObjectBulldozer::calc() {
    u32 timer = System::RaceManager::Instance()->timer();
    f32 posOffset = calcStateAndPosition(m_timeOffset + timer);
    EGG::Vector3f prevPos = m_pos;

    m_flags.setBit(eFlags::Position);
    m_pos.x = m_left ? m_initialPos.x + posOffset : m_initialPos.x - posOffset;

    setMovingObjVel(m_pos - prevPos);
}

/// @addr{0x807FE364}
void ObjectBulldozer::initCollision() {
    calcTransform();

    EGG::Matrix34f matInv;
    m_transform.ps_inverse(matInv);

    calcTransform();

    m_objColMgr->setMtx(m_transform);
    m_objColMgr->setInvMtx(matInv);
    m_objColMgr->setScale(getScaleY(0));

    ObjectKCL::initCollision();

    m_kclMidpoint = (m_objColMgr->kclHighWorld() + m_objColMgr->kclLowWorld()).multInv(2.0f);
}

/// @addr{0x807FE534}
const EGG::Matrix34f &ObjectBulldozer::getUpdatedMatrix(u32 timeOffset) {
    EGG::Vector3f pos = m_initialPos;
    u32 timer = System::RaceManager::Instance()->timer();
    f32 posOffset = calcStateAndPosition(m_timeOffset + timer - timeOffset);

    pos.x = m_left ? pos.x + posOffset : pos.x - posOffset;
    m_rtMat.makeRT(m_initialRot, pos);

    return m_rtMat;
}

/// @addr{0x807FE03C}
bool ObjectBulldozer::checkCollision(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    update(timeOffset);
    calcScale(timeOffset);

    return m_objColMgr->checkSphereFullPush(radius, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x807FE2CC}
bool ObjectBulldozer::checkCollisionCached(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    update(timeOffset);
    calcScale(timeOffset);

    return m_objColMgr->checkSphereCachedFullPush(radius, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x807FDE5C}
f32 ObjectBulldozer::calcStateAndPosition(u32 timeOffset) const {
    u16 t = timeOffset % m_fullPeriod;

    if (t >= m_halfPeriod - m_restFrames) {
        if (t < m_halfPeriod) {
            t = m_periodDenom / 2;
        } else if (t < m_fullPeriod - m_restFrames) {
            t -= m_restFrames;
        } else {
            t = m_periodDenom;
        }
    }

    return static_cast<f32>(m_amplitude) *
            (1.0f + EGG::Mathf::cos(m_period * static_cast<f32>(t))) * 0.5f;
}

} // namespace Field
