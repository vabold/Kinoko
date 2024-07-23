#pragma once

#include "game/field/obj/ObjectBase.hh"

namespace Field {

class ObjectNoImpl : public ObjectBase {
public:
    ObjectNoImpl(const System::MapdataGeoObj &params);
    ~ObjectNoImpl() override;
};

} // namespace Field
