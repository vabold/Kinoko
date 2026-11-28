#include "ObjectBelt.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/CollisionInfo.hh"

#include "game/system/RaceManager.hh"

namespace Kinoko::Field {

/// @addr{0x807FC248}
/// @copybrief ObjectDrivable::ObjectDrivable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Initializes @ref m_roadVel to `32.0f`.
ObjectBelt::ObjectBelt(const System::MapdataGeoObj &params)
    : ObjectDrivable(params), m_roadVel(32.0f) {}

/// @addr{0x807FC5EC}
/// @brief Default virtual destructor
ObjectBelt::~ObjectBelt() = default;

/// @addr{0x807FD79C}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectBelt::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x807FD798}
/// @copybrief ObjectBase::createCollision()
/// @details No-op because collision is managed entirely by the collision functions.
void ObjectBelt::createCollision() {}

/// @addr{0x807FD794}
/// @copybrief ObjectBase::calcCollisionTransform()
/// @details No-op because collision is managed entirely by the collision functions.
void ObjectBelt::calcCollisionTransform() {}

/// @addr{0x807FD784}
/// @copybrief ObjectBase::getCollisionRadius()
/// @return The collision radius of the conveyor belt, `25000.0f`.
f32 ObjectBelt::getCollisionRadius() const {
    return 25000.0f;
}

/// @addr{0x807FD750}
/// @copybrief ObjectDrivable::checkPointPartial()
/// @return `false`, as this class only implements full push collision checks
bool ObjectBelt::checkPointPartial(const EGG::Vector3f & /*pos*/, const EGG::Vector3f & /*prevPos*/,
        KCLTypeMask /*mask*/, CollisionInfoPartial * /*info*/, KCLTypeMask * /*maskOut*/) {
    return false;
}

/// @addr{0x807FD758}
/// @copybrief ObjectDrivable::checkPointPartialPush()
/// @return `false`, as this class only implements full push collision checks
bool ObjectBelt::checkPointPartialPush(const EGG::Vector3f & /*pos*/,
        const EGG::Vector3f & /*prevPos*/, KCLTypeMask /*mask*/, CollisionInfoPartial * /*info*/,
        KCLTypeMask * /*maskOut*/) {
    return false;
}

/// @addr{0x807FD760}
/// @copybrief ObjectDrivable::checkPointFull()
/// @return `false`, as this class only implements full push collision checks
bool ObjectBelt::checkPointFull(const EGG::Vector3f & /*pos*/, const EGG::Vector3f & /*prevPos*/,
        KCLTypeMask /*mask*/, CollisionInfo * /*info*/, KCLTypeMask * /*maskOut*/) {
    return false;
}

/// @addr{0x807FD768}
/// @copydoc ObjectDrivable::checkPointFullPush()
/// @details Dispatches to @ref calcCollision().
bool ObjectBelt::checkPointFullPush(const EGG::Vector3f &pos, const EGG::Vector3f & /*prevPos*/,
        KCLTypeMask /*mask*/, CollisionInfo *info, KCLTypeMask *maskOut) {
    return calcCollision(pos, info, maskOut, 0);
}

/// @addr{0x807FD728}
/// @copybrief ObjectDrivable::checkSpherePartial()
/// @return `false`, as this class only implements full push collision checks
bool ObjectBelt::checkSpherePartial(f32 /*radius*/, const EGG::Vector3f & /*pos*/,
        const EGG::Vector3f & /*prevPos*/, KCLTypeMask /*mask*/, CollisionInfoPartial * /*info*/,
        KCLTypeMask * /*maskOut*/, u32 /*timeOffset*/) {
    return false;
}

/// @addr{0x807FD730}
/// @copybrief ObjectDrivable::checkSpherePartialPush()
/// @return `false`, as this class only implements full push collision checks
bool ObjectBelt::checkSpherePartialPush(f32 /*radius*/, const EGG::Vector3f & /*pos*/,
        const EGG::Vector3f & /*prevPos*/, KCLTypeMask /*mask*/, CollisionInfoPartial * /*info*/,
        KCLTypeMask * /*maskOut*/, u32 /*timeOffset*/) {
    return false;
}

/// @addr{0x807FD738}
/// @copybrief ObjectDrivable::checkSphereFull()
/// @return `false`, as this class only implements full push collision checks
bool ObjectBelt::checkSphereFull(f32 /*radius*/, const EGG::Vector3f & /*pos*/,
        const EGG::Vector3f & /*prevPos*/, KCLTypeMask /*mask*/, CollisionInfo * /*info*/,
        KCLTypeMask * /*maskOut*/, u32 /*timeOffset*/) {
    return false;
}

/// @addr{0x807FD740}
/// @copydoc ObjectDrivable::checkSphereFullPush()
/// @details Dispatches to @ref calcCollision().
bool ObjectBelt::checkSphereFullPush(f32 /*radius*/, const EGG::Vector3f &pos,
        const EGG::Vector3f & /*prevPos*/, KCLTypeMask /*mask*/, CollisionInfo *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return calcCollision(pos, info, maskOut, timeOffset);
}

/// @addr{0x807FD6F4}
/// @copybrief ObjectDrivable::checkPointCachedPartial()
/// @return `false`, as this class only implements full push collision checks
bool ObjectBelt::checkPointCachedPartial(const EGG::Vector3f & /*pos*/,
        const EGG::Vector3f & /*prevPos*/, KCLTypeMask /*mask*/, CollisionInfoPartial * /*info*/,
        KCLTypeMask * /*maskOut*/) {
    return false;
}

/// @addr{0x807FD6FC}
/// @copybrief ObjectDrivable::checkPointCachedPartialPush()
/// @return `false`, as this class only implements full push collision checks
bool ObjectBelt::checkPointCachedPartialPush(const EGG::Vector3f & /*pos*/,
        const EGG::Vector3f & /*prevPos*/, KCLTypeMask /*mask*/, CollisionInfoPartial * /*info*/,
        KCLTypeMask * /*maskOut*/) {
    return false;
}

/// @addr{0x807FD704}
/// @copybrief ObjectDrivable::checkPointCachedFull()
/// @return `false`, as this class only implements full push collision checks
bool ObjectBelt::checkPointCachedFull(const EGG::Vector3f & /*pos*/,
        const EGG::Vector3f & /*prevPos*/, KCLTypeMask /*mask*/, CollisionInfo * /*info*/,
        KCLTypeMask * /*maskOut*/) {
    return false;
}

/// @addr{0x807FD70C}
/// @copydoc ObjectDrivable::checkPointCachedFullPush()
/// @details Dispatches to @ref calcCollision().
bool ObjectBelt::checkPointCachedFullPush(const EGG::Vector3f &pos,
        const EGG::Vector3f & /*prevPos*/, KCLTypeMask /*mask*/, CollisionInfo *info,
        KCLTypeMask *maskOut) {
    return calcCollision(pos, info, maskOut, 0);
}

/// @addr{0x807FD6CC}
/// @copybrief ObjectDrivable::checkSphereCachedPartial()
/// @return `false`, as this class only implements full push collision checks
bool ObjectBelt::checkSphereCachedPartial(f32 /*radius*/, const EGG::Vector3f & /*pos*/,
        const EGG::Vector3f & /*prevPos*/, KCLTypeMask /*mask*/, CollisionInfoPartial * /*info*/,
        KCLTypeMask * /*maskOut*/, u32 /*timeOffset*/) {
    return false;
}

/// @addr{0x807FD6D4}
/// @copybrief ObjectDrivable::checkSphereCachedPartialPush()
/// @return `false`, as this class only implements full push collision checks
bool ObjectBelt::checkSphereCachedPartialPush(f32 /*radius*/, const EGG::Vector3f & /*pos*/,
        const EGG::Vector3f & /*prevPos*/, KCLTypeMask /*mask*/, CollisionInfoPartial * /*info*/,
        KCLTypeMask * /*maskOut*/, u32 /*timeOffset*/) {
    return false;
}

/// @addr{0x807FD6DC}
/// @copybrief ObjectDrivable::checkSphereCachedFull()
/// @return `false`, as this class only implements full push collision checks
bool ObjectBelt::checkSphereCachedFull(f32 /*radius*/, const EGG::Vector3f & /*pos*/,
        const EGG::Vector3f & /*prevPos*/, KCLTypeMask /*mask*/, CollisionInfo * /*info*/,
        KCLTypeMask * /*maskOut*/, u32 /*timeOffset*/) {
    return false;
}

/// @addr{0x807FD6E4}
/// @copydoc ObjectDrivable::checkSphereCachedFullPush()
/// @details Dispatches to @ref calcCollision().
bool ObjectBelt::checkSphereCachedFullPush(f32 /*radius*/, const EGG::Vector3f &pos,
        const EGG::Vector3f & /*prevPos*/, KCLTypeMask /*mask*/, CollisionInfo *info,
        KCLTypeMask *maskOut, u32 timeOffset) {
    return calcCollision(pos, info, maskOut, timeOffset);
}

/// @addr{0x807FC294}
/// @brief Checks if an object at a given position is colliding with the conveyor and if so, saves
/// the road velocity to the @ref CollisionInfo
/// @param pos The position of the object being checked for collision
/// @param info Out parameter for retrieving collision information (if any)
/// @param maskOut The KCL flags that were hit during the course KCL check (if any)
/// @param timeOffset The time offset to use when calculating the road velocity
/// @return Whether or not a collision with the conveyor belt was detected
/// @details Early returns `false` if `maskOut` does not have @ref KColType::MovingRoad set. Next,
/// ensures that we actually have a collision entry of type @ref KColType::MovingRoad, returning
/// `false` if this is not the case. Next, checks to see if the conveyor belt is actually moving,
/// returning `false` if it is stationary. Finally, if a collision is detected, sets the collision
/// info's moving floor distance and road velocity based on the player's position, the conveyor
/// belt's variant, and the race framecount.
/// @note This function is unique from other collision check functions in that it requires that
/// `maskOut` has already had @ref KColType::MovingRoad set before calling. Conveyor belts will only
/// register collisions if the conveyor belt is placed on @ref KColType::MovingRoad present in the
/// course KCL.
/// @note This function is normally virtual in the base game, but since no derived class overrides
/// it, we can devirtualize for Kinoko.
bool ObjectBelt::calcCollision(const EGG::Vector3f &pos, CollisionInfo *info, KCLTypeMask *maskOut,
        u32 timeOffset) {
    if ((*maskOut & KCL_TYPE_BIT(KColType::MovingRoad)) == 0) {
        return false;
    }

    auto *colDir = CollisionDirector::Instance();
    const auto *entry = colDir->findClosestCollisionEntry(KCL_TYPE_BIT(KColType::MovingRoad));
    if (!entry) {
        return false;
    }

    if (!isMoving(entry->variant())) {
        return false;
    }

    if (entry->dist > info->movingFloorDist) {
        info->movingFloorDist = entry->dist;
        info->roadVelocity = calcRoadVelocity(entry->variant(), pos,
                System::RaceManager::Instance()->timer() - timeOffset);
    }

    return true;
}

} // namespace Kinoko::Field
