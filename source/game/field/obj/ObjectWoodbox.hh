#pragma once

#include "game/field/obj/ObjectBreakable.hh"

namespace Field {

class ObjectWoodbox final : public ObjectBreakable {
public:
    ObjectWoodbox(const System::MapdataGeoObj &params);
    ~ObjectWoodbox() override;

    /// @addr{0x8077ED7C}
    [[nodiscard]] const char *getKclName() const override {
        return "ironbox"; // woodbox when not in TTs
    }

    void calcCollisionTransform() override;
};

} // namespace Field
