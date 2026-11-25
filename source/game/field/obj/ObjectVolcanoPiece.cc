#include "ObjectVolcanoPiece.hh"

#include "game/field/ObjColMgr.hh"

#include "game/system/RaceManager.hh"
#include "game/system/ResourceManager.hh"
#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

/// @addr{0x80817DE8}
/// @copybrief ObjectKCL::ObjectKCL(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Sets @ref m_initialPos based on the object's initial position. Sets @ref m_restEndFrame
/// and @ref m_shakeEndFrame based on the number of seconds defined by param setting 2 and 3
/// respectively. Sets @ref m_quakeEndFrame based on the number of frames defined by param setting 8
/// plus one. Initializes @ref m_colMgrB and @ref m_colMgrC to `nullptr`. Sets @ref m_modelName
/// based on param setting 1. Finally, Kinoko computes the rotation of @ref m_rtMat, which never
/// changes.
ObjectVolcanoPiece::ObjectVolcanoPiece(const System::MapdataGeoObj &params)
    : ObjectKCL(params),
      m_initialPos(pos()),
      m_restEndFrame(params.setting(1) * 60),
      m_shakeEndFrame(m_restEndFrame + params.setting(2) * 60),
      m_quakeEndFrame(m_shakeEndFrame + params.setting(7) + 1),
      m_colMgrB(nullptr),
      m_colMgrC(nullptr) {
    snprintf(m_modelName, sizeof(m_modelName), "VolcanoPiece%hd",
            static_cast<s16>(params.setting(0)));

    // The object's rotation never changes, so compute m_rtMat up front
    m_rtMat.makeRT(rot(), m_initialPos);
}

/// @addr{0x80803DA8}
/// @brief Virtual destructor that deletes the secondary and tertiary collision managers for the
/// volcano piece, if they exist
ObjectVolcanoPiece::~ObjectVolcanoPiece() {
    EGG::egg_delete(m_colMgrB);
    EGG::egg_delete(m_colMgrC);
}

/// @addr{0x80819400}
/// @copybrief ObjectBase::calc()
/// @details If the volcano piece is at the last frame of falling, then calls @ref update() to set
/// the @ref ObjColMgr transformation matrices so they reflect the position of the fallen volcano
/// piece after it transitions to @ref State::Gone. Regardless, sets the object's transform based on
/// @ref calcShakeAndFall() and sets the moving object velocity accordingly.
void ObjectVolcanoPiece::calc() {
    u32 timer = System::RaceManager::Instance()->timer();
    if (calcState(timer) == State::Fall && FALL_DURATION - 1 == calcT(timer)) {
        update(0);
    }

    EGG::Vector3f movingObjVel;
    setTransform(calcShakeAndFall(&movingObjVel, 0));
    setMovingObjVel(movingObjVel);
}

/// @addr{0x80817F6C}
/// @copybrief ObjectBase::createCollision()
/// @details Creates the primary, secondary, and tertiary @ref ObjColMgr collision managers for the
/// volcano piece, if the corresponding KCL files exist. Updates the transformation matrices and
/// scale for each @ref ObjColMgr.
void ObjectVolcanoPiece::createCollision() {
    ObjectKCL::createCollision();

    char filepath[128];
    snprintf(filepath, sizeof(filepath), "%sb.kcl", getKclName());
    auto *file = System::ResourceManager::Instance()->getFile(filepath, nullptr,
            System::ArchiveId::Course);

    if (file) {
        m_colMgrB = EGG::egg_new<ObjColMgr>(file);
    }

    snprintf(filepath, sizeof(filepath), "%sc.kcl", getKclName());
    file = System::ResourceManager::Instance()->getFile(filepath, nullptr,
            System::ArchiveId::Course);

    if (file) {
        m_colMgrC = EGG::egg_new<ObjColMgr>(file);
    }

    EGG::Matrix34f rtMat;
    EGG::Matrix34f invMat;
    rtMat.makeRT(rot(), m_initialPos);
    rtMat.ps_inverse(invMat);

    m_objColMgr->setMtx(rtMat);
    m_objColMgr->setInvMtx(invMat);
    m_objColMgr->setScale(scale().y);

    if (m_colMgrB) {
        m_colMgrB->setMtx(rtMat);
        m_colMgrB->setInvMtx(invMat);
        m_colMgrB->setScale(scale().y);
    }

    if (m_colMgrC) {
        m_colMgrC->setMtx(rtMat);
        m_colMgrC->setInvMtx(invMat);
        m_colMgrC->setScale(scale().y);
    }
}

