#pragma once

#include "game/field/obj/GeoObject.hh"

namespace Field
{

class Mdush : public GeoObjectKCL {
public:
    Mdush(System::MapdataGeoObj* pMapDataGeoObj) : GeoObjectKCL(pMapDataGeoObj) {}
};

} // namespace Field
