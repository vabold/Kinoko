#include "ObjectWLWallGC.hh"

#include "game/system/RaceManager.hh"

namespace Field {

/// @addr{0x8086BC1C}
ObjectWLWallGC::ObjectWLWallGC(const System::MapdataGeoObj &params)
    : ObjectKCL(params), m_startFrame(params.setting(4)) {
    f32 extendedDuration = params.setting(1);
    s16 rate = params.setting(2);
    s16 distance = params.setting(3);

    if (rate == 0) {
        m_moveDuration = std::numeric_limits<u32>::max();
        m_hiddenDuration = -1;
        m_extendedFrame = -1;
        m_retractingFrame = -1;
        m_cycleDuration = -1;
    } else {
        m_moveDuration = static_cast<u32>(distance) / static_cast<u32>(rate);
        m_hiddenDuration = static_cast<s32>(params.setting(0));
        m_extendedFrame = m_hiddenDuration + m_moveDuration;
        m_retractingFrame = m_extendedFrame + extendedDuration;
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
    m_flags.setBit(eFlags::Position);
    m_pos = m_initialPos;

    calcTransform();
    m_currTransform = m_transform;
}

/// @addr{0x8086C108}
void ObjectWLWallGC::calc() {
    EGG::Vector3f prevPos = m_pos;

    m_flags.setBit(eFlags::Matrix);
    m_transform = getUpdatedMatrix(0);
    m_pos = m_transform.base(3);

    setMovingObjVel(m_pos - prevPos);
}

/// @addr{0x8086BF30}
const EGG::Matrix34f &ObjectWLWallGC::getUpdatedMatrix(u32 timeOffset) {
    u32 time = cycleFrame(System::RaceManager::Instance()->timer() - timeOffset);

    f32 t;
    if (time < static_cast<u32>(m_hiddenDuration)) {
        t = 0.0f;
    } else if (time < static_cast<u32>(m_extendedFrame)) {
        t = static_cast<f32>(time - static_cast<u32>(m_hiddenDuration)) /
                static_cast<f32>(m_moveDuration);
    } else if (time < static_cast<u32>(m_retractingFrame)) {
        t = 1.0f;
    } else {
        t = 1.0f -
                static_cast<f32>(time - static_cast<u32>(m_retractingFrame)) /
                        static_cast<f32>(m_moveDuration);
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
