#pragma once

#include "game/field/StateManager.hh"

#include <egg/math/Matrix.hh>

namespace Kinoko {

namespace Kart {

class KartObject;

} // namespace Kart

namespace Field {

class JugemMove;
class JugemSwitch;

/// @brief Manages interpolation keyframes for the Lakitu's position when ascending and descending
class JugemInterp {
public:
    JugemInterp(u32 count);
    ~JugemInterp();

    void initKeyframes(f32 startRate, f32 endRate, f32 tDelta);
    bool calc(f32 tDelta);
    void setStartKeyframe(f32 rate);
    void addKeyframe(f32 rate, f32 tDelta);
    [[nodiscard]] f32 calcInterpRate(f32 time) const;

    /// @beginGetters

    /// @brief Gets the current interpolation rate
    /// @return The current interpolation rate, @ref m_rate
    [[nodiscard]] f32 rate() const {
        return m_rate;
    }

    /// @endGetters

private:
    /// @brief Represents a single keyframe in the interpolation sequence
    struct InterpKeyframe {
        /// @brief Constructor
        /// @details Sets @ref m_interpRate and @ref m_time to zero.
        InterpKeyframe() : m_interpRate(0.0f), m_time(0.0f) {}

        f32 m_interpRate; ///< The interpolation rate at this keyframe
        f32 m_time;       ///< The time at which this keyframe occurs in the interpolation sequence
    };

    [[nodiscard]] static f32 SmoothLerp(f32 a, f32 b, f32 t);

    owning_span<InterpKeyframe> m_keyframes; ///< Array of keyframes for the interpolation sequence
    u32 m_numKeyframes;                      ///< Number of keyframes in the interpolation sequence
    f32 m_time;                              ///< Current time in the interpolation sequence
    f32 m_rate;                              ///< Current interpolation rate
};

/// @brief Represents a single Lakitu and houses some state management members
/// @details On each frame, evaluates switches to determine if Lakitu should be toggled on/off. When
/// Lakitu is idle and a switch is toggled, transitions to the appropriate state and evaluates
/// state-specific logic. For Kinoko, we only need to implement the reverse switch, which toggles
/// Lakitu on when the player is driving backwards for 60 frames.
class JugemUnit : private StateManager {
public:
    JugemUnit(const Kart::KartObject *kartObj);
    ~JugemUnit() override;

    void init();
    void createSwitchRace();
    void calc();

private:
    /// @brief Describes the current state of the Lakitu unit, which determines how it behaves and
    /// responds to switches
    enum class State {
        Away = 0,       ///< Lakitu is idle and not visible
        Descending = 1, ///< Lakitu is descending to the player
        Stay = 2,       ///< Lakitu is active and is following the player
        Ascending = 3,  ///< Lakitu is ascending and leaving the player
    };

    void enterIdle();
    void enterReverse();

    void calcIdle();
    void calcReverse();

    void calcSwitches();
    void setPosFromTransform(const EGG::Matrix34f &mat);
    [[nodiscard]] EGG::Vector3f transformLocalToWorldUpright(const EGG::Vector3f &v) const;
    void calcCollision();

    const Kart::KartObject *m_kartObj; ///< Pointer to the player's kart object
    EGG::Vector3f m_pos;               ///< Lakitu's position in world space
    State m_state;                     ///< The current state of the Lakitu unit
    u32 m_ascendTimer;                 ///< How long Lakitu has been disappearing/ascending for
    JugemSwitch *m_switchReverse; ///< Pointer to the switch that toggles when driving backwards
    JugemMove *m_move;            ///< Pointer to the Lakitu movement controller
    JugemInterp *m_interp;        ///< Pointer to the keyframe interpolation controller

    /// @brief The enter and calc functions for each @ref StateManager entry
    static constexpr std::array<StateManagerEntry, 2> STATE_ENTRIES = {{
            StateEntry<JugemUnit, &JugemUnit::enterIdle, &JugemUnit::calcIdle>(0),
            StateEntry<JugemUnit, &JugemUnit::enterReverse, &JugemUnit::calcReverse>(1),
    }};
};

} // namespace Field

} // namespace Kinoko
