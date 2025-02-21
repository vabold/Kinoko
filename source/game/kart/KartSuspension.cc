#include "KartSuspension.hh"

namespace Kart {

/// @addr{0x80598B08}
KartSuspension::KartSuspension() = default;

/// @addr{0x8058F52C}
KartSuspension::~KartSuspension() {
    delete m_physics;
}

/// @addr{0x80598B60}
void KartSuspension::init(u16 wheelIdx, KartSuspensionPhysics::TireType tireType, u16 bspWheelIdx) {
    m_physics = new KartSuspensionPhysics(wheelIdx, tireType, bspWheelIdx);
}

/// @addr{0x80598BD4}
void KartSuspension::initPhysics() {
    m_physics->init();
}

/// @addr{0x80598BE4}
void KartSuspension::setInitialState() {
    m_physics->setInitialState();
}

KartSuspensionFrontBike::KartSuspensionFrontBike() = default;

/// @addr{0x805993CC}
KartSuspensionFrontBike::~KartSuspensionFrontBike() = default;

KartSuspensionRearBike::KartSuspensionRearBike() = default;

/// @addr{0x8059938C}
KartSuspensionRearBike::~KartSuspensionRearBike() = default;

} // namespace Kart
