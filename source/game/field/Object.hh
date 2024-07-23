#pragma once

#include "game/field/ObjectId.hh"

#include "game/system/map/MapdataGeoObj.hh"

namespace Field {

class Object {
public:
    Object(const System::MapdataGeoObj &params);

protected:
    ObjectId m_id;
    EGG::Vector3f m_pos;
    EGG::Vector3f m_rot;
    EGG::Vector3f m_scale;
};

} // namespace Field
