#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectBreakable : public ObjectCollidable {
public:
    enum BreakableState : u32 {
        UNINITIALIZED = 0,
        ACTIVE = 1,
        BROKEN = 2,
        HIT = 3,
    };

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

    // @addr{0x8076ED70}
    void enableCollision();

    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;

private:
    BreakableState m_breakableState;
    u32 m_respawnTimer;
    bool m_collisionEnabled;
};

} // namespace Field