#include "ObjectEscalator.hh"

#include "game/field/CollisionDirector.hh"

#include "game/system/RaceManager.hh"
#include "game/system/map/MapdataGeoObj.hh"

#include <algorithm>

namespace Kinoko::Field {

/// @addr{0x807FFB20}
/// @brief Constructor
/// @param params The parameters used to initialize the object
/// @param reverse Whether the escalator should move in reverse
/// @details Initializes the escalator object with the given parameters and sets up its initial
/// state, including position, speed, and state durations.
ObjectEscalator::ObjectEscalator(const System::MapdataGeoObj &params, bool reverse /* = false */)
    : ObjectKCL(params),
      m_initialPos(pos()),
      m_stillFrames(
              {static_cast<s32>(params.setting(2)) * 60, static_cast<s32>(params.setting(4)) * 60}),
      m_speed(InitSpeed(params, reverse)),
      m_checkColYPosMax(m_initialPos.y + MAX_HEIGHT_OFFSET),
      m_checkColYPosMin(m_initialPos.y + MIN_HEIGHT_OFFSET),
      m_stopFrames({static_cast<f32>(m_stillFrames[0]) - REVERSE_FRAMES_F32,
              static_cast<f32>(m_stillFrames[1]) - REVERSE_FRAMES_F32}),
      m_startFrames({static_cast<f32>(m_stillFrames[0]) + STILL_FRAMES,
              static_cast<f32>(m_stillFrames[1]) + STILL_FRAMES}),
      m_fullSpeedFrames(
              {REVERSE_FRAMES_F32 + m_startFrames[0], REVERSE_FRAMES_F32 + m_startFrames[1]}),
      m_midDuration(m_stopFrames[1] - m_fullSpeedFrames[0]) {
    constexpr EGG::Vector3f STEP_DIMS = EGG::Vector3f(0.0f, STEP_HEIGHT, -30.0f);

    m_wrappedStepCount = 0.0f;

    EGG::Matrix34f mat;
    mat.makeR(rot());
    m_stepDims = mat.ps_multVector(STEP_DIMS);
}

/// @addr{0x80803D00}
/// @brief Default virtual destructor
ObjectEscalator::~ObjectEscalator() = default;

/// @addr{0x808008FC}
/// @copybrief ObjectBase::calc()
/// @details Calculates the escalator's moving object velocity, updates @ref m_wrappedStepCount
/// by calling @ref calcWrappedStepCount() with the current race framecount, and sets the
/// escalator's position accordingly.
void ObjectEscalator::calc() {
    s32 t = static_cast<s32>(System::RaceManager::Instance()->timer());
    setMovingObjVel(m_stepDims * calcSpeed(t));

    m_wrappedStepCount = calcWrappedStepCount(t);
    setPos(m_initialPos + m_stepDims * m_wrappedStepCount);
}

/// @addr{0x80803CF0}
/// @copybrief ObjectBase::id()
/// @return The ID of the escalator object, @ref ObjectId::Escalator.
ObjectId ObjectEscalator::id() const {
    return ObjectId::Escalator;
}

/// @addr{0x80803CF8}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectEscalator::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x80803910}
/// @copydoc ObjectDrivable::checkPointPartial()
bool ObjectEscalator::checkPointPartial(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfoPartial *info, KCLTypeMask *maskOut) {
    return checkPointImpl(&ObjColMgr::checkPointPartial, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x80803A04}
/// @copydoc ObjectDrivable::checkPointPartialPush()
bool ObjectEscalator::checkPointPartialPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfoPartial *info, KCLTypeMask *maskOut) {
    return checkPointImpl(&ObjColMgr::checkPointPartialPush, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x80803AF8}
/// @copydoc ObjectDrivable::checkPointFull()
bool ObjectEscalator::checkPointFull(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return checkPointImpl(&ObjColMgr::checkPointFull, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x80803BEC}
/// @copydoc ObjectDrivable::checkPointFullPush()
bool ObjectEscalator::checkPointFullPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return checkPointImpl(&ObjColMgr::checkPointFullPush, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x80803540}
/// @copydoc ObjectDrivable::checkSpherePartial()
bool ObjectEscalator::checkSpherePartial(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSphereImpl(&ObjColMgr::checkSpherePartial, radius, pos, prevPos, mask, info,
            maskOut, timeOffset);
}

/// @addr{0x80803680}
/// @copydoc ObjectDrivable::checkSpherePartialPush()
bool ObjectEscalator::checkSpherePartialPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSphereImpl(&ObjColMgr::checkSpherePartialPush, radius, pos, prevPos, mask, info,
            maskOut, timeOffset);
}

/// @addr{0x808037C0}
/// @copydoc ObjectDrivable::checkSphereFull()
bool ObjectEscalator::checkSphereFull(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkSphereImpl(&ObjColMgr::checkSphereFull, radius, pos, prevPos, mask, info, maskOut,
            timeOffset);
}

/// @addr{0x80803900}
/// @copydoc ObjectDrivable::checkSphereFullPush()
bool ObjectEscalator::checkSphereFullPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkCollision(radius, pos, prevPos, mask, info, maskOut, timeOffset);
}

/// @addr{0x80802D98}
/// @copybrief ObjectDrivable::narrScLocal()
/// @param radius The radius of the sphere to check
/// @param pos The position of the sphere to check
/// @param mask The KCL flags to check collision against
/// @param timeOffset Optional time offset (unused)
void ObjectEscalator::narrScLocal(f32 radius, const EGG::Vector3f &pos, KCLTypeMask mask,
        u32 /*timeOffset*/) {
    m_objColMgr->narrScLocal(radius, pos, mask);
}

/// @addr{0x80803170}
/// @copydoc ObjectDrivable::checkPointCachedPartial()
bool ObjectEscalator::checkPointCachedPartial(const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut) {
    return checkPointImpl(&ObjColMgr::checkPointCachedPartial, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x80803264}
/// @copydoc ObjectDrivable::checkPointCachedPartialPush()
bool ObjectEscalator::checkPointCachedPartialPush(const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut) {
    return checkPointImpl(&ObjColMgr::checkPointCachedPartialPush, pos, prevPos, mask, info,
            maskOut);
}

/// @addr{0x80803358}
/// @copydoc ObjectDrivable::checkPointCachedFull()
bool ObjectEscalator::checkPointCachedFull(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return checkPointImpl(&ObjColMgr::checkPointCachedFull, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x8080344C}
/// @copydoc ObjectDrivable::checkPointCachedFullPush()
bool ObjectEscalator::checkPointCachedFullPush(const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return checkPointImpl(&ObjColMgr::checkPointCachedFullPush, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x80802DA0}
/// @copydoc ObjectDrivable::checkSphereCachedPartial()
bool ObjectEscalator::checkSphereCachedPartial(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSphereImpl(&ObjColMgr::checkSphereCachedPartial, radius, pos, prevPos, mask, info,
            maskOut, timeOffset);
}

/// @addr{0x80802EE0}
/// @copydoc ObjectDrivable::checkSphereCachedPartialPush()
bool ObjectEscalator::checkSphereCachedPartialPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSphereImpl(&ObjColMgr::checkSphereCachedPartialPush, radius, pos, prevPos, mask,
            info, maskOut, timeOffset);
}

/// @addr{0x80803020}
/// @copydoc ObjectDrivable::checkSphereCachedFull()
bool ObjectEscalator::checkSphereCachedFull(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkSphereImpl(&ObjColMgr::checkSphereCachedFull, radius, pos, prevPos, mask, info,
            maskOut, timeOffset);
}

/// @addr{0x80803160}
/// @copydoc ObjectDrivable::checkSphereCachedFullPush()
bool ObjectEscalator::checkSphereCachedFullPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkCollisionCached(radius, pos, prevPos, mask, info, maskOut, timeOffset);
}

/// @addr{0x80800A10}
/// @copybrief ObjectKCL::getUpdatedMatrix()
/// @param timeOffset The time offset used to calculate the current frame's transformation
const EGG::Matrix34f &ObjectEscalator::getUpdatedMatrix(u32 timeOffset) {
    u32 t = System::RaceManager::Instance()->timer() - timeOffset;
    m_workMatrix.makeRT(rot(), m_initialPos + m_stepDims * calcWrappedStepCount(t));
    return m_workMatrix;
}

/// @addr{0x80803CE0}
/// @copybrief ObjectKCL::colRadiusAdditionalLength()
/// @return The additional length to be added to the escalator's collision radius, `1000.0f`.
f32 ObjectEscalator::colRadiusAdditionalLength() const {
    return 1000.0f;
}

/// @addr{0x808011CC}
/// @copydoc ObjectKCL::checkCollision()
/// @details Updates the escalator's @ref ObjColMgr transform. If the player is outside of the range
/// `[@ref m_checkColYPosMin, @ref m_checkColYPosMax]`, then early returns `false` and does not
/// perform any collision checks. Dispatches to the @ref ObjColMgr to check for collision between
/// the player and the escalator. If a collision occurred with #COL_TYPE_MOVING_ROAD, sets the
/// moving road velocity and distance to `info` and returns `true`, otherwise returns
/// `false`.
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
    const auto *entry =
            colDir->findClosestCollisionEntry(maskOut, KCL_TYPE_BIT(COL_TYPE_MOVING_ROAD));
    if (!entry) {
        return false;
    }

    if (entry->dist > info->movingFloorDist) {
        info->movingFloorDist = entry->dist;

        u32 t = System::RaceManager::Instance()->timer() - timeOffset;
        info->roadVelocity = m_stepDims * calcSpeed(t);
    }

    return true;
}

/// @addr{0x808014AC}
/// @copydoc ObjectKCL::checkCollisionCached()
/// @details Updates the escalator's transform. If the player is outside of the range `[@ref
/// m_checkColYPosMin, @ref m_checkColYPosMax]`, then early returns and does not perform any
/// collision checks. Dispatches to the @ref ObjColMgr to check for collision between the player and
/// the escalator using the cached prism data. If a collision occurred with #COL_TYPE_MOVING_ROAD,
/// sets the moving road velocity and distance to `info` and returns `true`, otherwise
/// returns `false`.
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
    const auto *entry =
            colDir->findClosestCollisionEntry(maskOut, KCL_TYPE_BIT(COL_TYPE_MOVING_ROAD));
    if (!entry) {
        return false;
    }

