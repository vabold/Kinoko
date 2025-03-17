#include "ObjectOilSFC.hh"

#include "game/kart/KartCollide.hh"
#include "game/kart/KartObject.hh"
#include "game/kart/KartState.hh"

namespace Field {

/// @addr{0x806DD934}
ObjectOilSFC::ObjectOilSFC(const System::MapdataGeoObj &params) : ObjectCollidable(params) {}

/// @addr{0x806DD998}
ObjectOilSFC::~ObjectOilSFC() = default;

Kart::Reaction ObjectOilSFC::onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
        Kart::Reaction /*reactionOnObj*/, EGG::Vector3f &hitDepth) {
    hitDepth = EGG::Vector3f::zero;
    return kartObj->state()->isTouchingGround() ? reactionOnKart : Kart::Reaction::None;
}

} // namespace Field
