#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectOilSFC final : public ObjectCollidable {
public:
    ObjectOilSFC(const System::MapdataGeoObj &params);
    ~ObjectOilSFC() override;

    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;
};

} // namespace Field
