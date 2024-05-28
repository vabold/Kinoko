#include "game/field/obj/GeoObject.hh"
#include "game/field/obj/ObjectDirector.hh"
#include "game/field/obj/ObjDrivableHolder.hh"
#include "game/field/obj/ObjFlow.hh"
#include "game/system/ResourceManager.hh"

#include <egg/math/Math.hh>
#include <cstring>

namespace Field
{

/// @addr{0x8081F828} 
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
    m_bMatrixShouldUpdatePosition = true;
    m_bMatrixShouldMakeRT = true;
}
/// @addr{0x8081F9BC}
GeoObject::GeoObject(u16 id, EGG::Vector3f &position, EGG::Vector3f &rotation, EGG::Vector3f &scale) {
    m_type = 0;
    m_id = id;
    m_mapDataGeoObj = nullptr;
    m_directorIndex = -1;
    m_isTangible = true;
    m_transformationMatrix = EGG::Matrix34f::ident;
    m_bMatrixShouldUpdatePosition = true;
    m_bMatrixShouldMakeRT = true;
    m_pos = position;
    m_rot = rotation;
    m_scale = scale;
}
/// @addr{0x8081FB04}
GeoObject::GeoObject(const char* name, EGG::Vector3f &position, EGG::Vector3f &rotation, EGG::Vector3f &scale) {
    m_type = 0;
    m_id = ObjectDirector::Instance()->objFlow()->getIdFromName(name);
    m_mapDataGeoObj = nullptr;
    m_directorIndex = -1;
    m_isTangible = true;
    m_transformationMatrix = EGG::Matrix34f::ident;
    m_bMatrixShouldUpdatePosition = true;
    m_bMatrixShouldMakeRT = true;
    m_pos = position;
    m_rot = rotation;
    m_scale = scale;
}
/// @addr{0x8067E3C4}
GeoObject::~GeoObject() = default;

/// @addr{0x80821640}
void GeoObject::updateMatrix() {
    if (m_bMatrixShouldMakeRT) {
        m_transformationMatrix.makeRT(m_rot, m_pos);
        m_bMatrixShouldUpdatePosition = false;
        m_bMatrixShouldMakeRT = false;
    } else if (m_bMatrixShouldUpdatePosition) {
        m_transformationMatrix[0, 3] = m_pos.x;
        m_transformationMatrix[1, 3] = m_pos.y;
        m_transformationMatrix[2, 3] = m_pos.z;
    }
}

/// @addr{0x808D6EF0}
u16 GeoObject::id() const {
    return m_id;
}
/// @addr{0x808D6EF4}
const char* GeoObject::name() const {
    const ObjFlow* objflow = ObjectDirector::Instance()->objFlow();
    return objflow->attrs(objflow->slots(m_id))->name();
}
/// @addr{0x808D6F00}
const char* GeoObject::resources() const {
    const ObjFlow* objflow = ObjectDirector::Instance()->objFlow();
    return objflow->attrs(objflow->slots(m_id))->resources();
}
/// @addr{0x808D6F68}
EGG::Vector3f GeoObject::position() const {
    return m_pos;
}

/// these don't actually exist in-game
u32 GeoObject::directorIndex() const {
    return m_directorIndex;
}
void GeoObject::setDirectorIndex(s32 val) {
    m_directorIndex = val;
}

void GeoObject::calc() {

}

f32 GeoObject::calcCollisionRadius() {
    return 100.0f;
}


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

/// @todo implement BoxColManager  
void GeoObjectDrivable::registerBoxColUnit(f32 /*radius*/) {

}


GeoObjectKCL::GeoObjectKCL(System::MapdataGeoObj* pMapDataGeoObj) : GeoObjectDrivable(pMapDataGeoObj) {

}

void GeoObjectKCL::loadCollision() {
    char buffer[0x80];
    snprintf(buffer, 0x80, "%s.kcl", resources());
    m_objectColMgr = new ObjectColMgr(buffer);
}
void GeoObjectKCL::updateCollision() {
    
}
f32 GeoObjectKCL::calcCollisionRadius() {
    return m_bboxHalfSideLength + colRadiusAdditionalLength();
}
void GeoObjectKCL::initCollision() {
    EGG::Matrix34f &mtx = getUpdatedMatrix();
    m_objectColMgr->setMtx(mtx);
    m_objectColMgr->setKCLScale(getScaleY());

    EGG::Vector3f bboxHigh = m_objectColMgr->getKclBboxHighWorld();
    EGG::Vector3f bboxLow = m_objectColMgr->getKclBboxLowWorld();
    m_kclMidpoint = (bboxHigh + bboxLow) / 2.0f;
    m_bboxHalfSideLength = std::max(EGG::Mathf::abs(bboxHigh.x - bboxLow.x), EGG::Mathf::abs(bboxHigh.z - bboxLow.z)) * 0.5f;
}

GeoObjectKCL::~GeoObjectKCL() {
    if (m_objectColMgr != nullptr) {
        delete m_objectColMgr;
    }
};

} // namespace Field
