#pragma once

#include "game/field/obj/GeoObject.hh"

namespace Field {

/// @brief Dash ramp used in Mario Circuit
class Mdush : public GeoObjectKCL {
public:
    Mdush(System::MapdataGeoObj *pMapDataGeoObj);
    ~Mdush() override;

    EGG::Matrix34f &getUpdatedMatrix() override;
    f32 getScaleY() const override;
    f32 colRadiusAdditionalLength() const override;
};

} // namespace Field
