#pragma once

#include "game/field/obj/GeoObject.hh"
#include <egg/math/Matrix.hh>

namespace Field
{

class Mdush : public GeoObjectKCL {
public:
    Mdush(System::MapdataGeoObj* pMapDataGeoObj) : GeoObjectKCL(pMapDataGeoObj) {}
    virtual ~Mdush();

    EGG::Matrix34f &getUpdatedMatrix() override;
    f32 getScaleY() const override;
    f32 colRadiusAdditionalLength() const override;
};

} // namespace Field
