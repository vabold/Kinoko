#include "KartObject.hh"

#include "game/kart/KartSub.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/RaceManager.hh"

namespace Kart {

KartObject::KartObject(KartParam *param) {
    m_pointers.m_param = param;
}

KartObject::~KartObject() {
    delete m_pointers.m_param;
}

KartBody *KartObject::createBody(KartPhysics *physics) {
    return new KartBodyKart(physics);
}

void KartObject::init() {
    createSub();
    auto *physics = KartPhysics::Create(*m_pointers.m_param);
    auto *body = createBody(physics);
    m_pointers.m_body = body;
}

void KartObject::initImpl() {
    sub()->init();
}

void KartObject::prepare() {
    EGG::Vector3f euler_angles_deg, position;

    System::RaceManager::Instance()->findKartStartPoint(position, euler_angles_deg);
    move()->setInitialPhysicsValues(position, euler_angles_deg);
}

void KartObject::createSub() {
    m_pointers.m_sub = new KartSub;
    m_pointers.m_sub->createSubsystems(m_pointers.m_param->isBike());
}

void KartObject::calcSub() {
    m_pointers.m_sub->calcPass0();
}

KartObject *KartObject::Create(Character character, Vehicle vehicle, u8 playerIdx) {
    Abstract::List list;
    s_list = &list;

    KartParam *param = new KartParam(character, vehicle, playerIdx);

    KartObject *object = nullptr;
    if (vehicle < Vehicle::Standard_Bike_S) {
        object = new KartObject(param);
    } else {
        object = new KartObjectBike(param);
    }

    object->init();
    object->m_pointers.m_sub->copyPointers(object->m_pointers);

    // Applies a valid pointer to all of the proxies we create
    ApplyAll(&object->m_pointers);
    s_list = nullptr;
    return object;
}

KartObjectBike::KartObjectBike(KartParam *param) : KartObject(param) {}

KartObjectBike::~KartObjectBike() {
    delete m_pointers.m_param;
}

KartBody *KartObjectBike::createBody(KartPhysics *physics) {
    return new KartBodyBike(physics);
}

} // namespace Kart
