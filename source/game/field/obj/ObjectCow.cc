#include "ObjectCow.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/Rail.hh"
#include "game/field/RailInterpolator.hh"
#include "game/field/RailManager.hh"

#include "game/kart/KartObject.hh"
#include "game/kart/Reaction.hh"

#include "game/system/RaceManager.hh"
#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

/// @addr{0x806BBEC0}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Sets @ref m_startFrame based off param setting 3.
ObjectCow::ObjectCow(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_startFrame(params.setting(2)) {}

/// @addr{0x806BBF24}
/// @brief Default virtual destructor
ObjectCow::~ObjectCow() = default;

/// @addr{0x806BBF64}
/// @brief copybrief ObjectBase::init()
/// @details Initializes the cow's position, scale, and rotation based off the object parameters.
/// Sets @ref m_interpRate to `0.05f`.
void ObjectCow::init() {
    ASSERT(m_mapObj);
    setScale(m_mapObj->scale());
    setPos(m_mapObj->pos());
    setRot(m_mapObj->rot() * DEG2RAD);
    m_tangent = EGG::Vector3f::ez;
    m_prevTangent = EGG::Vector3f::ez;
    m_up = EGG::Vector3f::ey;
    m_velocity = EGG::Vector3f::zero;
    m_xzSpeed = 0.0f;
    m_tangentAccel = 0.0f;
    m_floorNrm = EGG::Vector3f::ey;
    m_targetPos = pos();
    m_targetDir = EGG::Vector3f::ez;
    m_upForce = EGG::Vector3f::zero;
    m_interpRate = 0.05f;
}

/// @addr{0x806BC2AC}
/// @copybrief ObjectCollidable::onCollision()
/// @param kartObj The kart object that collided with this object
/// @param reactionOnKart The reaction that should be applied to the kart upon collision
/// @param reactionOnObj The reaction that should be applied to the object upon collision
/// @param hitDepth The depth of the collision between the kart and the object
/// @return @ref Kart::Reaction::WallAllSpeed if the kart's speed is below 50%, otherwise @ref
/// Kart::Reaction::KnockbackSomeSpeedLoseItem.
Kart::Reaction ObjectCow::onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
        Kart::Reaction /*reactionOnObj*/, EGG::Vector3f & /*hitDepth*/) {
    return kartObj->speedRatioCapped() < 0.5f ? Kart::Reaction::WallAllSpeed : reactionOnKart;
}

/// @addr{0x806BC87C}
/// @brief Calculates the cow's interaction with floors and walls
/// @details If a collision is detected, the cow's position is adjusted, the floor normal is
/// updated, and @ref m_upForce is set to counteract gravity. If no collision is detected, the cow's
/// upward force is set to zero so that gravity will take effect.
void ObjectCow::calcFloor() {
    constexpr f32 RADIUS = 50.0f;
    constexpr EGG::Vector3f POS_OFFSET = EGG::Vector3f(0.0f, RADIUS, 0.0f);

    CollisionInfo info;

    bool hasCol = CollisionDirector::Instance()->checkSphereFull(RADIUS, pos() + POS_OFFSET,
            EGG::Vector3f::inf, KCL_TYPE_OBJECT_COLLIDABLE, &info, nullptr, 0);

    if (hasCol) {
        addPos(info.tangentOff);

        if (info.floorDist > -std::numeric_limits<f32>::min()) {
            m_floorNrm = info.floorNrm;
        }

        m_velocity.y = 0.0f;
        m_upForce = GRAVITY_FORCE;
    } else {
        m_upForce = EGG::Vector3f::zero;
    }
}

/// @addr{0x806BC6D8}
/// @brief Calculates the cow's new position based on its velocity, acceleration, and forces
/// @details Acceleration is calculated based on the cow's tangent direction, change in tangent
/// direction, and upward force. Acceleration is then added to @ref m_velocity, and @ref
/// GRAVITY_FORCE is subtracted from @ref m_velocity. Zeroes out @ref m_velocity and @ref m_xzSpeed
/// if the cow is moving away from the tangent direction. Finally, the cow's position is adjusted
/// based on the new velocity and @ref m_tangentAccel is cleared.
void ObjectCow::calcPos() {
    EGG::Vector3f accel =
            m_tangent * m_tangentAccel + (m_tangent - m_prevTangent) * m_xzSpeed + m_upForce;
    m_velocity += accel - GRAVITY_FORCE;
    m_xzSpeed = EGG::Mathf::sqrt(m_velocity.x * m_velocity.x + m_velocity.z * m_velocity.z);

    if (m_tangent.z * m_velocity.z + m_tangent.x * m_velocity.x < 0.0f) {
        m_velocity.x = 0.0f;
        m_velocity.z = 0.0f;
        m_xzSpeed = 0.0f;
    }

    addPos(m_velocity);
    m_tangentAccel = 0.0f;
}

