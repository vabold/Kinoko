#pragma once

#include "game/kart/KartSuspensionPhysics.hh"

namespace Kart {

/// @brief Doesn't do much besides hold a pointer to KartSuspensionPhysics.
/// @nosubgrouping
class KartSuspension : protected KartObjectProxy {
public:
    KartSuspension();
    virtual ~KartSuspension();

    void init(u16 wheelIdx, KartSuspensionPhysics::TireType tireType, u16 bspWheelIdx);
    void initPhysics();

    /// @beginSetters
    void setInitialState();
    /// @endSetters

    /// @beginGetters
    [[nodiscard]] KartSuspensionPhysics *suspPhysics() {
        return m_physics;
    }
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