/// @addr{0x80805974}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame
ObjectBase::LoadFlags ObjectVolcanoPiece::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x80803F6C}
/// @copybrief ObjectBase::getKclName()
/// @return The model name of the volcano piece
const char *ObjectVolcanoPiece::getKclName() const {
    return m_modelName;
}

/// @brief Helper function for frequently re-used portion of point collision checks
/// @tparam T The type of collision info, either @ref CollisionInfo or @ref CollisionInfoPartial
/// @tparam U The type of check function, either @ref ObjectVolcanoPiece::CheckPointPartialFunc or
/// @ref ObjectVolcanoPiece::CheckPointFullFunc
/// @param pos The current position to check for collision
/// @param prevPos The previous position to check for collision
/// @param mask The KCL type mask to use for the collision check
/// @param info Pointer to the collision info structure to populate
/// @param maskOut Pointer to the KCL type mask that will be updated based on the collision check
/// @param checkFunc The member function pointer to the specific collision check function to use
/// @return `true` if a collision was detected, `false` otherwise
/// @details If the volcano piece is in the @ref State::Rest state, only the primary collision
/// manager is checked. Otherwise, the secondary and tertiary collision managers are also checked
/// for collisions.
template <typename T, typename U>
    requires(std::is_same_v<T, CollisionInfo> || std::is_same_v<T, CollisionInfoPartial>) &&
        (std::is_same_v<U, ObjectVolcanoPiece::CheckPointPartialFunc> ||
                std::is_same_v<U, ObjectVolcanoPiece::CheckPointFullFunc>)
bool ObjectVolcanoPiece::checkPointImpl(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, T *info, KCLTypeMask *maskOut, U checkFunc) {
    State state = calcState(System::RaceManager::Instance()->timer());
    bool hasCol = (m_objColMgr->*checkFunc)(pos, prevPos, mask, info, maskOut);

    if (state == State::Rest || hasCol) {
        return hasCol;
    }

    hasCol = m_colMgrB && (m_colMgrB->*checkFunc)(pos, prevPos, mask, info, maskOut);
    hasCol = hasCol || (m_colMgrC && (m_colMgrC->*checkFunc)(pos, prevPos, mask, info, maskOut));

    return hasCol;
}

/// @brief Helper function for frequently re-used portion of sphere collision checks
/// @tparam T The type of collision info, either @ref CollisionInfo or @ref CollisionInfoPartial
/// @tparam U The type of check function, either @ref ObjectVolcanoPiece::CheckSpherePartialFunc or
/// @ref ObjectVolcanoPiece::CheckSphereFullFunc
/// @param radius The radius of the sphere to check for collision
/// @param pos The current position of the sphere to check for collision
/// @param prevPos The previous position of the sphere to check for collision
/// @param mask The KCL type mask to use for the collision check
/// @param info Pointer to the collision info structure to populate
/// @param maskOut Pointer to the KCL type mask that will be updated based on the collision check
/// @param timeOffset The time offset to use when updating the volcano piece's state
/// @param checkFunc The member function pointer to the specific collision check function to use
/// @return `true` if a collision was detected, `false` otherwise
/// @details Calls @ref update() so that all of the @ref ObjColMgr objects reflect the current
/// transform of the volcano piece. If the volcano piece is in the @ref State::Rest state, only the
/// primary collision manager is checked. Otherwise, the secondary and tertiary collision managers
/// are also checked for collisions.
template <typename T, typename U>
    requires(std::is_same_v<T, CollisionInfo> || std::is_same_v<T, CollisionInfoPartial>) &&
        (std::is_same_v<U, ObjectVolcanoPiece::CheckSpherePartialFunc> ||
                std::is_same_v<U, ObjectVolcanoPiece::CheckSphereFullFunc>)