/// @addr{0x806BC124}
/// @brief Interpolates the cow's orientation vectors towards their target directions
/// @details Caches the previous frame's tangent to @ref m_prevTangent. Interpolates @ref m_tangent
/// towards @ref m_targetDir with an interpolation rate of @ref m_interpRate. Interpolates @ref m_up
/// towards @ref m_floorNrm with a fixed interpolation rate of `0.1f`. Updates the cow's
/// transformation matrix to align with the new orientation vectors.
void ObjectCow::interpOrientation() {
    constexpr f32 UP_INTERP_RATE = 0.1f;

    m_prevTangent = m_tangent;
    m_tangent = m_tangent.interpolate(m_interpRate, m_targetDir);

    if (m_tangent.squaredLength() > std::numeric_limits<f32>::epsilon()) {
        m_tangent.normalise2();
    } else {
        m_tangent = EGG::Vector3f::ez;
    }

    m_up = m_up.interpolate(UP_INTERP_RATE, m_floorNrm);

    if (m_up.squaredLength() > std::numeric_limits<f32>::epsilon()) {
        m_up.normalise2();
    } else {
        m_up = EGG::Vector3f::ey;
    }

    setMatrixTangentTo(m_up, m_tangent);
}

/// @addr{0x806BCDC4}
/// @brief Sets the target position and direction for the cow
/// @param v The target position
/// @return The distance from the current position to the target position
f32 ObjectCow::setTarget(const EGG::Vector3f &v) {
    m_targetPos = v;
    EGG::Vector3f posDiff = m_targetPos - pos();
    f32 dist = posDiff.normalise();
    m_targetDir = m_targetPos + posDiff * 1000.0f - pos();
    m_targetDir.normalise2();

    return dist;
}

/// @addr{0x806BD080}
/// @copybrief ObjectCow::ObjectCow()
/// @param params The parameters used to initialize the object
ObjectCowLeader::ObjectCowLeader(const System::MapdataGeoObj &params)
    : ObjectCow(params), StateManager(this, STATE_ENTRIES) {}

/// @addr{0x806BD1F8}
/// @brief Default virtual destructor
ObjectCowLeader::~ObjectCowLeader() = default;

/// @addr{0x806BD264}
/// @copybrief ObjectBase::init()
/// @details Initializes the cow leader's rail interpolator to the start of the rail. Sets the cow's
/// position to the rail interpolator's current position. Sets the cow's target position to `10.0f`
/// units forward in the direction of the rail's tangent. Sets the rail speed based on param
/// setting 2. Initializes the cow's internal state and transitions the cow to the roam state.
void ObjectCowLeader::init() {
    constexpr f32 TARGET_DISTANCE = 10.0f;

    ObjectCow::init();
    m_railInterpolator->init(0.0f, 0);
    setPos(m_railInterpolator->curPos());

    setTarget(pos() + m_railInterpolator->curTangentDir() * TARGET_DISTANCE);

    m_railInterpolator->setSpeed(static_cast<f32>(m_mapObj->setting(1)));

    m_railSpeed = 0.0f;
    m_endedRailSegment = false;
    m_eatAnmType = EatAnmType::EatST;
    m_eatFrames = 0;
    m_interpRate = 1.0f;
    m_nextStateId = 2;
}

/// @addr{0x806BD388}
/// @copybrief ObjectBase::calc()
/// @details If the current frame is greater than or equal to the cow's start frame, evaluate's the
/// leader's state machine. Updates the cow's position, velocity, gravity, and floor normal.
/// Interpolates the cow's orientation vectors and updates its transformation matrix accordingly.
void ObjectCowLeader::calc() {
    u32 t = System::RaceManager::Instance()->timer();

    if (t >= m_startFrame) {
        StateManager::calc();
    }

    calcPos();
    calcFloor();
    interpOrientation();
}

