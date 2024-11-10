#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectDokan final : public ObjectCollidable {
public:
    ObjectDokan(const System::MapdataGeoObj &params);
    ~ObjectDokan() override;

    void init() override;
    void calc() override;
    [[nodiscard]] u32 loadFlags() const override;

    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, const EGG::Vector3f &hitDepth) override;

private:
    void calcFloor();

    bool m_b0;
    EGG::Vector3f m_velocity;
};

} // namespace Field
