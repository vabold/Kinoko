#include "ObjectWLWallGC.hh"

#include "game/system/RaceManager.hh"

namespace Field {

/// @addr{0x8086BC1C}
ObjectWLWallGC::ObjectWLWallGC(const System::MapdataGeoObj &params) : ObjectKCL(params) {
    m_extendedDuration = params.setting(1);
    m_startFrame = params.setting(4);
    u32 rate = params.setting(2);
    u16 distance = params.setting(3);

    if (rate == 0) {
        m_moveDuration = -1;
        m_hiddenDuration = -1;
        m_extendedFrame = -1;
        m_retractingFrame = -1;
        m_cycleDuration = -1;
    } else {
        m_moveDuration = distance / rate;
        m_hiddenDuration = params.setting(0);
        m_extendedFrame = m_hiddenDuration + m_moveDuration;
        m_retractingFrame = m_extendedFrame + m_extendedDuration;
        m_cycleDuration = m_retractingFrame + m_moveDuration;
    }

    m_initialPos = m_pos;

    calcTransform();

    m_targetPos = m_initialPos - m_transform.base(2) * static_cast<f32>(distance);

    calcTransform();
    m_currTransform = m_transform;
}

/// @addr{0x8086BDE4}
ObjectWLWallGC::~ObjectWLWallGC() = default;

/// @addr{0x8086BE34}
void ObjectWLWallGC::init() {
    m_flags |= 1;
    m_pos = m_initialPos;

    calcTransform();
    m_currTransform = m_transform;
}

/// @addr{0x8086C108}
void ObjectWLWallGC::calc() {
    EGG::Vector3f prevPos = m_pos;

    m_flags |= 4;
    m_transform = getUpdatedMatrix(0);
    m_pos = m_transform.base(3);

    setMovingObjVel(m_pos - prevPos);
}

/// @addr{0x8086BF30}
const EGG::Matrix34f &ObjectWLWallGC::getUpdatedMatrix(u32 timeOffset) {
    s32 time = cycleFrame(System::RaceManager::Instance()->timer() - timeOffset);

    f32 t;
    if (time < m_hiddenDuration) {
        t = 0.0f;
    } else if (time < m_extendedFrame) {
        t = static_cast<f32>(time - m_hiddenDuration) / static_cast<f32>(m_moveDuration);
    } else if (time < m_retractingFrame) {
        t = 1.0f;
    } else {
        t = 1.0f - static_cast<f32>(time - m_retractingFrame) / static_cast<f32>(m_moveDuration);
    }

    m_currTransform.setBase(3, Interpolate(t, m_initialPos, m_targetPos));

    return m_currTransform;
}

/// @addr{0x8086C328}
bool ObjectWLWallGC::checkCollision(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    update(timeOffset);
    calcScale(timeOffset);

    return m_objColMgr->checkSphereFullPush(radius, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x8086C5A8}
bool ObjectWLWallGC::checkCollisionCached(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    update(timeOffset);
    calcScale(timeOffset);

    return m_objColMgr->checkSphereCachedFullPush(radius, pos, prevPos, mask, info, maskOut);
}

} // namespace Field
