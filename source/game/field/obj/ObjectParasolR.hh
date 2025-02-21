#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectParasolR : public ObjectCollidable {
public:
    ObjectParasolR(const System::MapdataGeoObj &params);
    ~ObjectParasolR() override;

    /// @addr{0x80779EF4}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }
};

} // namespace Field
