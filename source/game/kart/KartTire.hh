#pragma once

#include "game/kart/KartSuspensionPhysics.hh"

namespace Kart {

class KartTire {
public:
    KartTire(u16 bspWheelIdx);
    ~KartTire();

    virtual void createPhysics(u16 tireIdx);

    void init(u16 tireIdx);
    void initBsp();

    WheelPhysics *wheelPhysics();

protected:
    u16 m_bspWheelIdx;
    WheelPhysics *m_wheelPhysics;
};

class KartTireFrontBike : public KartTire {
public:
    KartTireFrontBike(u16 bspWheelIdx);
    ~KartTireFrontBike();

    void createPhysics(u16 tireIdx) override;
};

class KartTireRearBike : public KartTire {
public:
    KartTireRearBike(u16 bspWheelIdx);
    ~KartTireRearBike();

    void createPhysics(u16 tireIdx) override;
};

} // namespace Kart
