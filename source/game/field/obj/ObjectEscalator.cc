#include "ObjectEscalator.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/ObjectDirector.hh"

#include "game/system/RaceManager.hh"

#include <algorithm>

namespace Field {

/// @addr{0x807FFB20}
ObjectEscalator::ObjectEscalator(const System::MapdataGeoObj &params, bool reverse /* = false */)
    : ObjectKCL(params), m_initialPos(m_pos), m_initialRot(m_rot),
      m_stillFrames(
              {static_cast<s32>(params.setting(2)) * 60, static_cast<s32>(params.setting(4)) * 60}),
      m_speed({(reverse ? -1.0f : 1.0f) * (0.15f * static_cast<f32>(params.setting(1)) / 100.0f),
              (reverse ? -1.0f : 1.0f) * (0.15f * static_cast<f32>(params.setting(3)) / 100.0f),
              (reverse ? -1.0f : 1.0f) * (0.15f * static_cast<f32>(params.setting(5)) / 100.0f)}),
      m_checkColYPosMax(m_initialPos.y + MAX_HEIGHT_OFFSET),
      m_checkColYPosMin(m_initialPos.y + MIN_HEIGHT_OFFSET),
      m_stopFrames({static_cast<f32>(m_stillFrames[0]) - REVERSE_FRAMES_F32,
              static_cast<f32>(m_stillFrames[1]) - REVERSE_FRAMES_F32}),
      m_startFrames({static_cast<f32>(m_stillFrames[0]) + STANDSTILL_FRAMES,
              static_cast<f32>(m_stillFrames[1]) + STANDSTILL_FRAMES}),
      m_fullSpeedFrames(
              {REVERSE_FRAMES_F32 + m_startFrames[0], REVERSE_FRAMES_F32 + m_startFrames[1]}),
      m_midDuration(m_stopFrames[1] - m_fullSpeedFrames[0]) {
    constexpr EGG::Vector3f STEP_DIMS = EGG::Vector3f(0.0f, STEP_HEIGHT, -30.0f);

    m_stepFactor = 0.0f;

    EGG::Matrix34f mat;
    mat.makeR(m_initialRot);
    m_stepDims = mat.ps_multVector(STEP_DIMS);
}

/// @addr{0x80803D00}
ObjectEscalator::~ObjectEscalator() = default;

/// @addr{0x808008FC}
void ObjectEscalator::calc() {
    s32 t = static_cast<s32>(System::RaceManager::Instance()->timer());
    setMovingObjVel(m_stepDims * calcSpeed(t));

    m_stepFactor = calcStepFactor(t);
    m_pos = m_initialPos + m_stepDims * m_stepFactor;
    m_flags.setBit(eFlags::Position);
}

/// @addr{0x80803910}
bool ObjectEscalator::checkPointPartial(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfoPartial *info, KCLTypeMask *maskOut) {
    return checkPointImpl(&ObjectEscalator::shouldCheckColNoPush, &ObjColMgr::checkPointPartial,
            pos, prevPos, mask, info, maskOut);
}

/// @addr{0x80803A04}
bool ObjectEscalator::checkPointPartialPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfoPartial *info, KCLTypeMask *maskOut) {
    return checkPointImpl(&ObjectEscalator::shouldCheckColPush, &ObjColMgr::checkPointPartialPush,
            pos, prevPos, mask, info, maskOut);
}

/// @addr{0x80803AF8}
bool ObjectEscalator::checkPointFull(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return checkPointImpl(&ObjectEscalator::shouldCheckColNoPush, &ObjColMgr::checkPointFull, pos,
            prevPos, mask, info, maskOut);
}

/// @addr{0x80803BEC}
bool ObjectEscalator::checkPointFullPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return checkPointImpl(&ObjectEscalator::shouldCheckColPush, &ObjColMgr::checkPointFullPush, pos,
            prevPos, mask, info, maskOut);
}

/// @addr{0x80803540}
bool ObjectEscalator::checkSpherePartial(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSphereImpl(&ObjectEscalator::shouldCheckColNoPush, &ObjColMgr::checkSpherePartial,
            radius, pos, prevPos, mask, info, maskOut, timeOffset);
}

