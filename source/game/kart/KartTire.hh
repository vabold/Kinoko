#pragma once

#include "game/kart/KartSuspensionPhysics.hh"

namespace Kart {

/// @brief A holder for a wheel's physics data.
/// @nosubgrouping
class KartTire {
public:
    KartTire(u16 bspWheelIdx);
    virtual ~KartTire();

    virtual void createPhysics(u16 tireIdx);

    void init(u16 tireIdx);
    void initBsp();

    /// @beginGetters
    [[nodiscard]] WheelPhysics *wheelPhysics();
    /// @endGetters

protected:
    u16 m_bspWheelIdx;
    WheelPhysics *m_wheelPhysics;
};

/// @brief A holder for a bike's front tire's physics data.
class KartTireFrontBike : public KartTire {
public:
    KartTireFrontBike(u16 bspWheelIdx);
    ~KartTireFrontBike();

    void createPhysics(u16 tireIdx) override;
};

/// @brief A holder for a bike's rear tire's physics data.
class KartTireRearBike : public KartTire {
public:
    KartTireRearBike(u16 bspWheelIdx);
    ~KartTireRearBike();

    void createPhysics(u16 tireIdx) override;
};

} // namespace Kart
