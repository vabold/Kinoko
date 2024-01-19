#pragma once

#include "game/kart/KartObjectProxy.hh"

namespace Kart {

class KartMove : public KartObjectProxy {
public:
    KartMove();

    void setInitialPhysicsValues(const EGG::Vector3f &pos, const EGG::Vector3f &angles);
};

class KartMoveBike : public KartMove {};

} // namespace Kart
