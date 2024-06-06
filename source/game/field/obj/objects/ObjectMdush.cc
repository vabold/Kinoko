#include "game/field/obj/objects/ObjectMdush.hh"

namespace Field {

ObjectMdush::ObjectMdush(System::MapdataGeoObj *pMapDataGeoObj) : GeoObjectKCL(pMapDataGeoObj) {}

ObjectMdush::~ObjectMdush() = default;

EGG::Matrix34f &ObjectMdush::getUpdatedMatrix() {
    updateMatrix();
    return m_transformationMatrix;
}
f32 ObjectMdush::getScaleY() const {
    return m_scale.y;
}

f32 ObjectMdush::colRadiusAdditionalLength() const {
    return 0.0f;
}

} // namespace Field
