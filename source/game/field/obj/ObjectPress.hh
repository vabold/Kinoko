#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

/// @brief Represents the Toad's Factory stompers.
/// @details This is also a base class for item boxes that get squished by stompers.
class ObjectPress : public ObjectCollidable {
public:
    ObjectPress(const System::MapdataGeoObj &params);
    ~ObjectPress() override;

    void init() override;
    void calc() override;

    /// @addr{0x807787E8}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    void loadAnims() override;
    void createCollision() override;

    [[nodiscard]] f32 getCollisionRadius() const override {
        return 700.0f;
    }

    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;

    virtual void calcRaised();

private:
    enum class State {
        Raised,
        WindUp,   ///< Rising up a bit before stomping down
        Lowering, ///< Stomping down
        Lowered,  ///< Contacting the floor
        Raising,
    };

    void calcWindUp();
    void calcLowering();
    void checkCollisionLowering();
    void calcLowered();
    void calcRaising();

    State m_state;
    bool m_startingRise; ///< Used to delay state change by 1 frame
    u32 m_windUpTimer;   ///< Number of frames remaining in windup state
    u32 m_raisedTimer;   ///< Number of frames remaining in raised state
    u32 m_anmDuration;
    f32 m_loweringVelocity;
    f32 m_raisedHeight;
    f32 m_loweredHeight;
    bool m_startedLowered; ///< Used to induce crush effect even if it hit floor this frame

    static constexpr f32 ANM_RATE = 2.0f;
};

} // namespace Field
