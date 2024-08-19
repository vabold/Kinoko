#include "ObjectCollidable.hh"

namespace Field {

/// @addr{0x8081EFEC}
ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &params) : ObjectBase(params) {}

/// @addr{0x8067E384}
ObjectCollidable::~ObjectCollidable() = default;

/// @addr{0x8081F0A0}
void ObjectCollidable::init() {
    createCollision();
}

/// @addr{0x8081F224}
void ObjectCollidable::createCollision() {
    m_collision = new ObjectCollisionBase;
}

} // namespace Field
