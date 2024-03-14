#pragma once

#include "game/kart/KartObjectProxy.hh"

namespace Kart {

class KartState : KartObjectProxy {
public:
    KartState();

    void init();
    void reset();

    void calc();
    void calcCollisions();

    bool isGround() const;
    const EGG::Vector3f &top() const;

private:
    bool m_ground;

    EGG::Vector3f m_top;
};

} // namespace Kart
