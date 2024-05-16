#include "game/field/obj/GeoObject.hh"
#include "game/field/obj/ObjectDirector.hh"
#include "game/field/obj/ObjDrivableHolder.hh"
#include "game/field/obj/ObjFlow.hh"

#include <egg/math/Math.hh>

namespace Field
{

GeoObject::GeoObject(System::MapdataGeoObj* pMapDataGeoObj) {
    m_type = 0;
    m_id = pMapDataGeoObj->id();
    m_mapDataGeoObj = pMapDataGeoObj;
    m_directorIndex = -1;
    m_isTangible = true;
    m_transformationMatrix = EGG::Matrix34f::ident;
    m_pos = pMapDataGeoObj->position();
    m_rot = pMapDataGeoObj->rotation() * DEG2RAD;
    m_scale = pMapDataGeoObj->scale();
    m_flags = 0;
    m_flags |= 0xb;
}
GeoObject::GeoObject(u16 id, EGG::Vector3f &position, EGG::Vector3f &rotation, EGG::Vector3f &scale) {
    m_type = 0;
    m_id = id;
    m_mapDataGeoObj = nullptr;
    m_directorIndex = -1;
    m_isTangible = true;
    m_transformationMatrix = EGG::Matrix34f::ident;
    m_flags = 0;
    m_flags |= 1;
    m_flags |= 3;
    m_flags |= 0xb;
    m_pos = position;
    m_rot = rotation;
    m_scale = scale;
}
GeoObject::GeoObject(const char* name, EGG::Vector3f &position, EGG::Vector3f &rotation, EGG::Vector3f &scale) {
    m_type = 0;
    m_id = ObjectDirector::Instance()->objFlow()->getIdFromName(name);
    m_mapDataGeoObj = nullptr;
    m_directorIndex = -1;
    m_isTangible = true;
    m_transformationMatrix = EGG::Matrix34f::ident;
    m_flags = 0;
    m_flags |= 1;
    m_flags |= 3;
    m_flags |= 0xb;
    m_pos = position;
    m_rot = rotation;
    m_scale = scale;
}

GeoObject::~GeoObject() = default;


GeoObjectDrivable::GeoObjectDrivable(System::MapdataGeoObj* pMapDataGeoObj) : GeoObject(pMapDataGeoObj) {
    m_type = 2;
}

GeoObjectDrivable::~GeoObjectDrivable() = default;

void GeoObjectDrivable::init() {
    loadCollision();
    initCollision();
    registerBoxColUnit(calcCollisionRadius());
    ObjDrivableHolder::Instance()->push(this);
}


GeoObjectKCL::~GeoObjectKCL() = default;

} // namespace Field
