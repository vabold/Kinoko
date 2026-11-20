#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

/// @brief Represents the Toad's Factory stompers
/// @details The stompers are initially idle for @ref m_initDelay frames. After that, the
/// stompers cycle through a windup, lowering, hitting the floor, and raising back up to idle. When
/// the stompers hit the floor, the duration of the lowered state is determined based on the CHR
/// animation's framecount. To figure out where the floor is, @ref init() performs a floor collision
/// check.
/// @note If the press is positioned such that there is no floor beneath it, then @ref init() will
/// be stuck in an infinite loop, causing the race to never start.
class ObjectPress : public ObjectCollidable {
public:
    ObjectPress(const System::MapdataGeoObj &params);
    ~ObjectPress() override;

    void init() override;
    void calc() override;
    void loadAnims() override;
    void createCollision() override;
    [[nodiscard]] LoadFlags loadFlags() const override;
    [[nodiscard]] f32 getCollisionRadius() const override;

    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;

    virtual void calcRaised();

protected:
    /// @brief Describes the current phase of the press's animation cycle
    enum class State {
        Raised,   ///< Idle, waiting to stomp down
        WindUp,   ///< Rising up a bit before stomping down
        Lowering, ///< Stomping down
        Lowered,  ///< Contacting the floor
        Raising,  ///< Rising back up to the raised position
    };

    State m_state;     ///< The current phase of the press's animation cycle
    u32 m_windUpTimer; ///< Number of frames remaining in windup state

private:
    void initTimers();
    void calcWindUp();
    void calcLowering();
    void checkCollisionLowering();
    void calcLowered();
    void enterRaising();
    void calcRaising();

    bool m_startingRise;   ///< Used to delay state change by 1 frame
    u32 m_raisedTimer;     ///< Number of frames remaining in raised state
    u32 m_anmTimer;        ///< Remaining frames in the current press animation
    f32 m_loweringSpeed;   ///< Speed at which the press is lowering
    f32 m_raisedHeight;    ///< Height of the press when it is in the raised state
    bool m_startedLowered; ///< Used to induce crush effect even if it hit floor this frame

    const u32 m_initDelay;      ///< Initial delay before the first stomp of the race
    const u32 m_raisedDuration; ///< How long the press stays raised before stomping down again

    static constexpr f32 ANM_RATE = 2.0f; ///< Rate at which the press animation plays
};

/// @brief The stompers on the left and right side of the first TF factory room
/// @details These stompers don't stomp down until an @ref ObjectItemboxBlock tells them to.
class ObjectPressSenko final : public ObjectPress {
public:
    ObjectPressSenko(const System::MapdataGeoObj &params);
    ~ObjectPressSenko() override;

    [[nodiscard]] ObjectId id() const override;
    [[nodiscard]] LoadFlags loadFlags() const override;
    [[nodiscard]] const char *getResources() const override;
    [[nodiscard]] const char *getKclName() const override;

    void calcRaised() override;

    void beginStomp();

private:
    void startWindup();

    bool m_shouldStartWindup; ///< Signals that the press should begin stomping down next frame
};

} // namespace Kinoko::Field
