#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectBreakable : public ObjectCollidable {
public:
    enum class State {
        UNINITIALIZED,
        ACTIVE,
        BROKEN,
        RESPAWNING,
    };

    ObjectBreakable(const System::MapdataGeoObj &params);
    ~ObjectBreakable() override;

    /// @addr{0x8076EC68}
    void init() {
        enableCollision();
    }

    void calc() override;
    
    /// @addr{0x807677E4}
    [[nodiscard]] u32 ObjectBreakable::loadFlags() const override {
        return 1;
    }

    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;

    // @addr{0x8076ED70}
    void enableCollision() {
        m_state = State::ACTIVE;
        m_collisionEnabled = true;
    }

    /// @addr{0x807677E0}
    virtual void onRespawn() {
        return;
    }

private:
    State m_state;
    u32 m_respawnTimer;
    bool m_collisionEnabled;
};

} // namespace Field