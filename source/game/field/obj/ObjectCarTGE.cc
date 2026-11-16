#include "ObjectCarTGE.hh"

#include "game/field/ObjectCollisionCylinder.hh"
#include "game/field/ObjectDirector.hh"
#include "game/field/RailInterpolator.hh"
#include "game/field/RailManager.hh"
#include "game/field/obj/ObjectHighwayManager.hh"

#include "game/kart/KartObject.hh"
#include "game/kart/Reaction.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/map/MapdataGeoObj.hh"

#include <cstring>

namespace Kinoko::Field {

/// @addr{0x806D5EE4}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Pre-computes all floor normals along the rail. Determines @ref m_carName and @ref
/// m_carType based off the object name and @ref m_mdlName based off the resource name and the
/// car/truck color specified by param setting 4. Also determines the @ref m_dummyId based off the
/// car type. Finally, registers the object with the highway manager if the course is Moonview
/// Highway, so that the highway manager can enforce squish cooldowns for the player.
/// @note In the base game, the constructor may return early if the object does not have a valid
/// rail ID. This same failsafe is present in @ref init(). However, there is no such `nullptr` check
/// in @ref calc(). Furthermore, the object will never finish being initialized; since @ref init()
/// returns early, the model name for the car's shadow is never set, so `UpdateShadow()` will
/// attempt to dereference `nullptr` since the shadow could not be loaded. As a result, we do not
/// bother implementing this failsafe in Kinoko, since it is not actually an effective failsafe.
ObjectCarTGE::ObjectCarTGE(const System::MapdataGeoObj &params)
    : ObjectCollidable(params),
      StateManager(this, STATE_ENTRIES),
      m_auxCollision(nullptr),
      m_highwaySpeed(static_cast<f32>(params.setting(2))),
      m_localSpeed(static_cast<f32>(params.setting(1))),
      m_carName{},
      m_mdlName{},
      m_carType(CarType::Normal),
      m_dummyId(ObjectId::None),
      m_vel(EGG::Vector3f::zero),
      m_currSpeed(0.0f),
      m_tangent(EGG::Vector3f::zero) {
    u32 color = static_cast<u32>(params.setting(3));
    s16 pathId = params.pathId();

    ASSERT(pathId > -1);

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

    switch (color) {
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
/// @brief Virtual destructor that destroys the auxiliary collision object
ObjectCarTGE::~ObjectCarTGE() {
    EGG::egg_delete(m_auxCollision);
}

/// @addr{0x806D6B14}
/// @copybrief ObjectBase::init()
/// @details Initializes the rail interpolator to the node index specified by param setting 1. Sets
/// the rail interpolator's speed based on whether the car is starting on the highway or not. Calls
/// @ref calcState() to determine whether the car should be in the cruising, speedup, or
/// slowdown state. Sets the car's position to the current position of the rail interpolator. Sets
/// @ref m_currSpeed based off the rail interpolator's speed and sets @ref m_vel by scaling the rail
/// interpolator's tangent direction by @ref m_currSpeed. Finally, sets @ref m_hitAngle based on the
/// car type.
void ObjectCarTGE::init() {
    constexpr f32 HIT_ANGLE_TRUCK = 20.0f;
    constexpr f32 HIT_ANGLE_NORMAL = 40.0f;

    ASSERT(m_mapObj);
    u16 idx = m_mapObj->setting(0);
    m_railInterpolator->init(0.0f, idx);

    auto *rail = RailManager::Instance()->rail(m_mapObj->pathId());
    u16 speedSetting = rail->points()[idx].setting[1];
    if (speedSetting == 1) {
        m_railInterpolator->setCurrVel(m_highwaySpeed);
    } else if (speedSetting == 0) {
        m_railInterpolator->setCurrVel(m_localSpeed);
    }

    calcState();

    m_squashed = false;
    setPos(m_railInterpolator->curPos());
    m_currSpeed = m_railInterpolator->speed();
    m_vel = m_railInterpolator->curTangentDir() * m_currSpeed;
    m_hitAngle = (m_carType == CarType::Truck) ? HIT_ANGLE_TRUCK : HIT_ANGLE_NORMAL;
}

/// @addr{0x806D6ECC}
/// @copybrief ObjectBase::calc()
/// @details Evaluates the car's state machine and updates the rail interpolator. Calls @ref
/// calcState() to determine if the car should transition to a different state. Updates the
/// car's position along the rail via @ref calcPos(). Finally, resets @ref m_hasAuxCollision to
/// `false`.
void ObjectCarTGE::calc() {
    StateManager::calc();

    if (m_railInterpolator->calc() == RailInterpolator::Status::SegmentEnd) {
        calcState();
    }

    calcPos();

    m_hasAuxCollision = false;
}

/// @addr{0x806D7AF8}
/// @copybrief ObjectBase::createCollision()
/// @details Creates two collision objects, the second being a cylinder scaled based off vehicle
/// type.
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

    m_auxCollision = EGG::egg_new<ObjectCollisionCylinder>(radius, height, collisionCenter());
}

/// @addr{0x806D7BF8}
/// @copybrief ObjectBase::calcCollisionTransform()
/// @details Refreshes the collision transform to reflect the car's updated position and rotation.
/// Refreshes the auxiliary transform to reflect the same updates, but flattens the car's forward
/// direction to world up, thereby removing any pitch or tilt.
void ObjectCarTGE::calcCollisionTransform() {
    auto *col = collision();

    if (!col) {
        return;
    }

    calcTransform();
    col->transform(transform(), scale(), m_vel);
    calcTransform();
    EGG::Matrix34f mat;
    SetRotTangentHorizontal(mat, transform().base(2), EGG::Vector3f::ey);
    calcTransform();
    mat.setBase(3, transform().base(3));
    m_auxCollision->transform(mat, scale(), m_vel);
}

/// @addr{0x806DA7AC}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectCarTGE::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x806D68B0}
/// @copybrief ObjectBase::getResources()
/// @details Returns the resource name of the car variant
/// @return The resource name of the car variant, @ref m_carName
const char *ObjectCarTGE::getResources() const {
    return m_carName;
}

/// @addr{0x806DA7A4}
/// @copybrief ObjectBase::getKclName()
/// @return The model name of the car variant, @ref m_mdlName
const char *ObjectCarTGE::getKclName() const {
    return m_mdlName;
}

/// @addr{0x806D69C0}
/// @copybrief ObjectBase::getCollisionRadius()
/// @return The collision radius of the vehicle, `600.0f` for cars and `1100.0f` for trucks.
/// @pre Requires that @ref m_carType is either @ref CarType::Truck or @ref CarType::Normal.
f32 ObjectCarTGE::getCollisionRadius() const {
    constexpr f32 NORMAL_RADIUS = 600.0f;
    constexpr f32 TRUCK_RADIUS = 1100.0f;

    ASSERT(m_carType == CarType::Truck || m_carType == CarType::Normal);
    return (m_carType == CarType::Truck) ? TRUCK_RADIUS : NORMAL_RADIUS;
}

/// @addr{0x806D7CF8}
/// @copydoc ObjectCollidable::collisionCenter()
/// @details The center of the car's collision volume is based on its type. Normal cars have an
/// upwards offset of `100.0f` units, whereas trucks have an upwards offset of `300.0f` units.
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

/// @addr{0x806D7328}
/// @brief Causes the player to be hit, squished, or bounce depending on the collision scenario
/// @param kartObj The kart object that collided with this car
/// @param reactionOnKart The reaction that should be applied to the kart
/// @param reactionOnObj The reaction that should be applied to the car object (unused)
/// @param hitDepth The depth of the collision between the kart and the car
/// @return The reaction that should be applied to the kart after the collision.
/// @details If the kart collided with the primary collision shape of the car with a vertical hit
/// depth above `0.9f`, then causes the kart to bounce off the car/truck like a jump pad (@ref
/// Kart::Reaction::UntrickableJumpPad). Otherwise, if the @ref ObjectHighwayManager indicates that
/// the player is still within the squash invulnerability period, returns @ref Kart::Reaction::None
/// as a result of fetching the reaction for @ref m_dummyId. Otherwise, sets the @ref m_squashed
/// flag and returns either @ref Kart::Reaction::LaunchSpin or @ref
/// Kart::Reaction::ShortCrushLoseItem depending on the direction of the collision relative to the
/// car's orientation.
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
        EGG::Vector3f v2 = transform().base(2);
        v2.y = 0.0f;
        v2.normalise2();
        EGG::Vector3f posDelta = kartObj->pos() - pos();
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
/// @brief Checks kart collision against both the primary and auxiliary collision objects
/// @param lhs The collision object to check against this car
/// @param dist The vector to store the collision depth
/// @return `true` if a collision occurred, `false` otherwise
bool ObjectCarTGE::checkCollision(ObjectCollisionBase *lhs, EGG::Vector3f &dist) {
    dist = EGG::Vector3f::zero;
    bool hasCol = lhs->check(*m_collision, dist);

    if (!hasCol) {
        hasCol = lhs->check(*m_auxCollision, dist);
        m_hasAuxCollision = hasCol;
    }

    return hasCol;
}

/// @addr{0x806D7D70}
/// @brief Runs once per frame when cars are speeding up when getting on the highway
/// @details Increases @ref m_currSpeed by @ref TOLL_BOOTH_ACCEL. Once @ref m_currSpeed reaches
/// @ref m_highwaySpeed, it's clamped to @ref m_highwaySpeed and transitions to the cruising
/// state. Regardless, sets the rail interpolator's speed to the updated @ref m_currSpeed and
/// sets @ref m_vel by scaling the rail interpolator's current tangent direction by @ref
/// m_currSpeed.
/// @note On Moonview Highway, the cars' local speed is 40 units per frame and the highway speed is
/// 70 units per frame. Since this function accelerates cars by 200 units per frame, this speedup
/// state only lasts one frame.
void ObjectCarTGE::calcSpeedup() {
    m_currSpeed += TOLL_BOOTH_ACCEL;

    if (m_currSpeed > m_highwaySpeed) {
        m_currSpeed = m_highwaySpeed;
        m_nextStateId = 0;
    }

    m_railInterpolator->setCurrVel(m_currSpeed);
    m_vel = m_railInterpolator->curTangentDir() * m_currSpeed;
}

/// @addr{0x806D7E0C}
/// @brief Runs every frame when the car is slowing down after getting off the highway
/// @details Decreases @ref m_currSpeed by @ref TOLL_BOOTH_ACCEL. Once @ref m_currSpeed reaches
/// @ref m_localSpeed, it's clamped to @ref m_localSpeed and transitions to the cruising state.
/// Regardless, sets the rail interpolator's speed to the updated @ref m_currSpeed and sets @ref
/// m_vel by scaling the rail interpolator's current tangent direction by @ref m_currSpeed.
/// @note On Moonview Highway, the cars' local speed is 40 units per frame and the highway speed is
/// 70 units per frame. Since this function decelerates cars by 200 units per frame, this slowdown
/// state only lasts one frame.
void ObjectCarTGE::calcSlowdown() {
    m_currSpeed -= TOLL_BOOTH_ACCEL;

    if (m_currSpeed < m_localSpeed) {
        m_currSpeed = m_localSpeed;
        m_nextStateId = 0;
    }

    m_railInterpolator->setCurrVel(m_currSpeed);
    m_vel = m_railInterpolator->curTangentDir() * m_currSpeed;
}

/// @addr{0x806D9000}
/// @brief Calculates the position and orientation of the car along the rail
/// @details Derives position and orientation by computing a cubic bezier along the rail to find
/// the position of the rear of the car. Interpolates the X and Z components of @ref m_tangent
/// towards the true direction of motion, while the Y component is directly taken from the
/// car's travel direction. Updates the car's transformation matrix based on a derived up vector and
/// @ref m_tangent.
void ObjectCarTGE::calcPos() {
    constexpr f32 NORMAL_SPEED = 1500.0f;
    constexpr f32 TRUCK_SPEED = 1600.0f;
    constexpr f32 TANGENT_INTERP_RATE = 0.1f;

    f32 speed = (m_carType == CarType::Truck) ? TRUCK_SPEED : NORMAL_SPEED;
    f32 dist = speed * scale().z * 0.5f;

    EGG::Vector3f rearPos;
    EGG::Vector3f rearTangentDir;
    m_railInterpolator->evalCubicBezierOnPath(dist, rearPos, rearTangentDir);

    const EGG::Vector3f curPos = m_railInterpolator->curPos();
    EGG::Vector3f posDelta = curPos - rearPos;
    posDelta.normalise2();
    m_tangent += TANGENT_INTERP_RATE * (posDelta - m_tangent);
    m_tangent.y = posDelta.y;
    m_tangent.normalise2();

    if (m_tangent.y > -0.05f) {
        setPos(curPos - m_tangent * dist * 0.5f);
    } else {
        setPos(curPos - m_tangent * dist * 0.5f - EGG::Vector3f::ey * 5.0f);
    }

    setMatrixTangentTo(OrthonormalBasis(m_tangent).base(1), m_tangent);
}

/// @addr{0x806D9504}
/// @brief Checks if the car should speed up or slow down based off the rail point's settings
/// @details For each point along the rail, setting 2 is used to specify whether it is a highway
/// point (1) or a local road point (0). Based on the current and next rail point settings,
/// transitions the car to the appropriate state (or does nothing if the road type has not
/// changed).
void ObjectCarTGE::calcState() {
    u16 curPointSpeedSetting = m_railInterpolator->curPoint().setting[1];
    u16 nextPointSpeedSetting = m_railInterpolator->nextPoint().setting[1];

    if (curPointSpeedSetting == 0 && nextPointSpeedSetting == 1) {
        m_nextStateId = 1;
    } else if (curPointSpeedSetting == 1 && nextPointSpeedSetting == 0) {
        m_nextStateId = 2;
    }
}

} // namespace Kinoko::Field
