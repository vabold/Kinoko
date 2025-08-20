#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectBreakable : public ObjectCollidable {
public:
    enum class State {
        Uninitialized,
        Active,
        Broken,
        Respawning,
    };

    ObjectBreakable(const System::MapdataGeoObj &params);
    ~ObjectBreakable() override;

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

    // @addr{0x8076ED70}
    virtual void enableCollision() override {
        ObjectCollidable::enableCollision();
        m_state = State::Active;
        m_collisionEnabled = true;
    }

protected:
    State m_state;
    bool m_collisionEnabled;
};

} // namespace Field
