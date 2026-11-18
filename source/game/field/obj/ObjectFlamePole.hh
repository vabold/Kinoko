#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

/// @brief A flamepole that erupts from a geyser (@ref ObjectFlamePoleFoot) on Bowser's Castle.
/// @details Though this object does not directly have logic to oscillate its position,
/// @ref ObjectFlamePoleFoot dynamically adjusts the pole's Y-scale. The pole will then resize its
/// GJK collision to reflect the scale change.
class ObjectFlamePole final : public ObjectCollidable {
public:
    /// @addr{0x8067E280}
    /// @brief Constructor
    /// @param params The parameters used to initialize the object
    /// @param pos The initial position of the flame pole
    /// @param rot The initial rotation of the flame pole
    /// @param scale The initial scale of the flame pole
    /// @details Constructs an @ref ObjectCollidable for the object named `FlamePoleEff`.
    ObjectFlamePole(const System::MapdataGeoObj &params, const EGG::Vector3f &pos,
            const EGG::Vector3f &rot, const EGG::Vector3f &scale)
        : ObjectCollidable("FlamePoleEff", pos, rot, scale) {}

    /// @addr{0x80681828}
    /// @brief Default virtual destructor
    ~ObjectFlamePole() override = default;

    /// @addr{0x8067E410}
    /// @copybrief ObjectBase::calc()
    /// @details If the geyser is erupting (identified via @ref m_isActive), resizes the pole's GJK
    /// collision to properly reflect that it is now erupting. The radius is computed as `70.0f`
    /// times the flamepole's Y-scale.
    void calc() override {
        constexpr f32 RADIUS = 70.0f;

        if (m_isActive) {
            resize(RADIUS * scale().y, 0.0f);
        }
    }

    /// @addr{0x80681820}
    /// @copybrief ObjectBase::loadFlags()
    /// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
    [[nodiscard]] LoadFlags loadFlags() const override {
        return LoadFlags(eLoadFlags::Calc);
    }

    /// @copybrief ObjectBase::getResources()
    /// @details Returns the resource name for the flame pole.
    /// @return The resource name for the flame pole, `FlamePole`.
    [[nodiscard]] const char *getResources() const override {
        return "FlamePole";
    }

    /// @brief Enables or disables GJK collision resizing
    /// @param isSet Whether to enable or disable GJK collision resizing.
    void setActive(bool isSet) {
        m_isActive = isSet;
    }

    static constexpr f32 HEIGHT = 384.0f; ///< Normal height of the pole

private:
    bool m_isActive; ///< Used to toggle collision resizing on and off
};

} // namespace Kinoko::Field
