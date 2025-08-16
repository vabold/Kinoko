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

    void calc() override;
    
    /// @addr{0x807677E4}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;

    // @addr{0x8076ED70}
    void enableCollision() {
        m_state = State::ACTIVE;
        m_collisionEnabled = true;
    }

    /// @addr{0x8076EDD4}
    virtual void onTimerFinish() {
        enableCollision();
    }

    /// @addr{0x8076EDE4}
    virtual void onBreak(Kart::KartObject *kartObj) {
        m_state = State::Broken;
        m_collisionEnabled = false;
    }

    /// @addr{0x807677E0}
    virtual void onRespawn() {}

private:
    State m_state;
    u32 m_respawnTimer;
    bool m_collisionEnabled;
};

} // namespace Field
