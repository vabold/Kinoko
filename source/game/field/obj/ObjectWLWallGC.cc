#include "ObjectWLWallGC.hh"

#include "game/field/ObjColMgr.hh"

#include "game/system/RaceManager.hh"
#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

/// @addr{0x8086BC1C}
/// @copybrief ObjectKCL::ObjectKCL(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details In Kinoko, we cache param setting 3 to @ref m_rate so that other members can be
/// derived in the initializer list and thus marked const. Sets @ref m_extendedDuration based on
/// param setting 2. @ref m_moveDuration is derived by dividing param setting 4 by @ref m_rate,
/// defaulting to `-1` if @ref m_rate is zero. @ref m_startFrame is set based on param setting 5.
/// @ref m_hiddenDuration is set based on param setting 1, defaulting to `-1` if @ref m_rate is
/// zero. @ref m_extendedFrame is derived by summing @ref m_hiddenDuration and @ref
/// m_moveDuration, unless @ref m_rate is zero, in which case it is defaulted to `-1`. @ref
/// m_retractingFrame is derived by summing @ref m_extendedFrame and @ref m_extendedDuration,
/// unless @ref m_rate is zero, in which case it is defaulted to `-1`. @ref m_cycleDuration is
/// derived by summing @ref m_retractingFrame and @ref m_moveDuration, unless @ref m_rate is
/// zero, in which case it is defaulted to `-1`. @ref m_initialPos is set to the object's initial
/// position. Finally, the object's transformation is updated and cached to @ref m_rtMat, and
/// the Piranha Plant's @ref m_extendedPos is set by extending @ref m_initialPos along the
/// object's negative local Z-axis by the distance specified in param setting 4.
ObjectWLWallGC::ObjectWLWallGC(const System::MapdataGeoObj &params)
    : ObjectKCL(params),
      m_rate(static_cast<u32>(params.setting(2))),
      m_extendedDuration(static_cast<s32>(params.setting(1))),
      m_moveDuration(m_rate == 0 ? -1 : params.setting(3) / m_rate),
      m_startFrame(static_cast<s32>(params.setting(4))),
      m_hiddenDuration(m_rate == 0 ? -1 : params.setting(0)),
      m_extendedFrame(m_rate == 0 ? -1 : m_hiddenDuration + m_moveDuration),
      m_retractingFrame(m_rate == 0 ? -1 : m_extendedFrame + m_extendedDuration),
      m_cycleDuration(m_rate == 0 ? -1 : m_retractingFrame + m_moveDuration),
      m_initialPos(pos()),
      m_extendedPos(initExtendedPos(params)) {
    m_rtMat = transform();
}

/// @addr{0x8086BDE4}
/// @brief Default virtual destructor
ObjectWLWallGC::~ObjectWLWallGC() = default;

/// @addr{0x8086BE34}
/// @copybrief ObjectBase::init()
/// @details Sets the Piranha Plant's position to @ref m_initialPos and updates its transform
/// and @ref m_rtMat accordingly.
void ObjectWLWallGC::init() {
    setPos(m_initialPos);
    calcTransform();
    m_rtMat = transform();
}

/// @addr{0x8086C108}
/// @copybrief ObjectBase::calc()
/// @details Updates the Piranha Plant's transform based on its current position within the
/// movement cycle. Sets moving object velocity based on the change in position between the
/// current frame and the previous frame.
void ObjectWLWallGC::calc() {
    EGG::Vector3f prevPos = pos();
    setTransform(getUpdatedMatrix(0));
    setMovingObjVel(pos() - prevPos);
}

/// @addr{0x8086C640}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame
ObjectBase::LoadFlags ObjectWLWallGC::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/** @addr{0x8086BF30}
 * @copybrief ObjectKCL::getUpdatedMatrix()
 * @param timeOffset The time offset used to calculate the current frame's transformation
 * @return A const ref to the updated transformation matrix
 * @details Linearly interpolates between the Piranha Plant's initial position and its extended
 * position based on the current frame within the movement cycle. Updates the translation of
 * @ref m_rtMat accordingly. The interpolation factor \f$t\f$ is computed as:
 * \f[t(time) = \begin{cases}
 *     0 & time < hiddenDuration \\
 *     \frac{time - hiddenDuration}{moveDuration} & hiddenDuration \le time < extendedFrame \\
 *     1 & extendedFrame \le time < retractingFrame \\
 *     1 - \frac{time - retractingFrame}{moveDuration} & \text{otherwise}
 * \end{cases}\f]
 **/
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

    m_rtMat.setBase(3, m_initialPos.interpolate(t, m_extendedPos));

    return m_rtMat;
}

/// @addr{0x8086C648}
/// @copybrief ObjectKCL::colRadiusAdditionalLength()
/// @return Returns the additional length to be added to the collision radius, which is the
/// distance between @ref m_initialPos and @ref m_extendedPos
f32 ObjectWLWallGC::colRadiusAdditionalLength() const {
    return (m_initialPos - m_extendedPos).length();
}

/// @addr{0x8086C328}
/// @copydoc ObjectKCL::checkCollision()
bool ObjectWLWallGC::checkCollision(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    update(timeOffset);
    calcScale(timeOffset);

    return m_objColMgr->checkSphereFullPush(radius, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x8086C5A8}
/// @copydoc ObjectKCL::checkCollisionCached()
bool ObjectWLWallGC::checkCollisionCached(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    update(timeOffset);
    calcScale(timeOffset);

    return m_objColMgr->checkSphereCachedFullPush(radius, pos, prevPos, mask, info, maskOut);
}

/// @brief Helper function to initialize @ref m_extendedPos so that it can be `const`
/// @param params The parameters used to initialize the object
/// @return The extended position of the Piranha Plant
EGG::Vector3f ObjectWLWallGC::initExtendedPos(const System::MapdataGeoObj &params) {
    calcTransform();
    return m_initialPos - transform().base(2) * static_cast<f32>(params.setting(3));
}

/// @addr{0x8086BF08}
/// @brief Simple modulo to compute the current frame within the object's movement cycle offset by
/// @ref m_startFrame, clamped to `0` if @ref m_startFrame frames have not elapsed yet
/// @param t The current framecount of the race
/// @return Returns the current frame within the object's movement cycle
u32 ObjectWLWallGC::cycleFrame(s32 t) const {
    u32 time = t < m_startFrame ? 0 : t - m_startFrame;
    return time % static_cast<u32>(m_cycleDuration);
}

} // namespace Kinoko::Field