bool ObjectVolcanoPiece::checkSphereImpl(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, T *info, KCLTypeMask *maskOut,
        u32 timeOffset, U checkFunc) {
    update(timeOffset);
    State state = calcState(System::RaceManager::Instance()->timer());
    bool hasCol = (m_objColMgr->*checkFunc)(radius, pos, prevPos, mask, info, maskOut);

    if (state == State::Rest || hasCol) {
        return hasCol;
    }

    hasCol = m_colMgrB && (m_colMgrB->*checkFunc)(radius, pos, prevPos, mask, info, maskOut);
    hasCol = hasCol ||
            (m_colMgrC && (m_colMgrC->*checkFunc)(radius, pos, prevPos, mask, info, maskOut));

    return hasCol;
}

/// @brief Helper function for frequently re-used portion of KCL collision checks
/// @param radius The radius of the sphere to check for collision
/// @param pos The current position of the sphere to check for collision
/// @param prevPos The previous position of the sphere to check for collision
/// @param mask The KCL type mask to use for the collision check
/// @param info Pointer to the collision info structure to populate
/// @param maskOut Pointer to the KCL type mask that will be updated based on the collision check
/// @param timeOffset The time offset to use when updating the volcano piece's state
/// @param checkFunc The member function pointer to the specific collision check function to use
/// @return `true` if a collision was detected, `false` otherwise
/// @details Calls @ref update() so that all of the @ref ObjColMgr objects reflect the current
/// transform of the volcano piece. If the volcano piece is in the @ref State::Rest state, only the
/// primary collision manager is checked. If the volcano piece is in the @ref State::Gone state,
/// only the tertiary collision manager is checked. Otherwise, all three collision managers are
/// checked.
bool ObjectVolcanoPiece::checkCollisionImpl(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset, CheckSphereFullFunc checkFunc) {
    u32 t = System::RaceManager::Instance()->timer() - timeOffset;
    State state = calcState(t);
    update(timeOffset);

    if (state == State::Rest) {
        return (m_objColMgr->*checkFunc)(radius, pos, prevPos, mask, info, maskOut);
    }

    if (state == State::Gone) {
        return m_colMgrC && (m_colMgrC->*checkFunc)(radius, pos, prevPos, mask, info, maskOut);
    }

    bool hasCol = (m_objColMgr->*checkFunc)(radius, pos, prevPos, mask, info, maskOut);
    hasCol = hasCol ||
            (m_colMgrB && (m_colMgrB->*checkFunc)(radius, pos, prevPos, mask, info, maskOut));
    hasCol = hasCol ||
            (m_colMgrC && (m_colMgrC->*checkFunc)(radius, pos, prevPos, mask, info, maskOut));

    return hasCol;
}

/// @addr{0x80805404}
/// @copydoc ObjectDrivable::checkPointPartial()
bool ObjectVolcanoPiece::checkPointPartial(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfoPartial *info, KCLTypeMask *maskOut) {
    return checkPointImpl(pos, prevPos, mask, info, maskOut, &ObjColMgr::checkPointPartial);
}

/// @addr{0x8080554C}
/// @copydoc ObjectDrivable::checkPointPartialPush()
bool ObjectVolcanoPiece::checkPointPartialPush(const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut) {
    return checkPointImpl(pos, prevPos, mask, info, maskOut, &ObjColMgr::checkPointPartialPush);
}

/// @addr{0x80805694}
/// @copydoc ObjectDrivable::checkPointFull()
bool ObjectVolcanoPiece::checkPointFull(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return checkPointImpl(pos, prevPos, mask, info, maskOut, &ObjColMgr::checkPointFull);
}

/// @addr{0x808057DC}
/// @copydoc ObjectDrivable::checkPointFullPush()
bool ObjectVolcanoPiece::checkPointFullPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return checkPointImpl(pos, prevPos, mask, info, maskOut, &ObjColMgr::checkPointFullPush);
}

/// @addr{0x80804F68}
/// @copydoc ObjectDrivable::checkSpherePartial()
bool ObjectVolcanoPiece::checkSpherePartial(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSphereImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset,
            &ObjColMgr::checkSpherePartial);
}

/// @addr{0x808050EC}
/// @copydoc ObjectDrivable::checkSpherePartialPush()
bool ObjectVolcanoPiece::checkSpherePartialPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSphereImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset,
            &ObjColMgr::checkSpherePartialPush);
}

