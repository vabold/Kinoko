#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectBreakable : public ObjectCollidable {
public:
    /// @addr{0x8076EBE0}
    ObjectBreakable(const System::MapdataGeoObj &params);

    /// @addr{0x8076EC28}
    virtual ~ObjectBreakable();

    /// @addr{0x8076EC68}
    void load() override;

    /// @addr{0x8076ED1C}
    void calc() override;

    /// @addr{0x807677E4}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;

private:
    bool m_collisionEnabled;
};

} // namespace Field