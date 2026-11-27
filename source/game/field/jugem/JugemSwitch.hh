#pragma once

#include <Common.hh>

namespace Kinoko::Field {

/// @brief Base class which is used to represent scenarios that toggle Lakitu on or off
class JugemSwitch {
public:
    JugemSwitch();
    virtual ~JugemSwitch();

    virtual void init();

    /// @brief Updates the internal state of the switch, checking if Lakitu should be toggled on/off
    virtual void calc() = 0;

    /// @beginGetters

    /// @brief Exposes whether the switch is activated or not
    /// @return The value of @ref m_isOn
    [[nodiscard]] bool isOn() const {
        return m_isOn;
    }

    /// @endGetters

protected:
    bool m_isOn; ///< `true` if the switch has been toggled on, `false` otherwise
};

/// @brief Represents a switch that toggles Lakitu on when the player is driving backwards
/// @details Lakitu will be toggled on after the player accumulates 60 frames of driving the wrong
/// way.
class JugemSwitchReverse final : public JugemSwitch {
public:
    JugemSwitchReverse();
    ~JugemSwitchReverse() override;

    void init() override;
    void calc() override;

private:
    f32 m_activationPercent; ///< Lakitu is activated when this reaches `1.0f`
};

} // namespace Kinoko::Field
