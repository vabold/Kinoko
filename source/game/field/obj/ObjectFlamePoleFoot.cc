#include "ObjectFlamePoleFoot.hh"

#include "game/field/CollisionDirector.hh"

#include "game/system/RaceManager.hh"

#include <algorithm>

namespace Field {

/// @addr{0x8067E6F4}
ObjectFlamePoleFoot::ObjectFlamePoleFoot(const System::MapdataGeoObj &params)
    : ObjectKCL(params), StateManager(this, STATE_ENTRIES), m_extraCycleFrames(params.setting(0)),
      m_initDelay(params.setting(1)) {
    ++FLAMEPOLE_COUNT;

    m_poleScale = static_cast<f32>(params.setting(2));
    if (m_poleScale == 0.0f) {
        m_poleScale = 3.0f + static_cast<f32>(FLAMEPOLE_COUNT % 3);
    }

    m_pole = new ObjectFlamePole(params, m_pos, m_rot, m_scale);
    m_pole->load();
}

/// @addr{0x8067EBE0}
ObjectFlamePoleFoot::~ObjectFlamePoleFoot() {
    FLAMEPOLE_COUNT = 0;
}

/// @addr{0x8067EC94}
void ObjectFlamePoleFoot::init() {
    constexpr f32 NORMALIZATION = static_cast<f32>(CYCLE_FRAMES) * (7.0f - 1.0f);

    m_nextStateId = 0;
    m_cycleFrame = 0;
    m_eruptUpDuration = static_cast<s32>(0.1f * NORMALIZATION / 7.0f);
    m_eruptDownDuration = static_cast<s32>(0.2f * NORMALIZATION / 7.0f);

    s32 state1 = static_cast<s32>(0.3f * NORMALIZATION / 7.0f);
    s32 state2 = state1 + m_eruptUpDuration;
    s32 state3 = state2 + static_cast<s32>(static_cast<f32>(CYCLE_FRAMES) * 1.0f / 7.0f);
    s32 state4 = state3 + m_eruptDownDuration;
    s32 state5 = state4 + static_cast<s32>(0.4f * NORMALIZATION / 7.0f);
    m_stateStart = {{0, state1, state2, state3, state4, state5}};

    m_eruptDownVel = (m_poleScale - 1.0f) / static_cast<f32>(state1);
    m_initScaledHeight = ObjectFlamePole::HEIGHT * m_poleScale;
    m_scaleDelta = (300.0f + m_initScaledHeight) / static_cast<f32>(m_eruptDownDuration);
    m_pole->setActive(false);
    m_pole->disableCollision();

    EGG::Vector3f polePos = m_pole->pos();
    polePos.y = m_pos.y - ObjectFlamePole::HEIGHT * m_poleScale;
    m_pole->setPos(polePos);
}

/// @addr{0x8067EF70}
void ObjectFlamePoleFoot::calc() {
    if (System::RaceManager::Instance()->timer() < static_cast<u32>(m_initDelay)) {
        return;
    }

    calcStates();

    StateManager::calc();

    f32 scale = getScaleY(0);
    m_flags.setBit(eFlags::Scale);
    m_scale = EGG::Vector3f(scale, scale, scale);

    EGG::Vector3f polePos = m_pole->pos();
    m_pole->setPos(
            EGG::Vector3f(polePos.x, m_heightOffset + (m_pos.y - m_initScaledHeight), polePos.z));
    m_pole->setScale(EGG::Vector3f(m_poleScale, m_poleScale, m_poleScale));
}

/// @addr{0x8067F6B8}
void ObjectFlamePoleFoot::calcStates() {
    u32 frame = static_cast<s32>(
            System::RaceManager::Instance()->timer() - static_cast<u32>(m_initDelay));
    m_cycleFrame = frame % (m_extraCycleFrames + CYCLE_FRAMES);

    // Access the array of state timers to see which state corresponds with the current frame.
    auto it = std::ranges::upper_bound(m_stateStart.begin(), m_stateStart.end(), m_cycleFrame);
    auto idx = it - m_stateStart.begin() - 1;

    if (idx < 0) {
        return;
    }

    u16 stateId = static_cast<u16>(idx);

    if (m_currentStateId != stateId) {
        m_nextStateId = stateId;
    }
}

f32 ObjectFlamePoleFoot::getScaleY(u32 timeOffset) const {
    s32 frame = System::RaceManager::Instance()->timer() - timeOffset;
    if (frame < m_initDelay) {
        return 1.0f;
    }

    s32 cycleFrame = frame - m_initDelay;
    cycleFrame %= m_extraCycleFrames + CYCLE_FRAMES;

    if (cycleFrame >= m_stateStart[5]) {
        return 1.0f;
    }

    if (cycleFrame >= m_stateStart[4]) {
        return std::max(1.0f,
                m_poleScale - m_eruptDownVel * static_cast<f32>(m_eruptDownDuration / 2) -
                        m_eruptDownVel * static_cast<f32>(cycleFrame - m_stateStart[4]));
    }

    if (cycleFrame >= m_stateStart[3]) {
        s32 framesSince194 = cycleFrame - m_stateStart[3];
        s32 half17c = m_eruptDownDuration / 2;
        if (framesSince194 > half17c) {
            return m_poleScale - m_eruptDownVel * static_cast<f32>(framesSince194 - half17c);
        } else {
            return m_poleScale;
        }
    }

    if (cycleFrame >= m_stateStart[2] || cycleFrame >= m_stateStart[1]) {
        return m_poleScale;
    }

    if (cycleFrame >= 0) {
        return std::min(m_poleScale, m_eruptDownVel * static_cast<f32>(cycleFrame) + 1.0f);
    }

    return 1.0f;
}

/// @addr{0x8067FE88}
bool ObjectFlamePoleFoot::checkCollision(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    update(timeOffset);
    calcScale(timeOffset);
    f32 scale = getScaleY(timeOffset);

    if (!m_objColMgr->checkSphereFullPush(radius, pos, prevPos, mask, info, maskOut)) {
        return false;
    }

    if (2.0f < scale) {
        CollisionDirector::Instance()->setCurrentCollisionTrickable(true);
    }

    return true;
}

/// @addr{0x80680218}
bool ObjectFlamePoleFoot::checkCollisionCached(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    update(timeOffset);
    calcScale(timeOffset);
    f32 scale = getScaleY(timeOffset);

    if (!m_objColMgr->checkSphereCachedFullPush(radius, pos, prevPos, mask, info, maskOut)) {
        return false;
    }

    if (2.0f < scale) {
        CollisionDirector::Instance()->setCurrentCollisionTrickable(true);
    }

    return true;
}

/// @addr{0x8067F2F4}
void ObjectFlamePoleFoot::enterEruptingUp() {
    m_pole->setActive(true);
    m_pole->enableCollision();
    m_heightOffset = 0.0f;

    f32 t1 = -1.0f;
    f32 t2 = -1.0f;
    f32 vel = m_initScaledHeight;
    EGG::Mathf::FindRootsQuadratic(static_cast<f32>(m_eruptUpDuration * m_eruptUpDuration),
            -4.0f * m_initScaledHeight * static_cast<f32>(m_eruptUpDuration),
            4.0f * m_initScaledHeight * m_initScaledHeight, t1, t2);

    if (t1 <= 0.0f) {
        t1 = -1.0f;
    }

    if (t2 <= 0.0f) {
        vel = t1;
    }

    m_initEruptVel = vel;
    m_eruptAccel = vel * vel / (2.0f * m_initScaledHeight);
}

/// @addr{0x8067F484}
void ObjectFlamePoleFoot::calcEruptingUp() {
    f32 frame = static_cast<f32>(m_cycleFrame - m_stateStart[1]);
    m_heightOffset = std::min(m_initScaledHeight,
            m_initEruptVel * frame - frame * 0.5f * m_eruptAccel * frame);
}

/// @addr{0x8067F544}
void ObjectFlamePoleFoot::calcEruptingStay() {
    constexpr f32 AMPLITUDE = 50.0f;

    f32 angle = 360.0f * static_cast<f32>(m_cycleFrame - m_stateStart[2]) / 30.0f;
    m_heightOffset = m_scaledHeight + AMPLITUDE * EGG::Mathf::SinFIdx(DEG2FIDX * angle);
}

u32 ObjectFlamePoleFoot::FLAMEPOLE_COUNT = 0;

} // namespace Field
