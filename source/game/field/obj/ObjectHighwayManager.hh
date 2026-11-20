#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

class ObjectCarTGE;

/// @brief Watcher class that checks if any car has squashed the player
/// @details Maintains a squash cooldown via @ref m_squashTimer, which is used by @ref ObjectCarTGE
/// objects to determine if the player can be squashed again.
class ObjectHighwayManager final : public ObjectCollidable {
public:
    ObjectHighwayManager();
    ~ObjectHighwayManager() override;

    void init() override;
    void calc() override;
    void loadGraphics() override;
    void createCollision() override;
    void loadRail() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

    /// @beginGetters

    /// @brief Gets the number of frames since the last squash occurred from any managed car/truck
    /// object, capped at @ref SQUASH_MAX
    /// @return The number of frames since the last squash occurred, @ref m_squashTimer
    [[nodiscard]] u32 squashTimer() const {
        return m_squashTimer;
    }

    /// @endGetters

private:
    void calcSquash();

    owning_span<ObjectCarTGE *> m_cars; ///< Pointers to all car/truck objects

    /// @brief Frames since last squash
    /// @note In the base game, this is normally an array of timers, one for each player. For
    /// Kinoko, we can simplify since we only implement time trials.
    u32 m_squashTimer;

    static constexpr u32 SQUASH_MAX = 600; ///< Squash timer cap
};

} // namespace Kinoko::Field