/// @addr{0x806BF42C}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectCowLeader::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x806BDCD8}
/// @copybrief ObjectCow::calcFloor()
/// @details Clears the leader's upwards velocity. Sets@ref m_upForce to @ref GRAVITY_FORCE.
/// Finally, sets @ref m_floorNrm to the next rail point's floor normal.
void ObjectCowLeader::calcFloor() {
    m_velocity.y = 0.0f;
    m_upForce = GRAVITY_FORCE;
    m_floorNrm = m_railInterpolator->floorNrm(m_railInterpolator->nextPointIdx());
}

/// @addr{0x806BD6B0}
/// @brief Called when the cow enters the wait state.
/// @details Sets the cow's target position based on the current rail interpolator's position
/// and tangent direction.
void ObjectCowLeader::enterWait() {
    constexpr f32 DISTANCE = 10.0f;

    setTarget(m_railInterpolator->curPos() + m_railInterpolator->curTangentDir() * DISTANCE);
}

/// @addr{0x806BD7D8}
/// @brief Called when the cow enters the eat state.
/// @details Initializes the cow's eating animation and sets the duration by generating a random
/// number in the range `[120, 240]`.
void ObjectCowLeader::enterEat() {
    constexpr u32 BASE_EAT_FRAMES = 120;
    constexpr u32 EAT_FRAMES_RANGE = 240 - BASE_EAT_FRAMES;

    m_eatAnmType = EatAnmType::EatST;
    u32 rand = System::RaceManager::Instance()->random().getU32(EAT_FRAMES_RANGE);
    m_eatFrames = rand + BASE_EAT_FRAMES;
}

/// @addr{0x806BDA1C}
/// @brief Called when the cow enters the roaming state.
/// @details Resets @ref m_endedRailSegment to `false` to indicate that the cow has reached the
/// end of its current rail segment.
void ObjectCowLeader::enterRoam() {
    m_endedRailSegment = false;
}

/// @addr{0x806BD738}
/// @brief Called every frame while the cow is in the wait state.
/// @details Transitions the cow to the eat state once the current frame exceeds the wait
/// duration.
void ObjectCowLeader::calcWait() {
    if (m_currentFrame > m_railInterpolator->curPoint().setting[0]) {
        m_nextStateId = 2;
    }
}

/// @addr{0x806BD84C}
/// @brief Calculates the cow's behavior while in the eat state.
/// @details Manages the transitions between the different eating animation states and updates the
/// cow's state accordingly.
void ObjectCowLeader::calcEat() {
    constexpr u16 EAT_ST_FRAMES = 40;
    constexpr u16 EAT_ED_FRAMES = 60;

    switch (m_eatAnmType) {
    case EatAnmType::EatST: {
        if (m_currentFrame == EAT_ST_FRAMES) {
            m_eatAnmType = EatAnmType::Eat;
        }
    } break;
    case EatAnmType::Eat: {
        if (m_currentFrame > static_cast<u16>(m_eatFrames + EAT_ST_FRAMES)) {
            m_eatAnmType = EatAnmType::EatED;
        }
    } break;
    case EatAnmType::EatED: {
        if (static_cast<u16>(m_eatFrames + EAT_ST_FRAMES + EAT_ED_FRAMES) == m_currentFrame) {
            m_nextStateId = 2;
        }
    } break;
    default:
        break;
    }
}

/// @addr{0x806BDA70}
/// @brief Calculates the cow's behavior while in the roam state.
/// @details If the cow has reached the end of the rail segment, decreases the rail speed by `0.01f`
/// every frame until it comes to a stop. When it comes to a stop, it either transitions to the wait
/// state or the eat state based on the current rail point's settings. Otherwise, if the cow has not
/// reached the end of the rail segment, it increases the rail speed by `0.01f` every frame until
/// reaching the maximum speed of `4.0f`. Updates the rail to reflect the newly derived rail speed.
/// If the cow has reached the end of the segment and the current rail point has specific settings,
/// it sets @ref m_endedRailSegment so that cow will begin coming to a stop on the next frame.
/// Finally, updates the leader's current position and target based on the rail interpolator.
void ObjectCowLeader::calcRoam() {
    constexpr f32 RAIL_ACCEL = 0.1f;
    constexpr f32 RAIL_MAX_SPEED = 4.0f;

    if (m_endedRailSegment) {
        m_railSpeed -= RAIL_ACCEL;

        if (m_railSpeed < 0.0f) {
            m_railSpeed = 0.0f;

            if (m_railInterpolator->curPoint().setting[1] == 0) {
                m_nextStateId = 0;
            } else {
                m_nextStateId = 1;
            }
        }
    } else {
        if (m_railSpeed < RAIL_MAX_SPEED) {
            m_railSpeed += RAIL_ACCEL;
        } else {
            m_railSpeed = RAIL_MAX_SPEED;
        }
    }

    m_railInterpolator->setSpeed(m_railSpeed);

    auto status = m_railInterpolator->calc();
    const auto &curPoint = m_railInterpolator->curPoint();

    if (status == RailInterpolator::Status::SegmentEnd &&
            (curPoint.setting[0] != 0 || curPoint.setting[1] != 0)) {
        m_endedRailSegment = true;
    }

    setPos(m_railInterpolator->curPos() - EGG::Vector3f::ey * 10.0f);
    setTarget(m_railInterpolator->curPos() + m_railInterpolator->curTangentDir() * 10.0f);
}

