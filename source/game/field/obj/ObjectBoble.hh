#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

/// @brief Rising and falling fireballs on GBA Bowser Castle 3
/// @details The fireballs follow a simple rail path with per-point velocities. This allows the
/// fireball to rise up, slow down, and fall more smoothly.
class ObjectBoble final : public ObjectCollidable {
public:
    ObjectBoble(const System::MapdataGeoObj &params);
    ~ObjectBoble() override;

    void init() override;
    void calc() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

private:
    void calcTangent();

    EGG::Vector3f m_curTangentDir; ///< Smoothed rail tangent direction at the current position
};

} // namespace Kinoko::Field
