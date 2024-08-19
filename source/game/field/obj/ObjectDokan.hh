#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectDokan final : public ObjectCollidable {
public:
    ObjectDokan(const System::MapdataGeoObj &params);
    ~ObjectDokan() override;
};

} // namespace Field
