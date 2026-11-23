#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

/// @brief Abstract class that moves along its own rail and throws projectiles.
/// @note This class must be constructed before the projectiles, since @ref ObjectFireSnake only
/// registers itself if the managed object vector is non-empty.
class ObjectProjectileLauncher : public ObjectCollidable {
public:
    /// @addr{Inlined in 0x806DDDD8} @addr{Inlined in 0x806D18FC}
    /// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
    /// @details Registers the object to the list of managed objects in @ref ObjectDirector.
    ObjectProjectileLauncher(const System::MapdataGeoObj &params) : ObjectCollidable(params) {
        registerManagedObject();
    }

    /// @addr{0x806D1998}
    /// @brief Default virtual destructor
    ~ObjectProjectileLauncher() override = default;

    /// @copybrief ObjectBase::createCollision()
    /// @details Not overridden in the base game, but has collision mode 0 so no collision is
    /// created.
    void createCollision() override {}

    /// @brief Used by @ref ObjectSniper to check which rail index an object should be thrown from
    /// @return The index of the rail point from which to launch the projectile, or `-1` if no
    /// projectile should be launched this frame.
    [[nodiscard]] virtual s16 launchPointIdx() const = 0;
};

} // namespace Kinoko::Field
