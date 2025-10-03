#pragma once

#include "game/field/obj/ObjectProjectile.hh"
#include "game/field/obj/ObjectProjectileLauncher.hh"

namespace Field {

/// @brief The base class for a manager object which is responsible for synchronizing a set of
/// projectiles and a projectile launcher.
/// @details In the base game, this is used for:
/// - DS Desert Hills: The sun and FireSnakes
/// - GBA Shy Guy Beach: The ship and the bombs
/// Every frame, this class checks if the launcher is ready to spawn a projectile, and it maps
/// between the launcher's current rail point and the corresponding projectile to launch.
class ObjectSniper : public ObjectCollidable {
public:
    ObjectSniper();
    ~ObjectSniper() override;

    void init() override;
    void calc() override;

    /// @addr{0x806D2900}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x806D28FC}
    void loadGraphics() override {}

    /// @addr{0x806D28F4}
    void createCollision() override {}

    /// @addr{0x806D28F8}
    void loadRail() override {}

protected:
    std::span<ObjectProjectile *> m_projectiles;
    ObjectProjectileLauncher *m_launcher; // The rDH sun or the RSGB ship
    std::span<s16> m_pointIdxs;
};

} // namespace Field
