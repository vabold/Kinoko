#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

/// @brief Oscillating cars on Coconut Mall
/// @details These cars move along a rail and have a trapezoidal motion profile; they accelerate to
/// a set velocity, drive at that velocity for a set amount of time, and then decelerate to a stop.
class ObjectCarA final : public ObjectCollidable, private StateManager {
public:
    ObjectCarA(const System::MapdataGeoObj &params);
    ~ObjectCarA() override;

    void init() override;
    void calc() override;
    void createCollision() override;
    void calcCollisionTransform() override;
    [[nodiscard]] LoadFlags loadFlags() const override;
    Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth) override;

private:
    /// @brief Describes the trapezoidal motion profile for the car
    enum class MotionState {
        Accelerating = 0, ///< The car is speeding up
        Cruising = 1,     ///< The car is maintaining a constant speed
        Decelerating = 2, ///< The car is slowing down
    };

    void calcRail();
    void calcPos();
    void enterStop();
    void enterCruising();
    void calcStop();
    void calcAccel();
    void calcCruising();

    const f32 m_finalSpeed;      ///< Target speed after accelerating
    const f32 m_accel;           ///< Acceleration and deceleration rate
    const u32 m_stopDuration;    ///< How long to spend at 0 velocity before accelerating.
    f32 m_cruiseTime;            ///< How long to spend at cruising speed before decelerating.
    EGG::Vector3f m_currTangent; ///< The current forward vector of the car
    EGG::Vector3f m_currUp; ///< Smoothed up vector (always @ref EGG::Vector3f::ey in time trials)
    f32 m_currVel;          ///< Current velocity of the car this frame
    MotionState m_motionState; ///< The current motion state of the car
    bool m_changingDir;        ///< Triggers the deceleration-to-stop logic

    /// @brief The enter and calc functions for each @ref StateManager entry
    static constexpr std::array<StateManagerEntry, 3> STATE_ENTRIES = {{
            {StateEntry<ObjectCarA, &ObjectCarA::enterStop, &ObjectCarA::calcStop>(0)},
            {StateEntry<ObjectCarA, nullptr, &ObjectCarA::calcAccel>(1)},
            {StateEntry<ObjectCarA, &ObjectCarA::enterCruising, &ObjectCarA::calcCruising>(2)},
    }};
};

} // namespace Kinoko::Field
