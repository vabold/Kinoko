#include "ObjectBelt.hh"

#include "game/field/CollisionDirector.hh"

#include "game/system/RaceManager.hh"

namespace Field {

/// @addr{0x807FC248}
ObjectBelt::ObjectBelt(const System::MapdataGeoObj &params)
    : ObjectDrivable(params), m_roadVel(32.0f) {}

/// @addr{0x807FC5EC}
ObjectBelt::~ObjectBelt() = default;

/// @addr{0x807FC294}
bool ObjectBelt::calcCollision(const EGG::Vector3f &v0, const EGG::Vector3f & /*v1*/,
        KCLTypeMask /*mask*/, CollisionInfo *info, KCLTypeMask *maskOut, u32 timeOffset) {
    if ((*maskOut & KCL_TYPE_BIT(COL_TYPE_MOVING_ROAD)) == 0) {
        return false;
    }

    auto *colDir = CollisionDirector::Instance();
    if (!colDir->findClosestCollisionEntry(maskOut, KCL_TYPE_BIT(COL_TYPE_MOVING_ROAD))) {
        return false;
    }

    const auto *entry = colDir->closestCollisionEntry();
    if (!isMoving(entry->variant(), v0)) {
        return false;
    }

    if (entry->dist > info->movingFloorDist) {
        info->movingFloorDist = entry->dist;
        info->roadVelocity = calcRoadVelocity(entry->variant(), v0,
                static_cast<s32>(System::RaceManager::Instance()->timer() - timeOffset));
    }

    return true;
}

} // namespace Field
