#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

/// @brief Represents the moving crabs on GBA Shy Guy Beach
/// @details Crabs move back-and-forth along a rail with constant speed, periodically stopping when
/// reaching the end of the rail or a rail segment.
class ObjectCrab final : public ObjectCollidable {
public:
    ObjectCrab(const System::MapdataGeoObj &params);
    ~ObjectCrab() override;

    void init() override;
    void calc() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

private:
    /// @brief Describes the current behavior of the crab
    enum class State {
        Walking = 0,     ///< The crab is walking sideways
        Still = 1,       ///< The crab is stationary
        Deactivated = 2, ///< The crab is despawned after being hit (unused in Kinoko)
        Resurfacing = 3, ///< The crab is respawning after being hit (unused in Kinoko)
    };

    /// @brief Describes the phase of the current state
    enum class StatePhase {
        Start = 0,  ///< The current state has just started
        Middle = 1, ///< In the middle of the current state
        End = 2,    ///< The current state is ending
    };

    /// @brief Describes the crab's state during a pause
    enum class StateResult {
        Walking = 0,      ///< The crab is not paused
        Middle = 1,       ///< The crab is in the middle of a pause
        BeginWalking = 2, ///< The crab is about to resume walking
    };

    void setState(State state);
    bool calcRail();
    StateResult calcState();
    void calcCurRot(const EGG::Vector3f &rot);
    void calcTransMat(const EGG::Vector3f &rot);

    const f32 m_vel;         ///< The speed at which the crab moves along the rail
    u32 m_stillDuration;     ///< Frames the crab will remain still at the current rail point
    u32 m_stillFrame;        ///< Frames the crab has been still at the current rail point
    bool m_still;            ///< Whether the crab is currently paused at the current rail point
    const bool m_backwards;  ///< Flips the crab's rotation
    EGG::Vector3f m_curRot;  ///< The current rotation of the crab
    State m_state;           ///< The current state of the crab: walking or still
    StatePhase m_statePhase; ///< Phase of the current state
    bool m_introCalc;        ///< Enforces only one calc call during the race intro timer

    /// @brief The initial rotation of the crab
    static constexpr EGG::Vector3f INIT_ROT = EGG::Vector3f(0.0f, HALF_PI, 0.0f);
};

} // namespace Kinoko::Field
