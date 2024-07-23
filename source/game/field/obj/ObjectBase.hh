#pragma once

#include "game/field/obj/ObjectId.hh"

#include "game/system/map/MapdataGeoObj.hh"

namespace Field {

class ObjectBase {
public:
    ObjectBase(const System::MapdataGeoObj &params);
    virtual ~ObjectBase();

protected:
    ObjectId m_id;
    EGG::Vector3f m_pos;
    EGG::Vector3f m_rot;
    EGG::Vector3f m_scale;
};

} // namespace Field
