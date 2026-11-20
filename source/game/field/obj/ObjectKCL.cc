#include "ObjectKCL.hh"

#include "game/system/RaceManager.hh"
#include "game/system/ResourceManager.hh"

namespace Kinoko::Field {

/// @addr{0x8081A980}
/// @copybrief ObjectDrivable::ObjectDrivable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Initializes @ref m_lastMtxUpdateFrame and @ref m_lastScaleUpdateFrame to `-2000` so
/// that the collision manager will be updated on the first frame.
ObjectKCL::ObjectKCL(const System::MapdataGeoObj &params)
    : ObjectDrivable(params), m_lastMtxUpdateFrame(-2000), m_lastScaleUpdateFrame(-2000) {}

/// @addr{0x8067EAFC}
/// @brief Virtual destructor that destroys the associated collision manager
ObjectKCL::~ObjectKCL() {
    EGG::egg_delete(m_objColMgr);
}

/// @addr{0x8081AA58}
/// @copybrief ObjectBase::createCollision()
/// @details Loads the KCL file for the object and creates an @ref ObjColMgr to interface with
/// it.
void ObjectKCL::createCollision() {
    char filepath[128];
    snprintf(filepath, sizeof(filepath), "%s.kcl", getKclName());

    auto *resMgr = System::ResourceManager::Instance();
    m_objColMgr =
            EGG::egg_new<ObjColMgr>(resMgr->getFile(filepath, nullptr, System::ArchiveId::Course));
}

/// @addr{0x80681490}
/// @copybrief ObjectBase::calcCollisionTransform()
/// @details Dispatches to @ref ObjectKCL::update() to update the collision manager's transform
/// for the current frame.
void ObjectKCL::calcCollisionTransform() {
    update(0);
}

/// @addr{0x8081AB4C}
/// @copybrief ObjectDrivable::initCollision()
/// @details Saves the initial transformation matrix to the @ref ObjColMgr and computes the KCL's
/// bounding box midpoint and half-width.
void ObjectKCL::initCollision() {
    const EGG::Matrix34f &mat = getUpdatedMatrix(0);
    EGG::Matrix34f matInv;

    bool inverted = mat.ps_inverse(matInv);
    ASSERT(inverted);

    m_objColMgr->setMtx(mat);
    m_objColMgr->setInvMtx(matInv);
    m_objColMgr->setScale(getScaleY(0));

    EGG::Vector3f high = m_objColMgr->kclHighWorld();
    EGG::Vector3f low = m_objColMgr->kclLowWorld();

    m_kclMidpoint = (high + low).multInv(2.0f);

    EGG::Vector3f highLowDiffAbs = (high - low).abs();
    f32 maxDiff = std::max(highLowDiffAbs.x, highLowDiffAbs.z);
    m_bboxHalfSideLength = maxDiff * 0.5f;
}

/// @addr{0x80681448}
/// @copydoc ObjectBase::getPosition()
/// @details Computed as the midpoint of the KCL's bounding box.
const EGG::Vector3f &ObjectKCL::getPosition() const {
    return m_kclMidpoint;
}

/// @addr{0x80687D70}
/// @copybrief ObjectBase::getCollisionRadius()
/// @return The collision radius of the KCL object, calculated based on its bounding box and an
/// optional additional length.
f32 ObjectKCL::getCollisionRadius() const {
    return m_bboxHalfSideLength + colRadiusAdditionalLength();
}

/// @addr{0x806810F8}
/// @copydoc ObjectDrivable::checkPointPartial()
bool ObjectKCL::checkPointPartial(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfoPartial *info, KCLTypeMask *maskOut) {
    return m_objColMgr->checkPointPartial(pos, prevPos, mask, info, maskOut);
}

/// @addr{0x806811B0}
/// @copydoc ObjectDrivable::checkPointPartialPush()
bool ObjectKCL::checkPointPartialPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfoPartial *info, KCLTypeMask *maskOut) {
    return m_objColMgr->checkPointPartialPush(pos, prevPos, mask, info, maskOut);
}

/// @addr{0x80681268}
/// @copydoc ObjectDrivable::checkPointFull()
bool ObjectKCL::checkPointFull(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return m_objColMgr->checkPointFull(pos, prevPos, mask, info, maskOut);
}

/// @addr{0x80681320}
/// @copydoc ObjectDrivable::checkPointFullPush()
bool ObjectKCL::checkPointFullPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return m_objColMgr->checkPointFullPush(pos, prevPos, mask, info, maskOut);
}

