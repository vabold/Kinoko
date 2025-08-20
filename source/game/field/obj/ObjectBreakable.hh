#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectBreakable : public ObjectCollidable {
public:
    ObjectBreakable(const System::MapdataGeoObj &params) : ObjectCollidable(params) {}
    ~ObjectBreakable() = default;

    /// @addr{0x8076EC68}
    void init() override {
        enableCollision();
    }

    /// @addr{0x807677E4}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    Kart::Reaction onCollision(Kart::KartObject * /*kartObj*/, Kart::Reaction reactionOnKart,
            Kart::Reaction /*reactionOnObj*/, EGG::Vector3f & /*hitDepth*/) override {
        return reactionOnKart;
    }
};

} // namespace Field
