#pragma once

#include "game/field/obj/ObjectProjectileLauncher.hh"

namespace Kinoko::Field {

/// @brief The ship on GBA Shy Guy Beach that shoots cannonballs
/// @details The ship moves along a rail and bobs up and down, occasionally shooting cannonballs.
/// The synchronization between the ship and the cannonballs is managed by @ref
/// ObjectHeyhoShipManager.
class ObjectHeyhoShip final : public ObjectProjectileLauncher {
public:
    ObjectHeyhoShip(const System::MapdataGeoObj &params);
    ~ObjectHeyhoShip() override;

    void init() override;
    void calc() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

    [[nodiscard]] s16 launchPointIdx() override;

    const EGG::Vector3f &initRailDir(u16 idx);

private:
    void calcPos();

    const f32 m_yAmplitude;      ///< How much the ship bobs up and down
    u32 m_frame;                 ///< Number of frames since the ship was initialized
    u32 m_framesSinceLastLaunch; ///< Number of frames since the last cannonball was launched
};

} // namespace Kinoko::Field
