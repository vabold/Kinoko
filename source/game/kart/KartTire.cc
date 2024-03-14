#include "KartTire.hh"

namespace Kart {

KartTire::KartTire(u16 bspWheelIdx) : m_bspWheelIdx(bspWheelIdx) {}

KartTire::~KartTire() = default;

void KartTire::createPhysics(u16 tireIdx) {
    m_wheelPhysics = new WheelPhysics(tireIdx, 1);
}

void KartTire::init(u16 tireIdx) {
    createPhysics(tireIdx);
    m_wheelPhysics->init();
}

void KartTire::initBsp() {
    m_wheelPhysics->initBsp();
}

WheelPhysics *KartTire::wheelPhysics() {
    return m_wheelPhysics;
}

KartTireFrontBike::KartTireFrontBike(u16 bspWheelIdx) : KartTire(bspWheelIdx) {}

KartTireFrontBike::~KartTireFrontBike() = default;

void KartTireFrontBike::createPhysics(u16 tireIdx) {
    m_wheelPhysics = new WheelPhysics(tireIdx, 0);
}

KartTireRearBike::KartTireRearBike(u16 bspWheelIdx) : KartTire(bspWheelIdx) {}

KartTireRearBike::~KartTireRearBike() = default;

void KartTireRearBike::createPhysics(u16 tireIdx) {
    m_wheelPhysics = new WheelPhysics(tireIdx, 1);
}

} // namespace Kart