    if (entry->dist > info->movingFloorDist) {
        info->movingFloorDist = entry->dist;

        u32 t = System::RaceManager::Instance()->timer() - timeOffset;
        info->roadVelocity = m_stepDims * calcSpeed(t);
    }

    return true;
}

/// @brief Helper function which re-uses some shared code amongst collision check variants
/// @tparam T CollisionInfo or CollisionInfoPartial
/// @param checkFunc The function to use for the actual collision check
/// @param pos The point to check collision against
/// @param prevPos The previous position of the kart (used to compute collision depth)
/// @param mask The KCL masks to filter collision checks to (ignores other collision types)
/// @param info Out param that collision info is saved to (if any)
/// @param maskOut Type mask of the KCL the kart is colliding with (if any)
/// @return Whether or not a collision occurred
/// @details Early returns false if the kart's Y position is outside of the range `[@ref
/// m_checkColYPosMin, @ref m_checkColYPosMax]`.
template <typename T>
    requires std::is_same_v<T, CollisionInfo> || std::is_same_v<T, CollisionInfoPartial>
bool ObjectEscalator::checkPointImpl(CheckPointFunc<T> checkFunc, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, T *info, KCLTypeMask *maskOut) {
    if (m_checkColYPosMin > pos.y || pos.y >= m_checkColYPosMax) {
        return false;
    }

    return (m_objColMgr->*checkFunc)(pos, prevPos, mask, info, maskOut);
}