/// @addr{0x806BDD48}
/// @brief Constructor
/// @param params The parameters used to initialize the object
/// @param pos The initial position offset for the follower cow
/// @param initRot The initial rotation for the follower cow
/// @details Applies the provided position offset to the cow's position and sets its initial yaw
/// based on the given `initRot`.
ObjectCowFollower::ObjectCowFollower(const System::MapdataGeoObj &params, const EGG::Vector3f &pos,
        f32 initRot)
    : ObjectCow(params), StateManager(this, STATE_ENTRIES), m_posOffset(pos), m_rail(nullptr) {
    addPos(m_posOffset);
    setRot(EGG::Vector3f(rot().x, initRot, rot().z));
}

/// @addr{0x806BDFF4}
/// @brief Default virtual destructor
ObjectCowFollower::~ObjectCowFollower() = default;

/// @addr{0x806BE060}
/// @copybrief ObjectBase::init()
/// @details Initializes the cow follower by setting its initial position and orientation relative
/// to the leader's position offset by @ref m_posOffset. Initializes the cow to the waiting state.
void ObjectCowFollower::init() {
    ObjectCow::init();
    addPos(m_posOffset);
    EGG::Vector3f local_1c = m_posOffset;
    local_1c.normalise2();
    setMatrixTangentTo(EGG::Vector3f::ey, local_1c);
    m_nextStateId = 0;

    enterWait();

    m_waitFrames = 0;
    m_bStopping = false;
    m_railSegThreshold = 0.0f;
}

/// @addr{0x806BE1A8}
/// @copybrief ObjectBase::calc()
/// @details If the leader has not started moving yet, then the cow remains in the waiting state and
/// its target position is set to the opposite side of the leader. Otherwise, evaluates the cow's
/// state machine. In either case, the cow's position and orientation are updated accordingly.
void ObjectCowFollower::calc() {
    u32 t = System::RaceManager::Instance()->timer();

    if (t < m_startFrame) {
        calcWait();
        setTarget(pos() + m_posOffset * 2.0f);
    } else {
        StateManager::calc();
    }

    calcPos();
    calcFloor();
    interpOrientation();
}

/// @addr{0x806BF420}
/// @copybrief ObjectBase::loadRail()
/// @details no-op because the herd's rail is managed by @ref ObjectCowLeader.
void ObjectCowFollower::loadRail() {}

/// @addr{0x806BF424}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectCowFollower::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x806BE4E8}
/// @brief Called when the cow enters the wait state.
/// @details Sets the cow's wait duration and rail segment threshold using random values. @ref
/// m_waitFrames will be set in the range `[100, 160]` and @ref m_railSegThreshold will be set
/// in the range `[0.2f, 1.0f]`.
void ObjectCowFollower::enterWait() {
    constexpr u32 BASE_WAIT_FRAMES = 100;
    constexpr u32 WAIT_FRAMES_VARIANCE = 60;
    constexpr f32 BASE_RAIL_THRESHOLD = 0.2f;
    constexpr f32 RAIL_THRESHOLD_VARIANCE = 0.8f;

    auto &rand = System::RaceManager::Instance()->random();
    m_waitFrames = rand.getU32(WAIT_FRAMES_VARIANCE) + BASE_WAIT_FRAMES;
    m_railSegThreshold = BASE_RAIL_THRESHOLD + rand.getF32(RAIL_THRESHOLD_VARIANCE);
}

