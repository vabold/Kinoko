#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectOilSFC final : public ObjectCollidable {
public:
    ObjectOilSFC(const System::MapdataGeoObj &params);
    ~ObjectOilSFC() override;
};

} // namespace Field
