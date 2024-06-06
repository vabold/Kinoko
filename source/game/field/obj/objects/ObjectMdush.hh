#pragma once

#include "game/field/obj/GeoObject.hh"

namespace Field {

/// @brief Dash ramp used in Mario Circuit
class ObjectMdush : public GeoObjectKCL {
public:
    ObjectMdush(System::MapdataGeoObj *pMapDataGeoObj);
    ~ObjectMdush() override;

    EGG::Matrix34f &getUpdatedMatrix() override;
    f32 getScaleY() const override;
    f32 colRadiusAdditionalLength() const override;
};

} // namespace Field
