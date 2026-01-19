#include "ObjectCarTGE.hh"

#include "game/field/ObjectCollisionCylinder.hh"
#include "game/field/ObjectDirector.hh"
#include "game/field/RailManager.hh"
#include "game/field/obj/ObjectHighwayManager.hh"

#include "game/kart/KartObject.hh"

#include "game/system/RaceConfig.hh"

namespace Field {

/// @addr{0x806D5EE4}
ObjectCarTGE::ObjectCarTGE(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), StateManager(this, STATE_ENTRIES), m_auxCollision(nullptr),
      m_highwayVel(static_cast<f32>(params.setting(2))),
      m_localVel(static_cast<f32>(params.setting(1))), m_carName{}, m_mdlName{},
      m_carType(CarType::Normal), m_dummyId(ObjectId::None),
      m_scaledTangentDir(EGG::Vector3f::zero), m_currSpeed(0.0f), m_up(EGG::Vector3f::zero),
      m_tangent(EGG::Vector3f::zero) {
    // The base game returns before the StateManager sets its state entries.
    // Since we handle this in the template specialization's constructor in Kinoko,
    // we need to reset the StateManager parameters back to their default values before returning.
    s16 pathId = params.pathId();
    if (pathId == -1) {
        m_nextStateId = -1;
        m_currentStateId = 0;
        m_currentFrame = 0;

        return;
    }

    // The base game preemptively calculates collision for all points along the rail.
    RailManager::Instance()->rail(pathId)->checkSphereFull();

    const auto *name = getName();

    if (strcmp(name, "car_body") == 0) {
        snprintf(m_carName, sizeof(m_carName), "%s", "K_car_body");
        m_carType = CarType::Normal;
    } else if (strcmp(name, "kart_truck") == 0) {
        snprintf(m_carName, sizeof(m_carName), "%s", "K_truck");
        m_carType = CarType::Truck;
    } else if (strcmp(name, "K_bomb_car") == 0) {
        PANIC("Bomb cars are not implemented!");
    }

    const auto *resourceName = getResources();
    s32 carVariant = static_cast<s32>(params.setting(3));

    switch (carVariant) {
    case 0: {
        if (strcmp(resourceName, "K_truck") == 0) {
            snprintf(m_mdlName, sizeof(m_mdlName), "%s_b", "K_truck");
        } else if (strcmp(resourceName, "K_car_body") == 0) {
            snprintf(m_mdlName, sizeof(m_mdlName), "%s_b", "K_car");
        }
    } break;
    case 1: {
        if (strcmp(resourceName, "K_truck") == 0) {
            snprintf(m_mdlName, sizeof(m_mdlName), "%s_o", "K_truck");
        } else if (strcmp(resourceName, "K_car_body") == 0) {
            snprintf(m_mdlName, sizeof(m_mdlName), "%s_r", "K_car");
        }
    } break;
    case 2: {
        if (strcmp(resourceName, "K_truck") == 0) {
            snprintf(m_mdlName, sizeof(m_mdlName), "%s_g", "K_truck");
        } else if (strcmp(resourceName, "K_car_body") == 0) {
            snprintf(m_mdlName, sizeof(m_mdlName), "%s_y", "K_car");
        }
    } break;
    default: {
        PANIC("Bomb cars are not implemented!");
        break;
    }
    }

    switch (m_carType) {
    case CarType::Normal:
        m_dummyId = ObjectDirector::Instance()->flowTable().getIdFromName("car_body_dummy");
        break;
    case CarType::Truck:
        m_dummyId = ObjectDirector::Instance()->flowTable().getIdFromName("kart_truck_dummy");
        break;
    default:
        PANIC("Bomb cars are not implemented!");
        break;
    }

    if (System::RaceConfig::Instance()->raceScenario().course == Course::Moonview_Highway) {
        registerManagedObject();
    }
}

/// @addr{0x806D691C}
ObjectCarTGE::~ObjectCarTGE() {
    delete m_auxCollision;
}

/// @addr{0x806D6B14}
void ObjectCarTGE::init() {
    constexpr f32 HIT_ANGLE_TRUCK = 20.0f;
    constexpr f32 HIT_ANGLE_NORMAL = 40.0f;

    ASSERT(m_mapObj);
    if (m_mapObj->pathId() == -1) {
        return;
    }

    u16 idx = static_cast<u16>(m_mapObj->setting(0));
    m_railInterpolator->init(0.0f, idx);

    auto *rail = RailManager::Instance()->rail(m_mapObj->pathId());
    u16 speedSetting = rail->points()[idx].setting[1];
    if (speedSetting == 1) {
        m_railInterpolator->setCurrVel(m_highwayVel);
    } else if (speedSetting == 0) {
        m_railInterpolator->setCurrVel(m_localVel);
    }

    u16 curPointSpeedSetting = m_railInterpolator->curPoint().setting[1];
    u16 nextPointSpeedSetting = m_railInterpolator->nextPoint().setting[1];

    if (curPointSpeedSetting == 0 && nextPointSpeedSetting == 1) {
        m_nextStateId = 1;
    } else if (curPointSpeedSetting == 1 && nextPointSpeedSetting == 0) {
        m_nextStateId = 2;
    }

    m_squashed = false;
    m_pos = m_railInterpolator->curPos();
    m_flags.setBit(eFlags::Position);
    m_currSpeed = m_railInterpolator->speed();
    m_scaledTangentDir = m_railInterpolator->curTangentDir() * m_currSpeed;
    m_hitAngle = (m_carType == CarType::Truck) ? HIT_ANGLE_TRUCK : HIT_ANGLE_NORMAL;
}

/// @addr{0x806D6ECC}
void ObjectCarTGE::calc() {
    StateManager::calc();

    if (m_railInterpolator->calc() == RailInterpolator::Status::SegmentEnd) {
        u16 curPointSpeedSetting = m_railInterpolator->curPoint().setting[1];
        u16 nextPointSpeedSetting = m_railInterpolator->nextPoint().setting[1];

        if (curPointSpeedSetting == 0 && nextPointSpeedSetting == 1) {
            m_nextStateId = 1;
        } else if (curPointSpeedSetting == 1 && nextPointSpeedSetting == 0) {
            m_nextStateId = 2;
        }
    }

    calcPos();

    m_hasAuxCollision = false;
}

/// @addr{0x806D7AF8}
void ObjectCarTGE::createCollision() {
    constexpr f32 TRUCK_RADIUS = 190.0f;
    constexpr f32 TRUCK_HEIGHT = 500.0f;
    constexpr f32 NORMAL_RADIUS = 150.0f;
    constexpr f32 NORMAL_HEIGHT = 200.0f;

    ASSERT(m_carType == CarType::Truck || m_carType == CarType::Normal);

    bool isTruck = (m_carType == CarType::Truck);

    ObjectCollidable::createCollision();

    f32 radius = isTruck ? TRUCK_RADIUS : NORMAL_RADIUS;
    f32 height = isTruck ? TRUCK_HEIGHT : NORMAL_HEIGHT;

    m_auxCollision = new ObjectCollisionCylinder(radius, height, collisionCenter());
}

/// @addr{0x806D7BF8}
void ObjectCarTGE::calcCollisionTransform() {
    auto *col = collision();

    if (!col) {
        return;
    }

    calcTransform();
    col->transform(m_transform, m_scale, m_scaledTangentDir);
    calcTransform();
    EGG::Matrix34f mat;
    SetRotTangentHorizontal(mat, m_transform.base(2), EGG::Vector3f::ey);
    calcTransform();
    mat.setBase(3, m_transform.base(3));
    m_auxCollision->transform(mat, m_scale, m_scaledTangentDir);
}

/// @addr{0x806D69C0}
f32 ObjectCarTGE::getCollisionRadius() const {
    constexpr f32 NORMAL_RADIUS = 600.0f;
    constexpr f32 TRUCK_RADIUS = 1100.0f;

    ASSERT(m_carType == CarType::Truck || m_carType == CarType::Normal);
    return (m_carType == CarType::Truck) ? TRUCK_RADIUS : NORMAL_RADIUS;
}

/// @addr{0x806D7328}
Kart::Reaction ObjectCarTGE::onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
        Kart::Reaction /*reactionOnObj*/, EGG::Vector3f &hitDepth) {
    constexpr u32 SQUASH_INVULNERABILITY = 200;

    if (!m_hasAuxCollision) {
        EGG::Vector3f hitDepthNorm = hitDepth;
        hitDepthNorm.normalise2();

        if (hitDepthNorm.y > 0.9f) {
            return Kart::Reaction::UntrickableJumpPad;
        }
    }

    if (m_highwayMgr && m_highwayMgr->squashTimer() < SQUASH_INVULNERABILITY) {
        const auto &hitTable = ObjectDirector::Instance()->hitTableKart();
        reactionOnKart = hitTable.reaction(hitTable.slot(static_cast<ObjectId>(m_dummyId)));
    }

    // In the base game, behavior branches on reactionOnObj, but for time trials it's always 0.
    if (reactionOnKart != Kart::Reaction::None && reactionOnKart != Kart::Reaction::WallAllSpeed) {
        m_squashed = true;
        calcTransform();
        EGG::Vector3f v2 = m_transform.base(2);
        v2.y = 0.0f;
        v2.normalise2();
        EGG::Vector3f posDelta = kartObj->pos() - m_pos;
        posDelta.y = 0.0f;
        posDelta.normalise2();

        if (v2.dot(posDelta) < EGG::Mathf::CosFIdx(0.7111111f * m_hitAngle) && !m_hasAuxCollision) {
            reactionOnKart = Kart::Reaction::LaunchSpin;
        }

        hitDepth.setZero();
    }

    return reactionOnKart;
}

