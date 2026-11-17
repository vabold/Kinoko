#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko {

namespace Kart {

enum class Reaction;

} // namespace Kart

namespace Field {

/// @brief Represents the pipes on SNES Mario Circuit 3 and the piranha plants on N64 Mario Raceway
/// @details Pipes act like walls in time trials. The N64 Mario Raceway piranhas cause the player to
/// spinout.
/// @note An interesting oddity of pipe behavior is that they stay at the initial position defined
/// by the @ref System::MapdataGeoObj params until they are sent airborne, regardless of whether or
/// not they are colliding with the floor. Once a kart in a Mega Mushroom or Star sends them
/// airborne, they fly up and then fall below their initial position until they collide with a
/// floor.
class ObjectDokan final : public ObjectCollidable {
public:
    ObjectDokan(const System::MapdataGeoObj &params);
    ~ObjectDokan() override;

    void init() override;
    void calc() override;
    void calcCollisionTransform() override;
    [[nodiscard]] LoadFlags loadFlags() const override;
    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;

private:
    void calcPos();
    void calcFloor();
    void tryStartAirborne();

    bool m_isAirborne;        ///< Whether the pipe is currently mid-air
    EGG::Vector3f m_velocity; ///< The current velocity of the pipe
};

} // namespace Field

} // namespace Kinoko
