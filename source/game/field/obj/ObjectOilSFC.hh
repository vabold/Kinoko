#pragma once

#include "game/field/obj/ObjectBase.hh"

namespace Field {

class ObjectOilSFC : public ObjectBase {
public:
    ObjectOilSFC(const System::MapdataGeoObj &params);
    ~ObjectOilSFC() override;
};

} // namespace Field
