#pragma once

#include "game/field/obj/ObjectBelt.hh"

namespace Kinoko::Field {

/// @brief The conveyors in the first section of Toad's Factory.
/// @details Consists of two conveyors that move in opposite directions with variant 0 moving in
/// the negative direction and variant 1 moving in the positive direction.
class ObjectBeltCrossing final : public ObjectBelt {
public:
    /// @addr{0x807FC764}
    /// @copybrief ObjectBelt::ObjectBelt(const System::MapdataGeoObj &)
    /// @param params The parameters used to initialize the object
    /// @details Initializes road velocity to `28.0f`.
    ObjectBeltCrossing(const System::MapdataGeoObj &params) : ObjectBelt(params) {
        m_roadVel = 28.0f;
    }

    /// @addr{0x807FD8A8}
    /// @brief Default virtual destructor
    ~ObjectBeltCrossing() override = default;

private:
    /// @addr{0x807FC7D8}
    /// @brief Calculates the conveyor belt's velocity based on its variant
    /// @param variant The variant of the conveyor belt
    /// @return The velocity of the conveyor belt with the given variant
    /// @details The magnitude of velocity is @ref m_roadVel. The direction is determined by the
    /// variant, with variant 0 moving to the west and variant 1 moving to the east.
    [[nodiscard]] EGG::Vector3f calcRoadVelocity(u32 variant, const EGG::Vector3f & /*pos*/,
            u32 /*t*/) const override {
        switch (variant) {
        case 0:
            return -EGG::Vector3f::ex * m_roadVel;
        case 1:
            return EGG::Vector3f::ex * m_roadVel;
        default:
            return EGG::Vector3f::zero;
        }
    }

    /// @addr{0x807FC874}
    /// @copybrief ObjectBelt::isMoving()
    /// @param variant The variant of the conveyor belt
    /// @return `true` if the variant is 0 or 1, `false` otherwise (does not occur in the base game)
    /// @note In the base game, this function also takes in the player's current position, but it is
    /// unused and thus removed for Kinoko.
    [[nodiscard]] bool isMoving(u32 variant) const override {
        return variant == 0 || variant == 1;
    }
};

} // namespace Kinoko::Field
