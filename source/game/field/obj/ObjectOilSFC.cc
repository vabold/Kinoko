#include "ObjectOilSFC.hh"

#include "game/kart/KartObject.hh"
#include "game/kart/Reaction.hh"

namespace Kinoko::Field {

/// @addr{0x806DD934}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
ObjectOilSFC::ObjectOilSFC(const System::MapdataGeoObj &params) : ObjectCollidable(params) {}

/// @addr{0x806DD998}
/// @brief Default virtual destructor
ObjectOilSFC::~ObjectOilSFC() = default;

/// @addr{0x806DD9D8}
/// @copybrief ObjectCollidable::onCollision()
/// @param kartObj The kart object involved in the collision
/// @param reactionOnKart The reaction that should be applied to the kart upon collision
/// @param hitDepth The depth of the collision along each axis
/// @return @ref Kart::Reaction::SpinSomeSpeed if the player is on the ground, otherwise @ref
/// Kart::Reaction::None
/// @details Sets `hitDepth` to zero. Only applies the spinout if the kart is touching the ground,
/// indicated via @ref Kart::eStatus::TouchingGround in the kart's @ref Kart::Status.
Kart::Reaction ObjectOilSFC::onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
        Kart::Reaction /*reactionOnObj*/, EGG::Vector3f &hitDepth) {
    hitDepth = EGG::Vector3f::zero;
    bool touchingGround = kartObj->status().onBit(Kart::eStatus::TouchingGround);
    return touchingGround ? reactionOnKart : Kart::Reaction::None;
}

} // namespace Kinoko::Field