/// @addr{0x80805270}
/// @copydoc ObjectDrivable::checkSphereFull()
bool ObjectVolcanoPiece::checkSphereFull(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkSphereImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset,
            &ObjColMgr::checkSphereFull);
}

/// @addr{0x808053F4}
/// @copydoc ObjectDrivable::checkSphereFullPush()
bool ObjectVolcanoPiece::checkSphereFullPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkCollision(radius, pos, prevPos, mask, info, maskOut, timeOffset);
}

/// @addr{0x808044C0}
/// @copybrief ObjectDrivable::narrScLocal()
/// @param radius The radius of the sphere to check
/// @param pos The position of the sphere to check
/// @param mask The KCL flags to check collision against (other types are ignored)
/// @param timeOffset The time offset to use when updating the volcano piece's state (unused)
/// @details If the volcano piece is in the @ref State::Rest state, only the primary @ref ObjColMgr
/// has its spatial cache narrowed down. Otherwise, all three collision managers have their spatial
/// caches narrowed down.
void ObjectVolcanoPiece::narrScLocal(f32 radius, const EGG::Vector3f &pos, KCLTypeMask mask,
        u32 /*timeOffset*/) {
    State state = calcState(System::RaceManager::Instance()->timer());
    m_objColMgr->narrScLocal(radius, pos, mask);

    if (state == State::Rest) {
        return;
    }

    if (m_colMgrB) {
        m_colMgrB->narrScLocal(radius, pos, mask);
    }

    if (m_colMgrC) {
        m_colMgrC->narrScLocal(radius, pos, mask);
    }
}

/// @addr{0x80804A48}
/// @copydoc ObjectDrivable::checkPointCachedPartial()
bool ObjectVolcanoPiece::checkPointCachedPartial(const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut) {
    return checkPointImpl(pos, prevPos, mask, info, maskOut, &ObjColMgr::checkPointCachedPartial);
}

/// @addr{0x80804B90}
/// @copydoc ObjectDrivable::checkPointCachedPartialPush()
bool ObjectVolcanoPiece::checkPointCachedPartialPush(const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut) {
    return checkPointImpl(pos, prevPos, mask, info, maskOut,
            &ObjColMgr::checkPointCachedPartialPush);
}

/// @addr{0x80804CD8}
/// @copydoc ObjectDrivable::checkPointCachedFull()
bool ObjectVolcanoPiece::checkPointCachedFull(const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return checkPointImpl(pos, prevPos, mask, info, maskOut, &ObjColMgr::checkPointCachedFull);
}

/// @addr{0x80804E20}
/// @copydoc ObjectDrivable::checkPointCachedFullPush()
bool ObjectVolcanoPiece::checkPointCachedFullPush(const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return checkPointImpl(pos, prevPos, mask, info, maskOut, &ObjColMgr::checkPointCachedFullPush);
}

/// @addr{0x808045AC}
/// @copydoc ObjectDrivable::checkSphereCachedPartial()
bool ObjectVolcanoPiece::checkSphereCachedPartial(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSphereImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset,
            &ObjColMgr::checkSphereCachedPartial);
}

/// @addr{0x80804730}
/// @copydoc ObjectDrivable::checkSphereCachedPartialPush()
bool ObjectVolcanoPiece::checkSphereCachedPartialPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return checkSphereImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset,
            &ObjColMgr::checkSphereCachedPartialPush);
}

/// @addr{0x808048B4}
/// @copydoc ObjectDrivable::checkSphereCachedFull()
bool ObjectVolcanoPiece::checkSphereCachedFull(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkSphereImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset,
            &ObjColMgr::checkSphereCachedFull);
}

/// @addr{0x80804A38}
/// @copydoc ObjectDrivable::checkSphereCachedFullPush()
bool ObjectVolcanoPiece::checkSphereCachedFullPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkCollisionCached(radius, pos, prevPos, mask, info, maskOut, timeOffset);
}

