#pragma once

#include "game/field/obj/ObjectDossun.hh"

namespace Kinoko::Field {

/// @brief Individual Thwomps that move along a rail and stomp
class ObjectDossunSyuukai final : public ObjectDossun {
public:
    ObjectDossunSyuukai(const System::MapdataGeoObj &params);
    ~ObjectDossunSyuukai() override;

    void init() override;
    void calc() override;

private:
    /// @brief Describes the current motion state of the Thwomp
    enum class State {
        Moving = 0,               ///< Moving along the rail
        RotatingBeforeStomp = 1,  ///< Stationary and rotating
        Stomping = 2,             ///< Stomping down
        RotatingBeforeMoving = 3, ///< Stationary and rotating to face rail direction before moving
    };

    void startStill() override;

    void calcMoving();
    void calcRotating();

    State m_state;   ///< Current motion of the Thwomp
    f32 m_initYaw;   ///< Initial rotation about the Y-axis
    bool m_rotating; ///< Whether the Thwomp is currently rotating
};

} // namespace Kinoko::Field
