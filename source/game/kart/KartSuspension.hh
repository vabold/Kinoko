#pragma once

#include "game/kart/KartSuspensionPhysics.hh"

namespace Kart {

class KartSuspension : KartObjectProxy {
public:
    KartSuspension();
    ~KartSuspension();

    void init(u16 wheelIdx, u16 bspWheelIdx);
    void initPhysics();

    void setInitialState();

    KartSuspensionPhysics *suspPhysics();

private:
    KartSuspensionPhysics *m_physics;
};

class KartSuspensionFrontBike : public KartSuspension {};

class KartSuspensionRearBike : public KartSuspension {};

} // namespace Kart
