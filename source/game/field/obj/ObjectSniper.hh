#pragma once

#include <Types.hh>

#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

class ObjectProjectile;
class ObjectProjectileLauncher;

/// @brief The base class for a manager object which is responsible for synchronizing a set of
/// projectiles and a projectile launcher.
/// @details Every frame, this class checks if the launcher is ready to spawn a projectile, and it
/// maps between the launcher's current rail point and the corresponding projectile to launch.
/// Derived classes are expected to populate @ref m_projectiles and @ref m_launcher, and allocate
/// @ref m_pointIdxs to the launcher's rail point count.
class ObjectSniper : public ObjectCollidable {
public:
    ObjectSniper();
    ~ObjectSniper() override;

    void init() override;
    void calc() override;
    void loadGraphics() override;
    void createCollision() override;
    void loadRail() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

protected:
    owning_span<ObjectProjectile *> m_projectiles; ///< Pointers to the managed projectiles
    ObjectProjectileLauncher *m_launcher;          ///< The object launching the projectiles

    /// @brief Maps each launcher rail point index to the index of the projectile in @ref
    /// m_projectiles to launch there, or `-1` if none
    owning_span<s16> m_pointIdxs;
};

} // namespace Kinoko::Field
