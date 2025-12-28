#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

/// @brief The Bowser statues on N64 Bowser's Castle.
/// @details The big statue by the first turn has collision while the mini ones do not.
class ObjectKoopaFigure64 final : public ObjectCollidable {
public:
    ObjectKoopaFigure64(const System::MapdataGeoObj &params);
    ~ObjectKoopaFigure64() override;

    void init() override;
    void calc() override;

    /// @addr{0x806C0854}
    /// @details The game defines this in a GeoObjectSmoke base class, but we don't implement it.
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x806DB154}
    void createCollision() override {
        if (m_isBigStatue) {
            ObjectCollidable::createCollision();
        }
    }

    void calcCollisionTransform() override;

    /// @addr{0x8068179C}
    Kart::Reaction onCollision(Kart::KartObject * /*kartObj*/, Kart::Reaction reactionOnKart,
            Kart::Reaction /*reactionOnObj*/, EGG::Vector3f &hitDepth) override {
        hitDepth.setZero();
        return reactionOnKart;
    }

private:
    const bool m_isBigStatue; ///< Differentiates the first rBC turn statue from tiny statues
    const s32 m_startDelay;
    u32 m_cycleFrame;

    static constexpr u32 FIRE_DURATION = 300; ///< How long the statue shoots fire for in a cycle
    static constexpr u32 COOLDOWN_DURATION = 180; ///< How long fire is disabled for in a cycle
    static constexpr u32 CYCLE_DURATION = FIRE_DURATION + COOLDOWN_DURATION;
};

} // namespace Field
