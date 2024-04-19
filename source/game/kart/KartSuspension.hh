#pragma once

#include "game/kart/KartSuspensionPhysics.hh"

namespace Kart {

/// @brief Doesn't do much besides hold a pointer to KartSuspensionPhysics.
/// @nosubgrouping
class KartSuspension : protected KartObjectProxy {
public:
    KartSuspension();
    virtual ~KartSuspension();

    void init(u16 wheelIdx, u16 bspWheelIdx);
    void initPhysics();

    /// @beginSetters
    void setInitialState();
    /// @endSetters

    /// @beginGetters
    KartSuspensionPhysics *suspPhysics();
    /// @endGetters

private:
    KartSuspensionPhysics *m_physics;
};

class KartSuspensionFrontBike : public KartSuspension {
public:
    KartSuspensionFrontBike();
    ~KartSuspensionFrontBike();
};

class KartSuspensionRearBike : public KartSuspension {
public:
    KartSuspensionRearBike();
    ~KartSuspensionRearBike();
};

} // namespace Kart
