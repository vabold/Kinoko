#include "ObjectDrivable.hh"

#include "game/field/BoxColManager.hh"
#include "game/field/ObjectDrivableDirector.hh"

namespace Kinoko::Field {

/// @addr{0x8081A6D0}
/// @copybrief ObjectBase::ObjectBase(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
ObjectDrivable::ObjectDrivable(const System::MapdataGeoObj &params) : ObjectBase(params) {}

/// @addr{0x8067EB3C}
/// @brief Default virtual destructor
ObjectDrivable::~ObjectDrivable() = default;

/// @addr{0x8081A79C}
/// @copybrief ObjectBase::load()
/// @details Creates and initializes the object's collision and associated @ref BoxColUnit, and
/// registers the object to the @ref ObjectDrivableDirector.
void ObjectDrivable::load() {
    createCollision();
    initCollision();
    loadAABB(getCollisionRadius());

    ObjectDrivableDirector::Instance()->addObject(this);
}

/// @addr{0x80682918}
/// @copybrief ObjectBase::getCollisionRadius()
/// @return The default collision radius of a drivable object, `5000.0f`.
f32 ObjectDrivable::getCollisionRadius() const {
    return 5000.0f;
}

/// @brief Called once after collision creation to initialize the collision
void ObjectDrivable::initCollision() {}

/// @addr{0x8081A85C}
/// @brief Inserts this object into the @ref BoxColManager as a drivable entry
/// @param radius The collision radius for the @ref BoxColUnit
/// @details This function is normally virtual in the base game. Since no derived class overrides
/// it, we can devirtualize for Kinoko.
void ObjectDrivable::loadAABB(f32 radius) {
    auto *boxColMgr = BoxColManager::Instance();
    const EGG::Vector3f &pos = getPosition();
    m_boxColUnit = boxColMgr->insertDrivable(radius, 0.0f, &pos, false, this);
}

} // namespace Kinoko::Field
