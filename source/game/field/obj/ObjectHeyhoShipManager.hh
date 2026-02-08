#pragma once

#include "game/field/obj/ObjectSniper.hh"

namespace Field {

class ObjectHeyhoShipManager final : public ObjectSniper {
public:
    ObjectHeyhoShipManager();
    ~ObjectHeyhoShipManager() override;

    void init() override;
    void calc() override;
};

} // namespace Field
