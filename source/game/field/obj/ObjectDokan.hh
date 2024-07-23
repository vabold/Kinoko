#pragma once

#include "game/field/obj/ObjectBase.hh"

namespace Field {

class ObjectDokan : public ObjectBase {
public:
    ObjectDokan(const System::MapdataGeoObj &params);
    ~ObjectDokan() override;
};

} // namespace Field