/// @addr{0x80818334}
/// @copybrief ObjectKCL::update()
/// @param timeOffset The time offset used to calculate the current frame's transformation
/// @details If the volcano piece is in the @ref State::Rest or @ref State::Gone state, then this
/// function does nothing. Otherwise, it updates the transformation matrices of the primary and
/// secondary @ref ObjColMgr and sets moving object velocity.
void ObjectVolcanoPiece::update(u32 timeOffset) {
    State state = calcState(System::RaceManager::Instance()->timer() - timeOffset);
    if (state == State::Rest || state == State::Gone) {
        return;
    }

    EGG::Vector3f movingObjVel;
    const EGG::Matrix34f &rtMat = calcShakeAndFall(&movingObjVel, timeOffset);
    EGG::Matrix34f invMat;
    rtMat.ps_inverse(invMat);

    m_objColMgr->setMtx(rtMat);
    m_objColMgr->setInvMtx(invMat);

    if (m_colMgrB) {
        m_colMgrB->setMtx(rtMat);
        m_colMgrB->setInvMtx(invMat);
    }

    setMovingObjVel(movingObjVel);
}

/// @addr{0x80818674}
/// @copybrief ObjectKCL::calcScale()
/// @param timeOffset The time offset used to calculate the current frame's transformation
/// @details Determines the volcano piece's current @ref State. If the volcano piece is at rest or
/// is gone, then this function returns early and does not update the scale. Otherwise, it
/// updates the scale based on @ref getScaleY().
void ObjectVolcanoPiece::calcScale(u32 timeOffset) {
    State state = calcState(System::RaceManager::Instance()->timer() - timeOffset);

    if (state == State::Rest || state == State::Gone) {
        return;
    }

    f32 scale = getScaleY(timeOffset);
    m_objColMgr->setScale(scale);

    if (m_colMgrB) {
        m_colMgrB->setScale(scale);
    }
}

/// @addr{0x80805924}
/// @copydoc ObjectKCL::setMovingObjVel()
/// @details Sets the moving object velocity for both @ref m_objColMgr and @ref m_colMgrB.
void ObjectVolcanoPiece::setMovingObjVel(const EGG::Vector3f &v) {
    m_objColMgr->setMovingObjVel(v);

    if (m_colMgrB) {
        m_colMgrB->setMovingObjVel(v);
    }
}

/// @addr{0x8080595C}
/// @copybrief ObjectKCL::getUpdatedMatrix()
/// @param timeOffset The time offset used to calculate the current frame's transformation
/// @details Calls @ref calcShakeAndFall() to compute the current transformation matrix based on
/// the piece's current state.
const EGG::Matrix34f &ObjectVolcanoPiece::getUpdatedMatrix(u32 timeOffset) {
    return calcShakeAndFall(nullptr, timeOffset);
}

/// @addr{0x808199A8}
/// @copydoc ObjectKCL::checkCollision()
bool ObjectVolcanoPiece::checkCollision(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkCollisionImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset,
            &ObjColMgr::checkSphereFullPush);
}

/// @addr{0x80819DA0}
/// @copydoc ObjectKCL::checkCollisionCached()
bool ObjectVolcanoPiece::checkCollisionCached(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkCollisionImpl(radius, pos, prevPos, mask, info, maskOut, timeOffset,
            &ObjColMgr::checkSphereCachedFullPush);
}

/// @addr{0x808187B4}
/// @brief Updates position to reflect the fall duration or the current step in its shake cycle
/// @param vel Output vector that will be updated with the current moving object velocity of the
/// volcano piece
/// @param timeOffset The time offset used to calculate the volcano piece's shake and fall motion
/// @return The updated transformation matrix reflecting the current position and rotation of the
/// volcano piece
/// @details Calls @ref getShakePosY() to find the current vertical position of the volcano piece.
/// If `vel` is not `nullptr`, then it will be updated with the difference between the current and
/// previous positions, representing the current moving object velocity of the volcano piece.
/// Kinoko only updates the translation of @ref m_rtMat, since its rotation never changes.
const EGG::Matrix34f &ObjectVolcanoPiece::calcShakeAndFall(EGG::Vector3f *vel, u32 timeOffset) {
    u32 t = System::RaceManager::Instance()->timer() - timeOffset;

    f32 currPosY = getShakePosY(t);

    if (vel) {
        *vel = EGG::Vector3f(0.0f, currPosY - getShakePosY(t - 1), 0.0f);
    }

    m_rtMat.setBase(3, EGG::Vector3f(m_initialPos.x, currPosY, m_initialPos.z));
    return m_rtMat;
}