/// @brief Helper function which re-uses some shared code amongst collision check variants
/// @tparam T CollisionInfo or CollisionInfoPartial
/// @param checkFunc The function to use for the actual collision check
/// @param radius The radius of the sphere to check collision against
/// @param pos The position of the sphere to check collision against
/// @param prevPos The previous position of the kart (used to compute collision depth)
/// @param mask The KCL masks to filter collision checks to (ignores other collision types)
/// @param info Out param that collision info is saved to (if any)
/// @param maskOut Type mask of the KCL the kart is colliding with (if any)
/// @param timeOffset Optional time offset
/// @return Whether or not a collision occurred
/// @details Early returns false if the kart's Y position is outside of the range [@ref
/// m_checkColYPosMin, @ref m_checkColYPosMax]. Also updates the escalator's scale and the @ref
/// ObjColMgr transform before the collision check.
template <typename T>
    requires std::is_same_v<T, CollisionInfo> || std::is_same_v<T, CollisionInfoPartial>
bool ObjectEscalator::checkSphereImpl(CheckSphereFunc<T> checkFunc, f32 radius,
        const EGG::Vector3f &pos, const EGG::Vector3f &prevPos, KCLTypeMask mask, T *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    if (m_checkColYPosMin > pos.y || pos.y >= m_checkColYPosMax) {
        return false;
    }

    calcScale(timeOffset);
    update(timeOffset);

    return (m_objColMgr->*checkFunc)(radius, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x80800ABC}
/// @brief Evaluates a piecewise function to calculate a wrapped step count.
/// @param t The current frame used to evaluate the wrapped step count.
/// @return The wrapped step count of the escalator at the given frame `t`.
/// @details The escalator's geometry repeats every 20 steps / 200 units, so this function computes
/// a displacement modulo 200. This is the analytical integral of the piecewise-linear speed profile
/// from
/// @ref calcSpeed, height-scaled and wrapped to the span of a single step.
/// @par Piecewise Position Function
/// Let \f$v_0, v_1, v_2\f$ be @ref m_speed, let \f$t_{s0}, t_{s1}\f$ be @ref m_stopFrames, let
/// \f$t_{a0}, t_{a1}\f$ be @ref m_startFrames, let \f$t_{f0} = t_{a0} + R\f$ and
/// \f$t_{f1} = t_{a1} + R\f$ be @ref m_fullSpeedFrames, and let \f$R\f$ be REVERSE_FRAMES_F32
/// (240). Define the cumulative distances at each breakpoint:
/// \f[
/// C_1 = v_0 t_{s0}, \quad C_2 = C_1 + \frac{v_0 R}{2}, \quad C_3 = C_2 + \frac{v_1 R}{2}, \quad
/// C_4 = C_3 + v_1 (t_{s1} - t_{f0}), \quad C_5 = C_4 + \frac{v_1 R}{2}, \quad
/// C_6 = C_5 + \frac{v_2 R}{2} \, .
/// \f]
/// The escalator's total displacement \f$D(t)\f$ is then
/// \f[
/// D(t) =
/// \begin{cases}
///     v_0 t & 0 \le t < t_{s0}
///     \\ C_1 + \frac{v_0}{2}(t - t_{s0})\left(2 - \frac{t - t_{s0}}{R}\right)
///         & t_{s0} \le t < t_{s0} + R
///     \\ C_2 & t_{s0} + R \le t < t_{a0}
///     \\ C_2 + \frac{v_1 (t - t_{a0})^2}{2R} & t_{a0} \le t < t_{f0}
///     \\ C_3 + v_1 (t - t_{f0}) & t_{f0} \le t < t_{s1}
///     \\ C_4 + \frac{v_1}{2}(t - t_{s1})\left(2 - \frac{t - t_{s1}}{R}\right)
///         & t_{s1} \le t < t_{s1} + R
///     \\ C_5 & t_{s1} + R \le t < t_{a1}
///     \\ C_5 + \frac{v_2 (t - t_{a1})^2}{2R} & t_{a1} \le t < t_{f1}
///     \\ C_6 + v_2 (t - t_{f1}) & t \ge t_{f1}
/// \end{cases}
/// \f]
/// Finally, the result is wrapped to the span of a single step, where \f$H\f$ is
/// STEP_HEIGHT and \f$M = 200\f$ is the number of discrete step offsets:
/// \f[
/// \text{calcWrappedStepCount}(t) = \frac{\left(\left\lfloor H \cdot D(t) \right\rfloor \bmod M + M
/// \right) \bmod M}{H} \, .
/// \f]
f32 ObjectEscalator::calcWrappedStepCount(s32 t) {
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
/// @brief Calculates the speed of the escalator at a given time t
/// @param t The current frame used to evaluate the speed.
/// @return The speed of the escalator at the given frame `t`.
/// @details This is the derivative of the position function @ref calcWrappedStepCount() integrates.
/// @par Piecewise Speed Function
/// Using the same symbols as @ref calcWrappedStepCount(): \f$v_0, v_1, v_2\f$ are @ref m_speed,
/// \f$t_{s0}, t_{s1}\f$ are @ref m_stopFrames, \f$t_{a0}, t_{a1}\f$ are @ref m_startFrames,
/// \f$t_{f0} = t_{a0} + R\f$ and \f$t_{f1} = t_{a1} + R\f$ are @ref m_fullSpeedFrames, and
/// \f$R\f$ is REVERSE_FRAMES_F32.
/// \f[
/// v(t) =
/// \begin{cases}
///     v_0 & t < t_{s0}
///     \\ v_0 \frac{t_{s0} + R - t}{R} & t_{s0} \le t < t_{s0} + R
///     \\ 0 & t_{s0} + R \le t \le t_{a0}
///     \\ v_1 \frac{t - t_{a0}}{R} & t_{a0} < t < t_{f0}
///     \\ v_1 & t_{f0} \le t < t_{s1}
///     \\ v_1 \frac{t_{s1} + R - t}{R} & t_{s1} \le t < t_{s1} + R
///     \\ 0 & t_{s1} + R \le t \le t_{a1}
///     \\ v_2 \frac{t - t_{a1}}{R} & t_{a1} < t < t_{f1}
///     \\ v_2 & t \ge t_{f1}
/// \end{cases}
/// \f]
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

/// @brief Helper function to compute the speeds of the escalator at each stage of its movement
/// @param params The parameters used to initialize the object
/// @param reverse Whether the escalator should move in reverse
/// @return An array containing the speeds of the escalator at each stage of its movement
std::array<f32, 3> ObjectEscalator::InitSpeed(const System::MapdataGeoObj &params, bool reverse) {
    f32 sign = reverse ? -1.0f : 1.0f;
    f32 speed1 = sign * (0.15f * static_cast<f32>(static_cast<s16>(params.setting(1))) / 100.0f);
    f32 speed2 = sign * (0.15f * static_cast<f32>(static_cast<s16>(params.setting(3))) / 100.0f);
    f32 speed3 = sign * (0.15f * static_cast<f32>(static_cast<s16>(params.setting(5))) / 100.0f);

    return {speed1, speed2, speed3};
}

} // namespace Kinoko::Field
