#include "ObjectWLWallGC.hh"

#include "game/system/RaceManager.hh"

namespace Field {

/// @addr{0x8086BC1C}
ObjectWLWallGC::ObjectWLWallGC(const System::MapdataGeoObj &params) : ObjectKCL(params) {
    m_extendedDuration = params.setting(1);
    m_startFrame = params.setting(4);
    u32 rate = params.setting(2);

    if (rate == 0) {
        m_moveDuration = -1;
        m_hiddenDuration = -1;
        m_extendedFrame = -1;
        m_retractingFrame = -1;
        m_cycleDuration = -1;
    } else {
        m_moveDuration = params.setting(3) / rate;
        m_hiddenDuration = params.setting(0);
        m_extendedFrame = m_hiddenDuration + m_moveDuration;
        m_retractingFrame = m_extendedFrame + m_extendedDuration;
        m_cycleDuration = m_retractingFrame + m_moveDuration;
    }

    m_currPos = m_pos;

    calcTransform();

    u32 scalar = params.setting(3);
    m_posOffset = m_currPos - m_transform.base(2) * static_cast<f32>(scalar);

    calcTransform();
    m_currTransform = m_transform;
}

/// @addr{0x8086BDE4}
ObjectWLWallGC::~ObjectWLWallGC() = default;

/// @addr{0x8086BE34}
void ObjectWLWallGC::init() {
    m_flags |= 1;
    m_pos = m_currPos;

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
    s32 t = System::RaceManager::Instance()->timer() - timeOffset;
    t = t < m_startFrame ? 0 : (t - m_startFrame) % m_cycleDuration;

    f32 scalar;
    if (t < m_hiddenDuration) {
        scalar = 0.0f;
    } else if (t < m_extendedFrame) {
        scalar = static_cast<f32>(t - m_hiddenDuration) / static_cast<f32>(m_moveDuration);
    } else if (t < m_retractingFrame) {
        scalar = 1.0f;
    } else {
        scalar = 1.0f - static_cast<f32>(t - m_retractingFrame) / static_cast<f32>(m_moveDuration);
    }

    m_currTransform.setBase(3, interpolate(scalar, m_currPos, m_posOffset));

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

EGG::Vector3f ObjectWLWallGC::interpolate(f32 t, const EGG::Vector3f &v0,
        const EGG::Vector3f &v1) const {
    return v0 + (v1 - v0) * t;
}

} // namespace Field
