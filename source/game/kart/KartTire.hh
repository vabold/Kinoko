#pragma once

#include "game/kart/KartSuspensionPhysics.hh"

namespace Kart {

/// @brief A holder for a wheel's physics data.
/// @nosubgrouping
class KartTire {
public:
    KartTire(KartSuspensionPhysics::TireType tireType, u16 bspWheelIdx);
    virtual ~KartTire();

    void init(u16 tireIdx);
    void initBsp();

    /// @beginGetters
    [[nodiscard]] WheelPhysics *wheelPhysics();
    /// @endGetters

protected:
    virtual void createPhysics(u16 tireIdx);

    KartSuspensionPhysics::TireType m_tireType;
    u16 m_bspWheelIdx;
    WheelPhysics *m_wheelPhysics;
};

/// @brief A holder for a kart's front tire's physics data.
class KartTireFront : public KartTire {
public:
    KartTireFront(KartSuspensionPhysics::TireType tireType, u16 bspWheelIdx);
    ~KartTireFront();

private:
    void createPhysics(u16 tireIdx) override;
};

/// @brief A holder for a bike's front tire's physics data.
class KartTireFrontBike : public KartTire {
public:
    KartTireFrontBike(KartSuspensionPhysics::TireType tireType, u16 bspWheelIdx);
    ~KartTireFrontBike();

private:
    void createPhysics(u16 tireIdx) override;
};

/// @brief A holder for a bike's rear tire's physics data.
class KartTireRearBike : public KartTire {
public:
    KartTireRearBike(KartSuspensionPhysics::TireType tireType, u16 bspWheelIdx);
    ~KartTireRearBike();

private:
    void createPhysics(u16 tireIdx) override;
};

} // namespace Kart