/// @addr{0x806BE62C}
/// @brief Runs when the cow enters the free roam state
/// @details The cow's top speed while roaming is a random number in the range `[2.0f, 4.0f]`. The
/// cow's facing direction is randomly varied in the range `[10.0f, 20.0f]` (in degrees) biased
/// towards the leader's position. The target position is computed accordingly, based off a distance
/// that is randomly chosen in the range `[400.0f, 700.0f]`.
void ObjectCowFollower::enterFreeRoam() {
    constexpr f32 BASE_WALK_DISTANCE = 400.0f;
    constexpr f32 WALK_DISTANCE_VARIANCE = 300.0f;
    constexpr f32 AVG_ANGLE = DEG2RAD360 * 10.0f;
    STATIC_ASSERT(AVG_ANGLE == 0.34906584f);

    m_bStopping = false;
    auto &rand = System::RaceManager::Instance()->random();

    m_topSpeed = BASE_TOP_SPEED + rand.getF32(TOP_SPEED_VARIANCE);

    f32 dVar2 = AVG_ANGLE + rand.getF32(AVG_ANGLE);

    // Adjust the cow's yaw so that it is biased towards the rail's position.
    // This means the cow will always walk in a sort of zig-zag generally following the rail.
    f32 fVar3 = CheckPointAgainstLineSegment(pos(), m_rail->curPoint().pos, m_rail->curPos());
    f32 angle = fVar3 > 0.0f ? -dVar2 : dVar2;

    EGG::Vector3f dir = RotateXZByYaw(angle, m_tangent);
    dir.y = 0.0f;
    dir.normalise2();

    f32 distance = BASE_WALK_DISTANCE + rand.getF32(WALK_DISTANCE_VARIANCE);
    setTarget(pos() + dir * distance);
}

/// @addr{0x806BE930}
/// @brief Called when the cow enters the follow leader state.
/// @details Resets the stopping flag, sets the interpolation rate to `0.01f`, and determines
/// the cow's top speed using a random value in the range `[2.0f, 4.0f]`.
void ObjectCowFollower::enterFollowLeader() {
    m_bStopping = false;
    m_interpRate = 0.01f;

    auto &rand = System::RaceManager::Instance()->random();
    m_topSpeed = BASE_TOP_SPEED + rand.getF32(TOP_SPEED_VARIANCE);
}

/// @addr{0x806BE580}
/// @brief Called every frame while the cow is in the wait state.
/// @details Transitions the cow to the free roam state once @ref m_waitFrames frames elapse or
/// the cow reaches @ref m_railSegThreshold along the rail segment.
void ObjectCowFollower::calcWait() {
    if (m_currentFrame > m_waitFrames) {
        m_nextStateId = 1;
    }

    if (m_rail->segmentT() > m_railSegThreshold) {
        m_nextStateId = 2;
    }
}

/// @addr{0x806BE794}
/// @brief Calculates the cow's behavior while in the free roam state
/// @details The cow accelerates towards its target position until it reaches its top speed.
/// If it gets within @ref DIST_THRESHOLD of the target, it begins stopping. Otherwise, if the
/// leader has moved past @ref m_railSegThreshold, the follower transitions to the follow state.
void ObjectCowFollower::calcFreeRoam() {
    constexpr f32 ACCEL = 0.1f;

    if (m_bStopping) {
        m_tangentAccel = -ACCEL;

        if (m_xzSpeed == 0.0f) {
            m_nextStateId = 0;
        }
    } else {
        if (m_xzSpeed < m_topSpeed) {
            m_tangentAccel = ACCEL;
        }
    }

    EGG::Vector3f dist = m_targetPos - pos();
    if (dist.x * dist.x + dist.z * dist.z < DIST_THRESHOLD * DIST_THRESHOLD) {
        m_bStopping = true;
    }

    if (m_rail->segmentT() > m_railSegThreshold) {
        m_nextStateId = 2;
    }
}

/// @addr{0x806BE9CC}
/// @brief Calculates the cow's behavior while in the follow state
/// @details The cow accelerates towards the leader until it reaches its top speed.
/// If it gets within @ref DIST_THRESHOLD of the leader, it begins stopping.
void ObjectCowFollower::calcFollowLeader() {
    constexpr f32 ACCEL = 0.1f;
    constexpr f32 ACCEL_INTERP_RATE = 0.05f;

    f32 dist = 0.0f;

    if (m_bStopping) {
        m_tangentAccel = -ACCEL;

        if (m_xzSpeed == 0.0f) {
            m_interpRate = ACCEL_INTERP_RATE;
            m_nextStateId = 0;
        }
    } else {
        dist = setTarget(m_rail->curPos() + m_posOffset);

        if (m_xzSpeed < m_topSpeed) {
            m_interpRate = ACCEL_INTERP_RATE;
            m_tangentAccel = ACCEL;
        }
    }

    if (dist < DIST_THRESHOLD) {
        m_bStopping = true;
    }
}

