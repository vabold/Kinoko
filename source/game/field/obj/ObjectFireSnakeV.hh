#pragma once

#include "game/field/obj/ObjectFireSnake.hh"

namespace Kinoko::Field {

/// @brief Represents the bouncing fire snakes on Grumble Volcano
/// @details These fire snakes differ in behavior from the base class @ref ObjectFireSnake in that
/// their lifecycle is not controlled by @ref ObjectSunDS or any other @ref ObjectSniper. Instead,
/// these fire snakes repreatedly respawn from pipes.
class ObjectFireSnakeV final : public ObjectFireSnake {
public:
    ObjectFireSnakeV(const System::MapdataGeoObj &params);
    ~ObjectFireSnakeV() override;

    void init() override;
    void calc() override;

private:
    void calcSub();

    void enterDespawned();
    void enterFalling();
    void enterRest();

    void calcFalling();
    void calcHighBounce();
    void calcRest();
    void calcBounce();

    /// @brief The enter and calc functions for each @ref StateManager entry
    static constexpr std::array<StateManagerEntry, 6> STATE_ENTRIES = {{
            {StateEntry<ObjectFireSnakeV, &ObjectFireSnakeV::enterDespawned, nullptr>(0)},
            {StateEntry<ObjectFireSnakeV, &ObjectFireSnakeV::enterFalling,
                    &ObjectFireSnakeV::calcFalling>(1)},
            {StateEntry<ObjectFireSnakeV, nullptr, &ObjectFireSnakeV::calcHighBounce>(2)},
            {StateEntry<ObjectFireSnakeV, &ObjectFireSnakeV::enterRest,
                    &ObjectFireSnakeV::calcRest>(3)},
            {StateEntry<ObjectFireSnakeV, nullptr, &ObjectFireSnakeV::calcBounce>(4)},
            {StateEntry<ObjectFireSnakeV, nullptr, nullptr>(5)},
    }};

    const u16 m_delayFrame;    ///< Initial delay before state lifecycle starts
    const u16 m_cycleDuration; ///< Number of frames between two fire snake spawns
    const f32 m_distFromPipe;  ///< Distance from spawn position to the landing position
    const f32 m_fallSpeed;     ///< XZ speed of the fire snake after spawning and before landing

    static constexpr f32 RADIUS = 130.0f; ///< Collision radius of the fire snake
    static constexpr f32 GRAVITY = 3.0f;  ///< Gravity applied during spawn and the first bounce

    /// @brief How many frames the snake falls before starting to check against floor collision
    static constexpr u32 COL_CHECK_DELAY_FRAMES = 10;

    /// @brief How many frames the fire snake falls when spawning from the pipe
    static constexpr f32 SPAWN_FALL_DURATION = 140.0f;
};

} // namespace Kinoko::Field