/// @addr{0x806DA660}
bool ObjectCarTGE::checkCollision(ObjectCollisionBase *lhs, EGG::Vector3f &dist) {
    dist = EGG::Vector3f::zero;
    bool hasCol = lhs->check(*m_collision, dist);

    if (!hasCol) {
        hasCol = lhs->check(*m_auxCollision, dist);
        m_hasAuxCollision = hasCol;
    }

    return hasCol;
}

/// @addr{0x806D7CF8}
const EGG::Vector3f &ObjectCarTGE::collisionCenter() const {
    static constexpr EGG::Vector3f CENTER_TRUCK = EGG::Vector3f(0.0f, 300.0f, 0.0f);
    static constexpr EGG::Vector3f CENTER_NORMAL = EGG::Vector3f(0.0f, 100.0f, 0.0f);
    static constexpr EGG::Vector3f CENTER_DEFAULT = EGG::Vector3f(0.0f, 0.0f, 0.0f);

    switch (m_carType) {
    case CarType::Truck:
        return CENTER_TRUCK;
    case CarType::Normal:
        return CENTER_NORMAL;
    default:
        return CENTER_DEFAULT;
    }
}

void ObjectCarTGE::enterStateStub() {}

void ObjectCarTGE::calcStateStub() {}

/// @addr{0x806D7D70}
/// @brief The state when cars are speeding up.
/// @details On Moonview Highway, the speed cap m_highwayVel is 70, which means this state is only
/// executed for 1 frame when cars get on the highway.
void ObjectCarTGE::calcState1() {
    m_currSpeed += TOLL_BOOTH_ACCEL;

    if (m_currSpeed > m_highwayVel) {
        m_currSpeed = m_highwayVel;
        m_nextStateId = 0;
    }

    m_railInterpolator->setCurrVel(m_currSpeed);
    m_scaledTangentDir = m_railInterpolator->curTangentDir() * m_currSpeed;
}