/// @addr{0x80803680}
bool ObjectEscalator::checkSpherePartialPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSphereImpl(&ObjectEscalator::shouldCheckColPush, &ObjColMgr::checkSpherePartialPush,
            radius, pos, prevPos, mask, info, maskOut, timeOffset);
}

/// @addr{0x808037C0}
bool ObjectEscalator::checkSphereFull(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkSphereImpl(&ObjectEscalator::shouldCheckColNoPush, &ObjColMgr::checkSphereFull,
            radius, pos, prevPos, mask, info, maskOut, timeOffset);
}

/// @addr{0x80803900}
bool ObjectEscalator::checkSphereFullPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkCollision(radius, pos, prevPos, mask, info, maskOut, timeOffset);
}

/// @addr{0x80802D98}
void ObjectEscalator::narrScLocal(f32 radius, const EGG::Vector3f &pos, KCLTypeMask mask,
        u32 /*timeOffset*/) {
    m_objColMgr->narrScLocal(radius, pos, mask);
}

/// @addr{0x80803170}
bool ObjectEscalator::checkPointCachedPartial(const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut) {
    return checkPointImpl(&ObjectEscalator::shouldCheckColNoPush,
            &ObjColMgr::checkPointCachedPartial, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x80803264}
bool ObjectEscalator::checkPointCachedPartialPush(const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut) {
    return checkPointImpl(&ObjectEscalator::shouldCheckColPush,
            &ObjColMgr::checkPointCachedPartialPush, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x80803358}
bool ObjectEscalator::checkPointCachedFull(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return checkPointImpl(&ObjectEscalator::shouldCheckColNoPush, &ObjColMgr::checkPointCachedFull,
            pos, prevPos, mask, info, maskOut);
}

/// @addr{0x8080344C}
bool ObjectEscalator::checkPointCachedFullPush(const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return checkPointImpl(&ObjectEscalator::shouldCheckColPush,
            &ObjColMgr::checkPointCachedFullPush, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x80802DA0}
bool ObjectEscalator::checkSphereCachedPartial(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSphereImpl(&ObjectEscalator::shouldCheckColNoPush,
            &ObjColMgr::checkSphereCachedPartial, radius, pos, prevPos, mask, info, maskOut,
            timeOffset);
}

/// @addr{0x80802EE0}
bool ObjectEscalator::checkSphereCachedPartialPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSphereImpl(&ObjectEscalator::shouldCheckColPush,
            &ObjColMgr::checkSphereCachedPartialPush, radius, pos, prevPos, mask, info, maskOut,
            timeOffset);
}

/// @addr{0x80803020}
bool ObjectEscalator::checkSphereCachedFull(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkSphereImpl(&ObjectEscalator::shouldCheckColNoPush,
            &ObjColMgr::checkSphereCachedFull, radius, pos, prevPos, mask, info, maskOut,
            timeOffset);
}

/// @addr{0x80803160}
bool ObjectEscalator::checkSphereCachedFullPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkCollisionCached(radius, pos, prevPos, mask, info, maskOut, timeOffset);
}

/// @addr{0x80800A10}
const EGG::Matrix34f &ObjectEscalator::getUpdatedMatrix(u32 timeOffset) {
    u32 t = System::RaceManager::Instance()->timer() - timeOffset;
    m_workMatrix.makeRT(m_rot, m_initialPos + m_stepDims * calcStepFactor(t));
    return m_workMatrix;
}

/// @addr{0x808011CC}
bool ObjectEscalator::checkCollision(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    update(timeOffset);

    if (m_checkColYPosMin >= pos.y || pos.y >= m_checkColYPosMax) {
        return false;
    }

    if (!m_objColMgr->checkSphereFullPush(radius, pos, prevPos, mask, info, maskOut)) {
        return false;
    }

    if ((*maskOut & KCL_TYPE_BIT(COL_TYPE_MOVING_ROAD)) == 0) {
        return false;
    }

    auto *colDir = CollisionDirector::Instance();
    if (!colDir->findClosestCollisionEntry(maskOut, KCL_TYPE_BIT(COL_TYPE_MOVING_ROAD))) {
        return false;
    }

    auto *entry = colDir->closestCollisionEntry();
    if (entry->dist > info->movingFloorDist) {
        info->movingFloorDist = entry->dist;

        u32 t = System::RaceManager::Instance()->timer() - timeOffset;
        info->roadVelocity = m_stepDims * calcSpeed(t);
    }

    return true;
}

/// @addr{0x808014AC}
bool ObjectEscalator::checkCollisionCached(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    update(timeOffset);

    if (m_checkColYPosMin >= pos.y || pos.y >= m_checkColYPosMax) {
        return false;
    }

    if (!m_objColMgr->checkSphereCachedFullPush(radius, pos, prevPos, mask, info, maskOut)) {
        return false;
    }

    if ((*maskOut & KCL_TYPE_BIT(COL_TYPE_MOVING_ROAD)) == 0) {
        return false;
    }

    auto *colDir = CollisionDirector::Instance();
    if (!colDir->findClosestCollisionEntry(maskOut, KCL_TYPE_BIT(COL_TYPE_MOVING_ROAD))) {
        return false;
    }

    auto *entry = colDir->closestCollisionEntry();
    if (entry->dist > info->movingFloorDist) {
        info->movingFloorDist = entry->dist;

        u32 t = System::RaceManager::Instance()->timer() - timeOffset;
        info->roadVelocity = m_stepDims * calcSpeed(t);
    }

    return true;
}

template <typename T>
    requires std::is_same_v<T, CollisionInfo> || std::is_same_v<T, CollisionInfoPartial>
bool ObjectEscalator::checkPointImpl(ShouldCheckFunc shouldCheckFunc, CheckPointFunc<T> checkFunc,
        const EGG::Vector3f &pos, const EGG::Vector3f &prevPos, KCLTypeMask mask, T *info,
        KCLTypeMask *maskOut) {
    if (m_checkColYPosMin > pos.y || pos.y >= m_checkColYPosMax) {
        return false;
    }

    if (!(this->*shouldCheckFunc)()) {
        return false;
    }

    return (m_objColMgr->*checkFunc)(pos, prevPos, mask, info, maskOut);
}

template <typename T>
    requires std::is_same_v<T, CollisionInfo> || std::is_same_v<T, CollisionInfoPartial>
bool ObjectEscalator::checkSphereImpl(ShouldCheckFunc shouldCheckFunc, CheckSphereFunc<T> checkFunc,
        f32 radius, const EGG::Vector3f &pos, const EGG::Vector3f &prevPos, KCLTypeMask mask,
        T *info, KCLTypeMask *maskOut, u32 timeOffset) {
    if (m_checkColYPosMin > pos.y || pos.y >= m_checkColYPosMax) {
        return false;
    }

    if (!(this->*shouldCheckFunc)()) {
        return false;
    }

    calcScale(timeOffset);
    update(timeOffset);

    return (m_objColMgr->*checkFunc)(radius, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x80800ABC}
f32 ObjectEscalator::calcStepFactor(s32 t) {
    constexpr s32 REVERSE_FRAMES_S32 = static_cast<s32>(REVERSE_FRAMES_F32);
    constexpr s32 DISCRETE_STEP_OFFSETS = 200;

    // Time since escalator began stopping
    std::array<s32, 2> dtStop = {static_cast<s32>(static_cast<f32>(t) - m_stopFrames[0]),
            static_cast<s32>(static_cast<f32>(t) - m_stopFrames[1])};

    // Time since escalator started moving (second and third time)
    std::array<s32, 2> dtStart = {static_cast<s32>(static_cast<f32>(t) - m_startFrames[0]),
            static_cast<s32>(static_cast<f32>(t) - m_startFrames[1])};

    // Time since escalator started moving at full speed
    std::array<s32, 2> dtFullSpeed = {static_cast<s32>(static_cast<f32>(t) - m_fullSpeedFrames[0]),
            static_cast<s32>(static_cast<f32>(t) - m_fullSpeedFrames[1])};

    // Each dist lambda represents the displacement from each sub-function of the piecewise function

    // Escalator has not stopped yet
    auto dist0 = [this](s32 t) -> f64 {
        t = std::clamp<s32>(t, 0, static_cast<s32>(m_stopFrames[0]));
        return static_cast<f64>(static_cast<f32>(t) * m_speed[0]);
    };

    // Escalator is slowing down for the first time
    auto dist1 = [=, this]() -> f64 {
        f64 speed = static_cast<f64>(m_speed[0]);
        f64 t = static_cast<f64>(std::clamp<s32>(dtStop[0], 0, REVERSE_FRAMES_S32));
        return 0.5 * t * (speed + speed * (1.0 - t / static_cast<f64>(REVERSE_FRAMES_F32)));
    };

    // Escalator is speeding up after switching directions once
    auto dist2 = [=, this]() -> f64 {
        f64 t = static_cast<f64>(std::clamp<s32>(dtStart[0], 0, REVERSE_FRAMES_S32));
        return t * (0.5 * t * static_cast<f64>(m_speed[1])) / static_cast<f64>(REVERSE_FRAMES_F32);
    };

    // Escalator is moving full speed after switching directions once
    auto dist3 = [=, this](s32 t) -> f64 {
        t = std::clamp<s32>(dtFullSpeed[0], 0, m_midDuration);
        return static_cast<f64>(static_cast<f32>(t) * m_speed[1]);
    };

    // Escalator is slowing down for the second time
    auto dist4 = [=, this]() -> f64 {
        f64 speed = static_cast<f64>(m_speed[1]);
        f64 t = static_cast<f32>(std::clamp<s32>(dtStop[1], 0, REVERSE_FRAMES_S32));
        return 0.5 * t * (speed + speed * (1.0 - t / static_cast<f64>(REVERSE_FRAMES_F32)));
    };

    // Escalator is speeding up after switching directions twice
    auto dist5 = [=, this]() -> f64 {
        f64 speed = static_cast<f64>(m_speed[2]);
        f64 t = static_cast<f64>(std::clamp<s32>(dtStart[1], 0, REVERSE_FRAMES_S32));
        return (t * (0.5 * t * speed) / static_cast<f64>(REVERSE_FRAMES_F32));
    };

    // Escalator is moving full speed after switching directions twice
    auto dist6 = [=, this]() -> f64 {
        f64 t = static_cast<f64>(std::max(dtFullSpeed[1], 0));
        return t * static_cast<f64>(m_speed[2]);
    };

    f64 totalDist = dist6() + dist5() + dist4() + dist3(t) + dist2() + dist0(t) + dist1();
    s32 result = static_cast<s32>(STEP_HEIGHT * static_cast<f32>(totalDist));
    f32 fin = static_cast<f32>(result % DISCRETE_STEP_OFFSETS);

    if (fin < 0.0f) {
        fin += static_cast<f32>(DISCRETE_STEP_OFFSETS);
    }

    return fin / STEP_HEIGHT;
}

/// @addr{0x80800FBC}
f32 ObjectEscalator::calcSpeed(s32 t) {
    // Escalator has not stopped yet
    if (static_cast<f32>(t) < m_stopFrames[0]) {
        return m_speed[0];
    }

    // Escalator is slowing down for the first time
    if (t < m_stillFrames[0]) {
        return m_speed[0] * static_cast<f32>(m_stillFrames[0] - t) / REVERSE_FRAMES_F32;
    }

    // Standstill right after stopping for the first time
    if (static_cast<f32>(t) <= m_startFrames[0]) {
        return 0.0f;
    }

    // Escalator is speeding up after switching directions once
    if (static_cast<f32>(t) < REVERSE_FRAMES_F32 + m_startFrames[0]) {
        return m_speed[1] * (static_cast<f32>(t) - m_startFrames[0]) / REVERSE_FRAMES_F32;
    }

    // Escalator is moving full speed after switching directions once
    if (static_cast<f32>(t) < m_stopFrames[1]) {
        return m_speed[1];
    }

    // Escalator is slowing down for the second time
    if (t < m_stillFrames[1]) {
        return m_speed[1] * static_cast<f32>(m_stillFrames[1] - t) / REVERSE_FRAMES_F32;
    }

    // Standstill right after stopping for the second/final time
    if (static_cast<f32>(t) <= m_startFrames[1]) {
        return 0.0f;
    }

    // Escalator is speeding up after switching directions twice
    if (static_cast<f32>(t) < REVERSE_FRAMES_F32 + m_startFrames[1]) {
        return m_speed[2] * (static_cast<f32>(t) - m_startFrames[1]) / REVERSE_FRAMES_F32;
    }

    // Escalator is moving full speed after switching directions twice
    return m_speed[2];
}

} // namespace Field
