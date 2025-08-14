#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectBreakable : public ObjectCollidable {
public:
    /// @addr{0x8076EBE0}
    ObjectBreakable(const System::MapdataGeoObj &params) : ObjectCollidable(params) {}

    /// @addr{0x8076EC28}
    ~ObjectBreakable() = default;

    /// @addr{0x807677E4}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x8076F0AC}
    Kart::Reaction onCollision(Kart::KartObject * /*kartObj*/, Kart::Reaction reactionOnKart,
            Kart::Reaction /*reactionOnObj*/, EGG::Vector3f & /*hitDepth*/) override {
        return reactionOnKart;
    }
};

} // namespace Field
