#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

/// @brief The Bowser statues on N64 Bowser's Castle
/// @details The big statue by the first turn has collision while the small ones do not.
/// The big statue will breathe fire for a set duration, then stop for a cooldown period.
class ObjectKoopaFigure64 final : public ObjectCollidable {
public:
    ObjectKoopaFigure64(const System::MapdataGeoObj &params);
    ~ObjectKoopaFigure64() override;

    void init() override;
    void calc() override;

    void createCollision() override;
    void calcCollisionTransform() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;

private:
    const bool m_isBigStatue; ///< Differentiates the first rBC turn statue from small statues
    const u32 m_startDelay;   ///< Frame delay before the statue starts shooting fire
    u32 m_cycleFrame;         ///< Tracks the current frame in the fire/cooldown cycle

    static constexpr u32 FIRE_DURATION = 300; ///< How long the statue shoots fire for in a cycle
    static constexpr u32 COOLDOWN_DURATION = 180; ///< How long fire is disabled for in a cycle

    /// @brief Framecount of the entire cycle duration, including fire and cooldown
    static constexpr u32 CYCLE_DURATION = FIRE_DURATION + COOLDOWN_DURATION;
};

} // namespace Kinoko::Field