/// @addr{0x80680DF4}
/// @copydoc ObjectDrivable::checkSpherePartial()
bool ObjectKCL::checkSpherePartial(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    calcScale(timeOffset);
    update(timeOffset);

    return m_objColMgr->checkSpherePartial(radius, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x80680EF0}
/// @copydoc ObjectDrivable::checkSpherePartialPush()
bool ObjectKCL::checkSpherePartialPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    calcScale(timeOffset);
    update(timeOffset);

    return m_objColMgr->checkSpherePartialPush(radius, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x80680FEC}
/// @copydoc ObjectDrivable::checkSphereFull()
bool ObjectKCL::checkSphereFull(f32 radius, const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut, u32 timeOffset) {
    calcScale(timeOffset);
    update(timeOffset);

    return m_objColMgr->checkSphereFull(radius, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x806810E8}
/// @copydoc ObjectDrivable::checkSphereFullPush()
bool ObjectKCL::checkSphereFullPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkCollision(radius, pos, prevPos, mask, info, maskOut, timeOffset);
}

/// @addr{0x806807E8}
/// @copybrief ObjectDrivable::narrScLocal()
/// @param radius The radius of the sphere to check
/// @param pos The position of the sphere to check
/// @param mask The KCL flags to check collision against (other types are ignored)
/// @param timeOffset Optional time offset (unused)
void ObjectKCL::narrScLocal(f32 radius, const EGG::Vector3f &pos, KCLTypeMask mask,
        u32 /*timeOffset*/) {
    m_objColMgr->narrScLocal(radius, pos, mask);
}

/// @addr{0x80680B14}
/// @copydoc ObjectDrivable::checkPointCachedPartial()
bool ObjectKCL::checkPointCachedPartial(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfoPartial *info, KCLTypeMask *maskOut) {
    return m_objColMgr->checkPointCachedPartial(pos, prevPos, mask, info, maskOut);
}

/// @addr{0x80680BCC}
/// @copydoc ObjectDrivable::checkPointCachedPartialPush()
bool ObjectKCL::checkPointCachedPartialPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfoPartial *info, KCLTypeMask *maskOut) {
    return m_objColMgr->checkPointCachedPartialPush(pos, prevPos, mask, info, maskOut);
}

/// @addr{0x80680C84}
/// @copydoc ObjectDrivable::checkPointCachedFull()
bool ObjectKCL::checkPointCachedFull(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return m_objColMgr->checkPointCachedFull(pos, prevPos, mask, info, maskOut);
}

/// @addr{0x80680D3C}
/// @copydoc ObjectDrivable::checkPointCachedFullPush()
bool ObjectKCL::checkPointCachedFullPush(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut) {
    return m_objColMgr->checkPointCachedFullPush(pos, prevPos, mask, info, maskOut);
}

