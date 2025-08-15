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

    void init() override;
    void calc() override;
    [[nodiscard]] u32 loadFlags() const override;
    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;

    void enableCollision();
    virtual void respawnCallback();

private:
    State m_state;
    u32 m_respawnTimer;
    bool m_collisionEnabled;
};

} // namespace Field