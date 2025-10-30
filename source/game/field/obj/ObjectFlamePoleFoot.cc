#include "ObjectFlamePoleFoot.hh"

#include "game/field/CollisionDirector.hh"

#include "game/system/RaceManager.hh"

#include <algorithm>

namespace Field {

/// @addr{0x8067E6F4}
ObjectFlamePoleFoot::ObjectFlamePoleFoot(const System::MapdataGeoObj &params)
    : ObjectKCL(params), StateManager(this, STATE_ENTRIES) {
    m_extraCycleFrames = params.setting(0);
    m_initDelay = params.setting(1);
    m_poleScale = static_cast<f32>(params.setting(2));

    ++FLAMEPOLE_COUNT;

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
    constexpr f32 COMMON_EXPR_UNKNOWN = static_cast<f32>(CYCLE_FRAMES) * (7.0f - 1.0f);

    m_nextStateId = 0;
    m_cycleFrame = 0;
    m_state1Duration = static_cast<s32>(0.1f * COMMON_EXPR_UNKNOWN / 7.0f);
    m_state3Duration = static_cast<s32>(0.2f * COMMON_EXPR_UNKNOWN / 7.0f);
    s32 state1 = static_cast<s32>(0.3f * COMMON_EXPR_UNKNOWN / 7.0f);
    s32 state2 = state1 + m_state1Duration;
    s32 state3 = state2 + static_cast<s32>(static_cast<f32>(CYCLE_FRAMES) * 1.0f / 7.0f);
    s32 state4 = state3 + m_state3Duration;
    s32 state5 = state4 + static_cast<s32>(0.4f * COMMON_EXPR_UNKNOWN / 7.0f);
    m_stateStart = {{0, state1, state2, state3, state4, state5}};
    m_1a0 = (m_poleScale - 1.0f) / static_cast<f32>(state1);
    m_initScaledHeight = ObjectFlamePole::HEIGHT * m_poleScale;
    m_scaleDelta = (300.0f + m_initScaledHeight) / static_cast<f32>(m_state3Duration);
    m_pole->setActive(false);
    m_pole->disableCollision();

    EGG::Vector3f polePos = m_pole->pos();
    polePos.y = m_pos.y - ObjectFlamePole::HEIGHT * m_poleScale;
    m_pole->setPos(polePos);
}

/// @addr{0x8067EF70}
void ObjectFlamePoleFoot::calc() {
    if (System::RaceManager::Instance()->timer() < m_initDelay) {
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
    u32 frame = static_cast<s32>(System::RaceManager::Instance()->timer() - m_initDelay);
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

/// @addr{0x8067FBB8}
const EGG::Matrix34f &ObjectFlamePoleFoot::getUpdatedMatrix(u32 /*timeOffset*/) {
    calcTransform();
    m_workMatrix = m_transform;
    return m_transform;
}

f32 ObjectFlamePoleFoot::getScaleY(u32 timeOffset) const {
    u32 frame = System::RaceManager::Instance()->timer() - timeOffset;
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
                m_poleScale - m_1a0 * static_cast<f32>(m_state3Duration / 2) -
                        m_1a0 * static_cast<f32>(cycleFrame - m_stateStart[4]));
    }

    if (cycleFrame >= m_stateStart[3]) {
        s32 framesSince194 = cycleFrame - m_stateStart[3];
        s32 half17c = m_state3Duration / 2;
        if (framesSince194 > half17c) {
            return m_poleScale - m_1a0 * static_cast<f32>(framesSince194 - half17c);
        } else {
            return m_poleScale;
        }
    }

    if (cycleFrame >= m_stateStart[2] || cycleFrame >= m_stateStart[1]) {
        return m_poleScale;
    }

    if (cycleFrame >= 0) {
        return std::min(m_poleScale, m_1a0 * static_cast<f32>(cycleFrame) + 1.0f);
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

    f32 root0 = -1.0f;
    f32 root1 = -1.0f;
    f32 fVar1 = m_initScaledHeight;
    EGG::Mathf::FUN_800867C0(static_cast<f32>(m_state1Duration * m_state1Duration),
            -4.0f * m_initScaledHeight * static_cast<f32>(m_state1Duration),
            4.0f * m_initScaledHeight * m_initScaledHeight, root0, root1);

    if (root0 <= 0.0f) {
        root0 = -1.0f;
    }

    if (root1 <= 0.0f) {
        root1 = -1.0f;
        fVar1 = root0;
    }

    m_1c0 = fVar1;
    m_1bc = fVar1 * fVar1 / (2.0f * m_initScaledHeight);
}

/// @addr{0x8067F484}
void ObjectFlamePoleFoot::calcEruptingUp() {
    f32 frame = static_cast<f32>(m_cycleFrame - static_cast<u32>(m_stateStart[1]));
    m_heightOffset = std::min(m_initScaledHeight, m_1c0 * frame - frame * 0.5f * m_1bc * frame);
}

/// @addr{0x8067F544}
void ObjectFlamePoleFoot::calcEruptingStay() {
    constexpr f32 AMPLITUDE = 50.0f;

    f32 angle = 360.0f * static_cast<f32>(m_cycleFrame - m_stateStart[2]) / 30.0f;
    m_heightOffset = m_scaledHeight + AMPLITUDE * EGG::Mathf::SinFIdx(DEG2FIDX * angle);
}

u32 ObjectFlamePoleFoot::FLAMEPOLE_COUNT = 0;

} // namespace Field