/// @addr{0x806807F0}
/// @copydoc ObjectDrivable::checkSphereCachedPartial()
bool ObjectKCL::checkSphereCachedPartial(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    update(timeOffset);
    calcScale(timeOffset);

    return m_objColMgr->checkSphereCachedPartial(radius, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x806808FC}
/// @copydoc ObjectDrivable::checkSphereCachedPartialPush()
bool ObjectKCL::checkSphereCachedPartialPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfoPartial *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    update(timeOffset);
    calcScale(timeOffset);

    return m_objColMgr->checkSphereCachedPartialPush(radius, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x80680A08}
/// @copydoc ObjectDrivable::checkSphereCachedFull()
bool ObjectKCL::checkSphereCachedFull(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    update(timeOffset);
    calcScale(timeOffset);

    return m_objColMgr->checkSphereCachedFull(radius, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x80680B04}
/// @copydoc ObjectDrivable::checkSphereCachedFullPush()
bool ObjectKCL::checkSphereCachedFullPush(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    return checkCollisionCached(radius, pos, prevPos, mask, info, maskOut, timeOffset);
}

/// @addr{0x8081AD6C}
/// @brief Advances the collision manager's transform to reflect the current frame
/// @param timeOffset The time offset used to calculate the current frame's transformation
/// @details If the collision manager's transform has already been updated for the current frame,
/// this function does nothing. Otherwise, it computes the updated transformation matrix and sets
/// it in the collision manager. If `timeOffset` is 0, then computes the transformation via @ref
/// calcTransform(), otherwise calls @ref getUpdatedMatrix(). Finally, updates @ref
/// m_lastMtxUpdateFrame.
void ObjectKCL::update(u32 timeOffset) {
    u32 time = System::RaceManager::Instance()->timer() - timeOffset;
    if (m_lastMtxUpdateFrame == static_cast<s32>(time)) {
        return;
    }

    EGG::Matrix34f mat;

    if (timeOffset == 0) {
        calcTransform();
        mat = transform();
    } else {
        mat = getUpdatedMatrix(timeOffset);
    }

    EGG::Matrix34f matInv;
    mat.ps_inverse(matInv);
    m_objColMgr->setMtx(mat);
    m_objColMgr->setInvMtx(matInv);

    m_lastMtxUpdateFrame = time;
}

/// @addr{0x8081AF28}
/// @brief Updates the collision manager's scale to reflect the current frame
/// @param timeOffset The time offset used to calculate the current frame's transformation
/// @details If the collision manager's scale has already been updated for the current frame,
/// this function does nothing. Otherwise, it computes the updated scale and sets it in the
/// collision manager. If the countdown has just ended this frame, then uses the default scale via
/// @ref scale(), otherwise calls @ref getScaleY(). Finally, updates @ref m_lastScaleUpdateFrame.
void ObjectKCL::calcScale(u32 timeOffset) {
    u32 time = System::RaceManager::Instance()->timer() - timeOffset;
    if (m_lastScaleUpdateFrame == static_cast<s32>(time)) {
        return;
    }

    m_objColMgr->setScale(time == 0 ? scale().y : getScaleY(timeOffset));
    m_lastScaleUpdateFrame = time;
}

/// @addr{0x80681450}
/// @brief Sets the velocity that describes the movement effect when a player is on the object
/// @param v The velocity vector
void ObjectKCL::setMovingObjVel(const EGG::Vector3f &v) {
    m_objColMgr->setMovingObjVel(v);
}

/// @addr{0x807FEAC0}
/// @brief Computes an updated collision transformation matrix for the current frame
/// @return Const ref to the updated transformation matrix.
const EGG::Matrix34f &ObjectKCL::getUpdatedMatrix(u32 /*timeOffset*/) {
    calcTransform();
    return transform();
}

/// @addr{0x80687DB0}
/// @brief Gets the object's Y-axis scale for the current frame
/// @param timeOffset Optional time offset (unused)
/// @return The current scale of the object in the Y direction.
f32 ObjectKCL::getScaleY(u32 /* timeOffset */) const {
    return scale().y;
}

/// @addr{0x8068143C}
/// @brief Optional additional length to add when computing the collision radius
/// @return The default length, `0.0f`.
f32 ObjectKCL::colRadiusAdditionalLength() const {
    return 0.0f;
}

/// @addr{0x8081AFB4}
/// @brief Checks collision between a sphere and the object, writing out full collision info
/// @param radius The radius of the sphere to check for collision
/// @param pos The current position of the sphere
/// @param prevPos The previous position of the sphere
/// @param mask The collision mask to use for the check
/// @param info Out parameter for retrieving collision information (if any)
/// @param maskOut The KCL flags that were hit during the collision check (if any)
/// @param timeOffset The time offset used to calculate the current frame's collision
/// @return `true` if a collision was detected, `false` otherwise.
/// @desync This function can result in physics desynchronizations when racing a ghost.
/// Since this function is only called when the player/ghost's hitbox is close enough to the
/// object (as per GJK collision checks), it is possible that the first player hitbox is just
/// barely too far away from the object while the second player hitbox is just close enough to
/// pass the GJK check and thus call this function and update the collision manager's transform.
/// Subsequently, if the ghost's first hitbox normally would be too far away from the object to
/// pass the GJK check, the transform update from the player's second hitbox check means that
/// the ghost's first hitbox is now being checked against a different transform than the player,
/// which can result in the GJK check passing when it originally would fail. This is the reason
/// that the DS Delfino Square bridge can sometimes desync.
bool ObjectKCL::checkCollision(f32 radius, const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
        KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut, u32 timeOffset) {
    update(timeOffset);
    calcScale(timeOffset);

    return m_objColMgr->checkSphereFullPush(radius, pos, prevPos, mask, info, maskOut);
}

/// @addr{0x8081B16C}
/// @brief Checks collision between a sphere and the object, using only cached KCL prisms,
/// writing out full collision info
/// @param radius The radius of the sphere to check for collision
/// @param pos The current position of the sphere
/// @param prevPos The previous position of the sphere
/// @param mask The collision mask to use for the check
/// @param info Out parameter for retrieving collision information (if any)
/// @param maskOut The KCL flags that were hit during the collision check (if any)
/// @param timeOffset The time offset used to calculate the current frame's collision
/// @return `true` if a collision was detected, `false` otherwise.
/// @desync This function can result in physics desynchronizations when racing a ghost.
/// Since this function is only called when the player/ghost's hitbox is close enough to the
/// object (as per GJK collision checks), it is possible that the first player hitbox is just
/// barely too far away from the object while the second player hitbox is just close enough to
/// pass the GJK check and thus call this function and update the collision manager's transform.
/// Subsequently, if the ghost's first hitbox normally would be too far away from the object to
/// pass the GJK check, the transform update from the player's second hitbox check means that
/// the ghost's first hitbox is now being checked against a different transform than the player,
/// which can result in the GJK check passing when it originally would fail. This is the reason
/// that the DS Delfino Square bridge can sometimes desync.
bool ObjectKCL::checkCollisionCached(f32 radius, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    update(timeOffset);
    calcScale(timeOffset);

    return m_objColMgr->checkSphereCachedFullPush(radius, pos, prevPos, mask, info, maskOut);
}

} // namespace Kinoko::Field
