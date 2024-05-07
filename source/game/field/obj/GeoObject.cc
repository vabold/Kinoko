#include "game/field/obj/GeoObject.hh"
#include "ObjDrivableHolder.hh"

namespace Field
{

void GeoObjectDrivable::init() {
    loadCollision();
    initCollision();
    registerBoxColUnit(calcCollisionRadius());
    ObjDrivableHolder::Instance()->push(this);
}

} // namespace Field
