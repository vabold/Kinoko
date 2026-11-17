#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {
class ObjectDossunTsuibiHolder;

/// @brief Base class for the various different Thwomp types in the game.
/// @details Thwomps are constructed by creating an instance of @ref ObjectDossunc where the second
/// param setting specifies the Thwomp "mode". On initialization, the game pre-computes how many
/// frames it takes the Thwomp to stomp down and hit the floor.
/// @note If a Thwomp is positioned such that there is no floor beneath it, then
/// the init function will be stuck in an infinite loop, causing the race to never start.
class ObjectDossun : public ObjectCollidable {
    friend ObjectDossunTsuibiHolder;

public:
    ObjectDossun(const System::MapdataGeoObj &params);
    ~ObjectDossun() override;

    void init() override;
    void calcCollisionTransform() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;

protected:
    /// @brief Represents the current movement state of the Thwomp in terms of its stomp
    enum class AnmState {
        Still = 0,      ///< The Thwomp is stationary and not moving
        BeforeFall = 1, ///< The quick upwards motion before slamming down
        Falling = 2,    ///< Slamming down
        Grounded = 3,   ///< Contacting the floor
        Rising = 4,     ///< Resetting
    };

    /// @brief Discerns whether the Thwomp is currently stomping or not
    enum class StompState {
        Inactive = 0, ///< The Thwomp is not stomping and is either still or rising
        Active = 1,   ///< The Thwomp is stomping or is quickly rising before stomping
    };

    virtual void startStill();

    void calcStomp();

    StompState m_stompState; ///< Identifies whether the Thwomp is currently stomping or not
    AnmState m_anmState;     ///< The current movement state of the Thwomp in terms of its stomp
    s32 m_beforeFallTimer;   ///< Number of frames until the Thwomp will stomp down
    s32 m_stillTimer;        ///< Number of frames the Thwomp will remain stationary (or shakes)
    s32 m_groundedTimer;     ///< Number of frames the Thwomp remains on the ground
    s32 m_shakePhase;    ///< Causes the Thwomp's position to shake at the end of the "still" state
    f32 m_vel;           ///< Vector3f in the base game, but only the y-component is used
    f32 m_initialPosY;   ///< Starting position. Thwomps reset to this position after a stomp
    u32 m_fullDuration;  ///< Framecount of an entire animation loop
    s32 m_stompDuration; ///< Framecount of the Thwomp's stomp downwards and reset animation
    f32 m_currYaw;       ///< Rotation around the y-axis
    bool m_touchingGround; ///< Set when the Thwomp collides with the floor

    /// @brief The total number of frames the thwomp rises before crashing down
    static constexpr u32 BEFORE_FALL_DURATION = 10;

private:
    void initState();
    void calcBeforeFall();
    void calcFalling();
    void startGrounded();
    void calcGrounded();
    void calcRising();
    void checkFloorCollision();

    /// @brief The number of frames the Thwomp remains on the ground after a stomp
    static constexpr u32 GROUND_DURATION = 60;

    /// @brief The acceleration of the Thwomp while stomping downwards
    static constexpr u32 STOMP_ACCEL = 17.0f;

    /// @brief The radius of the Thwomp's collision sphere while stomping downwards
    static constexpr f32 STOMP_RADIUS = 20.0f;

    /// @brief The offset of the Thwomp's collision sphere while stomping downwards
    static constexpr EGG::Vector3f STOMP_POS_OFFSET = EGG::Vector3f(0.0f, STOMP_RADIUS, 0.0f);

    /// @brief The velocity of the Thwomp while rising to its initial position after a stomp
    static constexpr f32 RISING_VEL = 10.0f;
};

} // namespace Kinoko::Field
