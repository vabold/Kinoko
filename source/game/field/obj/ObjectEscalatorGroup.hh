#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectEscalator;

/// @brief Represents a group of two escalators and the Pianta dancing in the middle.
class ObjectEscalatorGroup final : public ObjectCollidable {
public:
    ObjectEscalatorGroup(const System::MapdataGeoObj &params);
    ~ObjectEscalatorGroup() override;

    /// @brief Not overridden in the base game, but it's effectively a nop because the corresponding
    /// kcl (monte_a) doesn't have any primitive collision.
    void createCollision() override {}

private:
    ObjectEscalator *m_rightEscalator;
    ObjectEscalator *m_leftEscalator;
};

} // namespace Field
