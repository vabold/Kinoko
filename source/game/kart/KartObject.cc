#include "KartObject.hh"

#include "game/kart/KartParam.hh"
#include "game/kart/KartSub.hh"
#include "game/kart/KartSuspension.hh"
#include "game/kart/KartTire.hh"

#include "game/field/ObjectCollisionKart.hh"

#include "game/render/KartModel.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/RaceManager.hh"

namespace Kart {

/// @addr{0x8058DDBC}
KartObject::KartObject(KartParam *param) {
    m_pointers.param = param;
}

/// @addr{0x8058DEF0}
KartObject::~KartObject() {
    delete m_pointers.param;
    delete m_pointers.body;
    delete m_pointers.sub;
    delete m_pointers.model;
    delete m_pointers.objectCollisionKart;

    for (auto *susp : m_pointers.suspensions) {
        delete susp;
    }

    for (auto *tire : m_pointers.tires) {
        delete tire;
    }
}

/// @addr{0x8058EA0C}
void KartObject::createTires() {
    constexpr u16 BSP_WHEEL_INDICES[8] = {0, 0, 1, 1, 2, 2, 3, 3};
    constexpr KartSuspensionPhysics::TireType X_MIRRORED_TIRE[8] = {
            KartSuspensionPhysics::TireType::Kart,
            KartSuspensionPhysics::TireType::KartReflected,
            KartSuspensionPhysics::TireType::Kart,
            KartSuspensionPhysics::TireType::KartReflected,
            KartSuspensionPhysics::TireType::Kart,
            KartSuspensionPhysics::TireType::KartReflected,
            KartSuspensionPhysics::TireType::Kart,
            KartSuspensionPhysics::TireType::KartReflected,
    };

    auto bodyType = m_pointers.param->stats().body;
    u32 tireCount = m_pointers.param->tireCount();

    if (bodyType == KartParam::Stats::Body::Three_Wheel_Kart) {
        tireCount = 4;
    }

    for (u16 wheelIdx = 0, i = 0; i < tireCount; ++i) {
        if (bodyType == KartParam::Stats::Body::Three_Wheel_Kart && i == 0) {
            continue;
        }

        u16 bspWheelIdx = BSP_WHEEL_INDICES[i];
        KartSuspensionPhysics::TireType tireType = X_MIRRORED_TIRE[i];

        KartSuspension *sus = new KartSuspension;
        KartTire *tire = (bspWheelIdx == 0) ? new KartTireFront(tireType, bspWheelIdx) :
                                              new KartTire(tireType, bspWheelIdx);

        m_pointers.suspensions.push_back(sus);
        m_pointers.tires.push_back(tire);

        sus->init(wheelIdx++, tireType, bspWheelIdx);
    }
}

/// @addr{0x8058E5F8}
KartBody *KartObject::createBody(KartPhysics *physics) {
    return new KartBodyKart(physics);
}

/// @addr{0x8058E22C}
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
    m_pointers.objectCollisionKart = new Field::ObjectCollisionKart();
}

/// @addr{0x8058E188}
void KartObject::initImpl() {
    sub()->initAABB(m_pointers, this);
    sub()->init();
    objectCollisionKart()->init(param()->playerIdx());
}

/// @brief Sets the initial position and rotation of the kart based off the current track.
/// @addr{0x8058EE48}
void KartObject::prepare() {
    EGG::Vector3f euler_angles_deg, position;

    System::RaceManager::Instance()->findKartStartPoint(position, euler_angles_deg);
    move()->setInitialPhysicsValues(position, euler_angles_deg);
}

/// @addr{0x8058E804}
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

/// @addr{0x8058E724}
void KartObject::createSub() {
    m_pointers.sub = new KartSub;
    m_pointers.sub->createSubsystems(m_pointers.param->isBike());
}

/// @addr{0x8058F820}
void KartObject::createModel() {
    s_proxyList.clear();

    if (isBike()) {
        m_pointers.model = new Render::KartModelBike;
    } else {
        m_pointers.model = new Render::KartModelKart;
    }

    ApplyAll(&m_pointers);

    m_pointers.model->init();
}

/// @addr{0x8058EEB4}
void KartObject::calcSub() {
    sub()->calcPass0();
}

/// @addr{0x8058EEBC}
void KartObject::calc() {
    sub()->calcPass1();
    model()->calc();
}

const KartAccessor *KartObject::accessor() const {
    return &m_pointers;
}

/// @addr{0x8058F5B4}
KartObject *KartObject::Create(Character character, Vehicle vehicle, u8 playerIdx) {
    s_proxyList.clear();

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

    for (u16 i = 0; i < object->suspCount(); ++i) {
        object->suspension(i)->initPhysics();
    }

    for (u16 i = 0; i < object->tireCount(); ++i) {
        object->tire(i)->initBsp();
    }

    return object;
}

/// @addr{0x8058F20C}
KartObjectBike::KartObjectBike(KartParam *param) : KartObject(param) {}

/// @addr{0x8058F8B0}
KartObjectBike::~KartObjectBike() = default;

/// @addr{0x8058F260}
KartBody *KartObjectBike::createBody(KartPhysics *physics) {
    if (m_pointers.param->isVehicleRelativeBike()) {
        return new KartBodyQuacker(physics);
    } else {
        return new KartBodyBike(physics);
    }
}

/// @addr{0x8058F2E8}
void KartObjectBike::createTires() {
    for (u16 wheelIdx = 0; wheelIdx < m_pointers.param->suspCount(); ++wheelIdx) {
        KartSuspension *sus = nullptr;
        KartTire *tire = nullptr;

        if (wheelIdx == 0 || wheelIdx == 2) {
            sus = new KartSuspensionFrontBike;
            tire = new KartTireFrontBike(KartSuspensionPhysics::TireType::Bike, 0);
        } else {
            sus = new KartSuspensionRearBike;
            tire = new KartTireRearBike(KartSuspensionPhysics::TireType::Bike, 1);
        }

        m_pointers.suspensions.push_back(sus);
        m_pointers.tires.push_back(tire);

        sus->init(wheelIdx, KartSuspensionPhysics::TireType::Bike, wheelIdx);
    }
}
} // namespace Kart
