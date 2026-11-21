#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

/// @brief Represents the oil slick on SNES Mario Circuit 3
/// @details If the player drives over the oil slick, they will lose traction and spin out.
class ObjectOilSFC final : public ObjectCollidable {
public:
    ObjectOilSFC(const System::MapdataGeoObj &params);
    ~ObjectOilSFC() override;

    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;
};

} // namespace Kinoko::Field
