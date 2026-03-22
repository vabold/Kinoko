#pragma once

#include "game/field/obj/ObjectSniper.hh"

namespace Kinoko::Field {

class ObjectHeyhoShipManager final : public ObjectSniper {
public:
    ObjectHeyhoShipManager();
    ~ObjectHeyhoShipManager() override;

    void init() override;
    void calc() override;
};

} // namespace Kinoko::Field