/// @addr{0x806BEB54}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Loads the leader and follower cows. Computes each follower's initial position and
/// rotation around the leader. The cows initially form a circular formation around the leader, such
/// that each cow is equally spaced apart and is facing away from the leader. Finally, pre-computes
/// all floor normals along the rail so that they can be fetched without having to perform repeat
/// collision checks.
ObjectCowHerd::ObjectCowHerd(const System::MapdataGeoObj &params) : ObjectCollidable(params) {
    constexpr f32 FOLLOWER_SPACING = 600.0f;

    m_leader = EGG::egg_new<ObjectCowLeader>(params);
    m_leader->load();

    u8 followerCount = params.setting(0);
    m_followers = owning_span<ObjectCowFollower *>(followerCount);

    for (u32 i = 0; i < followerCount; ++i) {
        auto *&child = m_followers[i];
        f32 rot = F_TAU / static_cast<f32>(followerCount) * static_cast<f32>(i);
        f32 z = EGG::Mathf::SinFIdx(RAD2FIDX * rot);
        f32 x = EGG::Mathf::CosFIdx(RAD2FIDX * rot);
        EGG::Vector3f pos = EGG::Vector3f(x, 0.0f, z) * FOLLOWER_SPACING;

        child = EGG::egg_new<ObjectCowFollower>(params, pos, rot);
        child->load();
    }

    auto *rail = RailManager::Instance()->rail(static_cast<size_t>(params.pathId()));
    rail->checkSphereFull();
}

/// @addr{0x806BEFEC}
/// @brief Default virtual destructor
ObjectCowHerd::~ObjectCowHerd() = default;

/// @addr{0x806BF02C}
/// @copybrief ObjectBase::init()
/// @details Assigns the leader's rail to each child.
void ObjectCowHerd::init() {
    for (auto *&child : m_followers) {
        child->m_rail = m_leader->m_railInterpolator;
    }
}

/// @addr{0x806BF064}
/// @copybrief ObjectBase::calc()
/// @details Checks for collisions between the cows in the herd. If a follower strays `4000.0f`
/// units away from the leader, it will transition to the follow state.
void ObjectCowHerd::calc() {
    constexpr f32 MAX_DIST = 4000.0f; // Distance at which a cow will return to its leader

    checkIntraCollision();

    for (auto *&follower : m_followers) {
        EGG::Vector3f posDelta = follower->pos() - m_leader->pos();

        if (posDelta.squaredLength() > MAX_DIST * MAX_DIST) {
            follower->m_nextStateId = 2;
        }
    }
}

/// @addr{0x806BF348}
/// @copybrief ObjectBase::createCollision()
/// @details no-op because the herd object itself does not have collision.
void ObjectCowHerd::createCollision() {}

/// @addr{0x806BF34C}
/// @copybrief ObjectBase::loadRail()
/// @details no-op because the herd's rail is managed by @ref ObjectCowLeader.
void ObjectCowHerd::loadRail() {}

/// @addr{0x806BF42C}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectCowHerd::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x806BF114}
/// @brief Prevents cows from walking into each other.
/// @details Checks for collisions between all follower cows and between each follower and the
/// leader cow. Cows are determined to be colliding if the distance between them is less than
/// `400.0f` units. If a collision is detected, the cows are moved apart. This function performs two
/// passes of collision checks: one for follower-to-follower collisions and another for
/// follower-to-leader collisions.
void ObjectCowHerd::checkIntraCollision() {
    constexpr f32 WIDTH = 400.0f;

    for (u32 i = 0; i < m_followers.size() - 1; ++i) {
        auto *iFollower = m_followers[i];

        for (u32 j = i + 1; j < m_followers.size(); ++j) {
            auto *jFollower = m_followers[j];

            EGG::Vector3f posDelta = jFollower->pos() - iFollower->pos();
            f32 length = posDelta.normalise();

            if (length < WIDTH) {
                EGG::Vector3f change = posDelta * (WIDTH - length) * 1.5f;
                jFollower->addPos(change);
                iFollower->subPos(change);
            }
        }
    }

    for (auto *&follower : m_followers) {
        EGG::Vector3f posDelta = m_leader->pos() - follower->pos();
        f32 length = posDelta.normalise();

        if (length < WIDTH) {
            EGG::Vector3f change = posDelta * (WIDTH - length) * 1.5f;

            follower->subPos(change);
        }
    }
}

} // namespace Kinoko::Field
