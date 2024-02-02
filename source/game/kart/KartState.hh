#pragma once

#include "game/kart/KartObjectProxy.hh"

namespace Kart {

class KartState : KartObjectProxy {
public:
    KartState();

    void init();
    void reset();

    bool isGround() const;

private:
    bool m_ground;
};

} // namespace Kart
