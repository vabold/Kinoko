#pragma once

#include "game/field/obj/ObjectSniper.hh"

namespace Kinoko::Field {

/// @brief Manages the synchronization between the @ref ObjectHeyhoBall and the @ref ObjectHeyhoShip
/// @details Communicates the ship's position to all cannonballs so they can initialize their launch
/// parameters accordingly. In @ref calc(), this manager reacts when the @ref ObjectHeyhoShip has
/// reached the end of a rail segment, checking to see if any cannonballs should be launched.
class ObjectHeyhoShipManager final : public ObjectSniper {
public:
    ObjectHeyhoShipManager();
    ~ObjectHeyhoShipManager() override;

    void init() override;
    void calc() override;
};

} // namespace Kinoko::Field
