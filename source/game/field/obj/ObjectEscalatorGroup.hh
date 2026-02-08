#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectEscalator;

/// @brief Represents a group of two escalators and the Pianta dancing in the middle.
class ObjectEscalatorGroup final : public ObjectCollidable {
public:
    ObjectEscalatorGroup(const System::MapdataGeoObj &params);
    ~ObjectEscalatorGroup() override;

    /// @addr{0x80802D18}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x80802D00}
    [[nodiscard]] const char *getResources() const override {
        return "monte_a";
    }

    /// @addr{0x80802D0C}
    [[nodiscard]] const char *getKclName() const override {
        return "monte_a";
    }

    /// @brief Not overridden in the base game, but it's effectively a nop because the corresponding
    /// kcl (monte_a) doesn't have any primitive collision.
    void createCollision() override {}

private:
    ObjectEscalator *m_rightEscalator;
    ObjectEscalator *m_leftEscalator;
};

} // namespace Field
