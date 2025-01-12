#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectParasolR : public ObjectCollidable {
public:
    ObjectParasolR(const System::MapdataGeoObj &params);
    ~ObjectParasolR() override;

    [[nodiscard]] u32 loadFlags() const override;
};

} // namespace Field
