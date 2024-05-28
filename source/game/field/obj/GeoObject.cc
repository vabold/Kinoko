#include "game/field/obj/GeoObject.hh"
#include "game/field/obj/ObjectDirector.hh"
#include "game/field/obj/ObjDrivableHolder.hh"
#include "game/field/obj/ObjFlow.hh"
#include "game/system/ResourceManager.hh"

#include <egg/math/Math.hh>
#include <cstring>

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
    m_bMatrixShouldUpdatePosition = true;
    m_bMatrixShouldUpdateRT = true;
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
    m_bMatrixShouldUpdatePosition = true;
    m_bMatrixShouldUpdateRT = true;
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
    m_bMatrixShouldUpdatePosition = true;
    m_bMatrixShouldUpdateRT = true;
    m_flags |= 0xb;
    m_pos = position;
    m_rot = rotation;
    m_scale = scale;
}

GeoObject::~GeoObject() = default;

void GeoObject::updateMatrix() {
    if (m_bMatrixShouldUpdateRT) {
        m_transformationMatrix.makeRT(m_rot, m_pos);
        m_bMatrixShouldUpdatePosition = false;
        m_bMatrixShouldUpdateRT = false;
    } else if (m_bMatrixShouldUpdatePosition) {
        m_transformationMatrix[0, 3] = m_pos.x;
        m_transformationMatrix[1, 3] = m_pos.y;
        m_transformationMatrix[2, 3] = m_pos.z;
    }
    m_flags |= 4;
}

u16 GeoObject::id() const {
    return m_id;
}
const char* GeoObject::name() const {
    const ObjFlow* objflow = ObjectDirector::Instance()->objFlow();
    return objflow->attrs(objflow->slots(m_id))->name();
}
const char* GeoObject::resources() const {
    const ObjFlow* objflow = ObjectDirector::Instance()->objFlow();
    return objflow->attrs(objflow->slots(m_id))->resources();
}
u32 GeoObject::directorIndex() const {
    return m_directorIndex;
}
EGG::Vector3f GeoObject::position() const {
    return m_pos;
}
void GeoObject::setDirectorIndex(s32 val) {
    m_directorIndex = val;
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

void GeoObjectDrivable::registerBoxColUnit(f32 /*radius*/) {

}


GeoObjectKCL::GeoObjectKCL(System::MapdataGeoObj* pMapDataGeoObj) : GeoObjectDrivable(pMapDataGeoObj) {

}

f32 GeoObjectKCL::calcCollisionRadius() {
    return m_bboxHalfSideLength + colRadiusAdditionalLength();
}

void GeoObjectKCL::loadCollision() {
    char buffer[0x80];
    snprintf(buffer, 0x80, "%s.kcl", resources());
    m_objectColMgr = new ObjectColMgr(buffer);
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
