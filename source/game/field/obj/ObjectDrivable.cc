#include "ObjectDrivable.hh"

#include "game/field/ObjectDrivableDirector.hh"

namespace Field {

/// @addr{0x8081A6D0}
ObjectDrivable::ObjectDrivable(const System::MapdataGeoObj &params) : ObjectBase(params) {}

/// @addr{0x8067EB3C}
ObjectDrivable::~ObjectDrivable() = default;

/// @addr{0x8081A79C}
void ObjectDrivable::load() {
    createCollision();
    initCollision();
    loadAABB(getCollisionRadius());

    ObjectDrivableDirector::Instance()->addObject(this);
}

/// @addr{0x8081A85C}
void ObjectDrivable::loadAABB(f32 radius) {
    auto *boxColMgr = BoxColManager::Instance();
    const EGG::Vector3f &pos = getPosition();
    m_boxColUnit = boxColMgr->insertDrivable(radius, 0.0f, &pos, false, this);
}

} // namespace Field
