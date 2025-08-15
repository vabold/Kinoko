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
    virtual ~ObjectBreakable();

    void init();

    void calc() override;

    void enableCollision();

    virtual void respawn() = 0;

    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;

    /// @addr{0x807677E4}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

private:
    State m_breakableState;
    u32 m_respawnTimer;
    bool m_collisionEnabled;
};

} // namespace Field