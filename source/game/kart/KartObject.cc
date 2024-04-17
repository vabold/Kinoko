#include "KartObject.hh"

#include "game/kart/KartParam.hh"
#include "game/kart/KartSub.hh"
#include "game/kart/KartSuspension.hh"
#include "game/kart/KartTire.hh"

#include "game/render/KartModel.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/RaceManager.hh"

namespace Kart {

KartObject::KartObject(KartParam *param) {
    m_pointers.param = param;
}

KartObject::~KartObject() {
    delete m_pointers.param;
    delete m_pointers.body;
    delete m_pointers.sub;
    delete m_pointers.model;

    for (auto *susp : m_pointers.suspensions) {
        delete susp;
    }

    for (auto *tire : m_pointers.tires) {
        delete tire;
    }
}

KartBody *KartObject::createBody(KartPhysics *physics) {
    return new KartBodyKart(physics);
}

void KartObject::init() {
    prepareTiresAndSuspensions();
    createSub();
    auto *physics = KartPhysics::Create(*m_pointers.param);
    auto *body = createBody(physics);
    m_pointers.body = body;
    createTires();
    for (u16 tireIdx = 0; tireIdx < m_pointers.param->tireCount(); ++tireIdx) {
        m_pointers.tires[tireIdx]->init(tireIdx);
    }
}

void KartObject::initImpl() {
    sub()->init();
}

void KartObject::prepare() {
    EGG::Vector3f euler_angles_deg, position;

    System::RaceManager::Instance()->findKartStartPoint(position, euler_angles_deg);
    move()->setInitialPhysicsValues(position, euler_angles_deg);
}

void KartObject::prepareTiresAndSuspensions() {
    constexpr u16 LOCAL_20[4] = {2, 1, 1, 1};
    constexpr u16 LOCAL_28[4] = {2, 1, 1, 2};

    const BSP &rBsp = m_pointers.param->bsp();
    const KartParam::Stats::Body bodyWheels = m_pointers.param->stats().body;
    u16 wheelCount = 0;

    if (rBsp.wheels[0].enable != 0) {
        wheelCount += LOCAL_20[static_cast<u16>(bodyWheels)];
    }
    if (rBsp.wheels[1].enable != 0) {
        wheelCount += LOCAL_28[static_cast<u16>(bodyWheels)];
    }
    if (rBsp.wheels[2].enable != 0) {
        wheelCount += LOCAL_20[static_cast<u16>(bodyWheels)];
    }
    if (rBsp.wheels[3].enable != 0) {
        wheelCount += LOCAL_28[static_cast<u16>(bodyWheels)];
    }

    m_pointers.param->setTireCount(wheelCount);
    m_pointers.param->setSuspCount(wheelCount);
}

void KartObject::createSub() {
    m_pointers.sub = new KartSub;
    m_pointers.sub->createSubsystems(m_pointers.param->isBike());
}

void KartObject::createModel() {
    Abstract::List list;
    s_list = &list;

    if (isBike()) {
        m_pointers.model = new Render::KartModelBike;
    } else {
        m_pointers.model = new Render::KartModelKart;
    }

    ApplyAll(&m_pointers);
    s_list = nullptr;

    m_pointers.model->init();
}

void KartObject::calcSub() {
    sub()->calcPass0();
}

void KartObject::calc() {
    sub()->calcPass1();
    model()->calc();
}

const KartAccessor *KartObject::accessor() const {
    return &m_pointers;
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
    object->m_pointers.sub->copyPointers(object->m_pointers);

    // Applies a valid pointer to all of the proxies we create
    ApplyAll(&object->m_pointers);
    s_list = nullptr;

    for (u16 i = 0; i < object->suspCount(); ++i) {
        object->suspension(i)->initPhysics();
    }

    for (u16 i = 0; i < object->tireCount(); ++i) {
        object->tire(i)->initBsp();
    }

    return object;
}

KartObjectBike::KartObjectBike(KartParam *param) : KartObject(param) {}

KartObjectBike::~KartObjectBike() = default;

KartBody *KartObjectBike::createBody(KartPhysics *physics) {
    return new KartBodyBike(physics);
}

void KartObjectBike::createTires() {
    for (u16 wheelIdx = 0; wheelIdx < m_pointers.param->suspCount(); ++wheelIdx) {
        KartSuspension *sus = nullptr;
        KartTire *tire = nullptr;

        if (wheelIdx == 0 || wheelIdx == 2) {
            sus = new KartSuspensionFrontBike;
            tire = new KartTireFrontBike(0);
        } else {
            sus = new KartSuspensionRearBike;
            tire = new KartTireRearBike(1);
        }

        m_pointers.suspensions.push_back(sus);
        m_pointers.tires.push_back(tire);

        sus->init(wheelIdx, wheelIdx);
    }
}

} // namespace Kart