/** @addr{0x80818FCC}
 * @brief Calculates the current state of the volcano piece based on the framecount
 * @param frame The current framecount in the race
 * @return The current state of the volcano piece
 * @details \f[
 * calcState(frame) = \begin{cases}
 *     Rest & frame < m_restEndFrame \\
 *     Shake & m_restEndFrame \le frame < m_shakeEndFrame \\
 *     Quake & m_shakeEndFrame \le frame < m_quakeEndFrame \\
 *     Fall & m_quakeEndFrame \le frame < m_quakeEndFrame + fallDuration \\
 *     Gone & \text{otherwise}
 * \end{cases}
 * \f]
 **/
ObjectVolcanoPiece::State ObjectVolcanoPiece::calcState(u32 frame) const {
    if (frame < m_restEndFrame) {
        return State::Rest;
    }

    if (frame < m_shakeEndFrame) {
        return State::Shake;
    }

    if (frame < m_quakeEndFrame) {
        return State::Quake;
    }

    if (frame < m_quakeEndFrame + FALL_DURATION) {
        return State::Fall;
    }

    return State::Gone;
}

/** @addr{0x80819028}
 * @brief Calculates elapsed time within the current state based on the framecount
 * @param frame The current framecount in the race
 * @return The elapsed framecount within the current state
 * @details \f[
 * calcT(frame) = \begin{cases}
 *     frame & frame < restDuration \\
 *     frame - restDuration & restDuration \le frame < shakeDuration \\
 *     frame - shakeDuration & shakeDuration \le frame < quakeDuration \\
 *     frame - quakeDuration & quakeDuration \le frame < quakeDuration + fallDuration \\
 *     frame - quakeDuration - fallDuration & \text{otherwise}
 * \end{cases}
 * \f]
 **/
f32 ObjectVolcanoPiece::calcT(u32 frame) const {
    if (frame < m_restEndFrame) {
        return static_cast<f32>(frame);
    }

    if (frame < m_shakeEndFrame) {
        return static_cast<f32>(frame - m_restEndFrame);
    }

    if (frame < m_quakeEndFrame) {
        return static_cast<f32>(frame - m_shakeEndFrame);
    }

    if (frame < m_quakeEndFrame + FALL_DURATION) {
        return static_cast<f32>(frame - m_quakeEndFrame);
    }

    return static_cast<f32>(frame - m_quakeEndFrame - FALL_DURATION);
}

/// @brief Inlined function that calculates the volcano piece's vertical position at a given time
/// `t`
/// @param t The current framecount of the race
/// @return The vertical position of the volcano piece at the given framecount
/// @details If the volcano piece is in the @ref State::Shake state at the given time `t`, then its
/// vertical position follows a cycle of 8 frames. For the first 4 frames, it rises `5.0f` units
/// per frame above its initial position, up to `20.0f`. It then jumps to `5.0f` units below its
/// initial position and drops `5.0f` units per frame for the last 4 frames, down to `20.0f` below.
/// If the volcano piece is in the @ref State::Fall state, then its position decreases by `10.0f`
/// units per frame. Once the volcano piece is in the @ref State::Gone state, its position remains
/// at the last fallen position.
f32 ObjectVolcanoPiece::getShakePosY(u32 t) const {
    constexpr f32 FALL_SPEED = 10.0f;
    constexpr s32 SHAKE_STEPS = 8;
    constexpr s32 SHAKE_STEPS_HALF = SHAKE_STEPS / 2;
    constexpr f32 SHAKE_STEP_AMPLITUDE = 5.0f;

    State state = calcState(t);

    f32 posY = m_initialPos.y;

    switch (state) {
    case State::Shake: {
        s32 step = static_cast<s32>(calcT(t)) % SHAKE_STEPS + 1;

        if (step > SHAKE_STEPS_HALF) {
            step = SHAKE_STEPS_HALF - step;
        }
        posY += SHAKE_STEP_AMPLITUDE * static_cast<f32>(step);
    } break;
    case State::Fall:
        posY -= FALL_SPEED * static_cast<f32>(calcT(t));
        break;
    case State::Gone:
        posY -= FALL_SPEED * static_cast<f32>(FALL_DURATION);
        break;
    default:
        break;
    }

    return posY;
}

} // namespace Kinoko::Field
