#include "KartObject.hh"

#include "source/game/system/RaceConfig.hh"

namespace Kart {

KartObject::KartObject(KartParam *param) : m_param(param) {}

KartObject::~KartObject() {
    delete m_param;
}

void KartObject::createComponents() {}

KartObject *KartObject::Create(Character character, Vehicle vehicle) {
    KartParam *param = new KartParam(character, vehicle);

    KartObject *object = nullptr;
    if (vehicle < Vehicle::Standard_Bike_S) {
        object = new KartObject(param);
    } else {
        object = new KartObjectBike(param);
    }
    return object;
}

KartObjectBike::KartObjectBike(KartParam *param) : KartObject(param) {}

KartObjectBike::~KartObjectBike() {
    delete m_param;
}

void KartObjectBike::createComponents() {}

} // namespace Kart