/// @addr{0x806D7E0C}
/// @brief The state when cars are slowing down.
/// @details On Moonview Highway, the speed floor m_localVel is 40, which means this state is only
/// executed for 1 frame when cars get off the highway.
void ObjectCarTGE::calcState2() {
    m_currSpeed -= TOLL_BOOTH_ACCEL;

    if (m_currSpeed < m_localVel) {
        m_currSpeed = m_localVel;
        m_nextStateId = 0;
    }

    m_railInterpolator->setCurrVel(m_currSpeed);
    m_scaledTangentDir = m_railInterpolator->curTangentDir() * m_currSpeed;
}

/// @addr{0x806D9000}
void ObjectCarTGE::calcPos() {
    constexpr f32 NORMAL_SPEED = 1500.0f;
    constexpr f32 TRUCK_SPEED = 1600.0f;

    f32 speed = (m_carType == CarType::Truck) ? TRUCK_SPEED : NORMAL_SPEED;
    f32 t = speed * m_scale.z * 0.5f;

    EGG::Vector3f curDir;
    EGG::Vector3f curTangentDir;
    m_railInterpolator->evalCubicBezierOnPath(t, curDir, curTangentDir);

    const EGG::Vector3f curPos = m_railInterpolator->curPos();
    EGG::Vector3f posDelta = curPos - curDir;
    posDelta.normalise2();
    m_tangent += 0.1f * (posDelta - m_tangent);
    m_tangent.y = posDelta.y;
    m_tangent.normalise2();

    if (m_tangent.y > -0.05f) {
        m_pos = curPos - m_tangent * t * 0.5f;
    } else {
        m_pos = (curPos - m_tangent * t * 0.5f) - EGG::Vector3f::ey * 5.0f;
    }

    m_flags.setBit(eFlags::Position);
    m_up = OrthonormalBasis(m_tangent).base(1);
    setMatrixTangentTo(m_up, m_tangent);
}

} // namespace Field
