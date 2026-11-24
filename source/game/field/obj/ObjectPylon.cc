#include "ObjectPylon.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/CollisionInfo.hh"
#include "game/field/ObjectCollisionBase.hh"
#include "game/field/ObjectDirector.hh"

#include "game/kart/KartObject.hh"
#include "game/kart/Reaction.hh"

#include "game/system/RaceManager.hh"

namespace Kinoko::Field {

/// @addr{0x8082CAD8}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Caches the cone's initial position, scale, and rotation to @ref m_initPos, @ref
/// m_initScale, and @ref m_initRot.
ObjectPylon::ObjectPylon(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_initPos(pos()), m_initScale(scale()), m_initRot(rot()) {}

/// @addr{0x8082E500}
/// @brief Default virtual destructor
ObjectPylon::~ObjectPylon() = default;

/// @addr{0x8082CD60}
/// @copybrief ObjectBase::init()
/// @details Checks for floor and wall collision to make sure the pylon is not clipping. Assigns
/// neighbors by taking the first two pylons that have already been loaded that are found within
/// 2400 units.
/// @warning The base game does not apply any ObjectId check when assigning neighbors. Therefore,
/// all managed objects for the race MUST be pylons, otherwise an unsafe `reinterpret_cast` will
/// occur.
/// @note If the cone's initial position is 20 or more units above the floor (assuming a scale of
/// `{1.0f, 1.0f, 1.0f}`), then the cone will be positioned above the floor without actually making
/// contact. Touching the cone will result in it snapping to the ground, but it will hover above the
/// floor again when it respawns.
void ObjectPylon::init() {
    constexpr f32 NEIGHBOR_SQUARE_RADIUS = 2400.0f;

    m_state = State::Idle;
    m_stateStartFrame = 0;
    m_vel.setZero();
    m_numBounces = 0;
    m_neighbors = {nullptr};
    subPos(EGG::Vector3f(0.0f, FALL_VEL, 0.0f));

    CollisionInfo info;
    KCLTypeMask mask;
    EGG::Vector3f colPos = pos() + EGG::Vector3f::ey * RADIUS;
    EGG::Vector3f prevPos = pos() + EGG::Vector3f::ey * (RADIUS + FALL_VEL);

    bool hasCol = CollisionDirector::Instance()->checkSphereFull(RADIUS, colPos, prevPos,
            KCL_TYPE_6CEBDFFF, &info, &mask, 0);

    if (hasCol) {
        addPos(info.tangentOff);

        if (mask & KCL_TYPE_FLOOR) {
            setMatrixTangentTo(info.floorNrm, EGG::Vector3f::ez);
        }
    }

    auto *objDir = ObjectDirector::Instance();
    const auto &managedObjs = objDir->managedObjects();

    for (auto *const &obj : managedObjs) {
        if ((obj->pos() - pos()).length() >= NEIGHBOR_SQUARE_RADIUS) {
            continue;
        }

        // The base game assumes that each managed object is pylon.
        ASSERT(obj->id() == ObjectId::Pylon);
        auto *const &pylon = reinterpret_cast<ObjectPylon *const &>(obj);
        for (auto *&neighbor : m_neighbors) {
            if (!neighbor) {
                neighbor = pylon;
                break;
            }
        }

        for (auto *&neighbor : pylon->m_neighbors) {
            if (!neighbor) {
                neighbor = this;
                break;
            }
        }
    }

    objDir->addManagedObject(this);
}

/// @addr{0x8082D044}
/// @copybrief ObjectBase::calc()
/// @details Calls the appropriate calculation function based on the cone's current @ref m_state.
void ObjectPylon::calc() {
    switch (m_state) {
    case State::Hit:
        calcHit();
        break;
    case State::Hiding:
        calcHiding();
        break;
    case State::Hide:
        calcHide();
        break;
    case State::ComeBack:
        calcComeBack();
        break;
    case State::Moving:
        calcMoving();
        break;
    default:
        break;
    }
}

/// @addr{0x8082E4F8}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectPylon::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x8082DBEC}
/// @copybrief ObjectCollidable::onCollision()
/// @param kartObj The kart object involved in the collision
/// @param hitDepth The depth of the collision along each axis
/// @return @ref Kart::Reaction::WeakWall or @ref Kart::Reaction::None, depending on the @ref
/// m_state of the Pylon, the collision, and the kart's speed
/// @details If the player's speed ratio is at or above 70%, then the pylon enters the "Hit" state
/// where it bounces away. If the pylon is already in the hit state, then returns @ref
/// Kart::Reaction::None so that the pylon does not affect the kart as it flies off. Else, it checks
/// to see the angle of impact between the player and the pylon. If the player's angle falls outside
/// of 30-150 degrees, then the cone will reduce the player's speed down to 82%.
Kart::Reaction ObjectPylon::onCollision(Kart::KartObject *kartObj,
        Kart::Reaction /*reactionOnKart*/, Kart::Reaction /*reactionOnObj*/,
        EGG::Vector3f &hitDepth) {
    constexpr f32 HIT_SPEED_RATIO_THRESHOLD = 0.7f;
    constexpr f32 PI_OVER_SIX = 0.52359879f;
    constexpr f32 FIVE_PI_OVER_SIX = 2.617994f;
    constexpr f32 COME_BACK_HIT_FACTOR = 0.5f;

    u32 t = System::RaceManager::Instance()->timer();

    if (m_state == State::ComeBack) {
        startHit(COME_BACK_HIT_FACTOR, hitDepth);
        m_stateStartFrame = t;
        return Kart::Reaction::WeakWall;
    }

    u32 stateFrame = t - m_stateStartFrame;
    bool canChangeState = stateFrame > STATE_COOLDOWN_FRAMES;
    if (canChangeState && m_state == State::Moving) {
        m_state = State::Idle;
        return Kart::Reaction::WeakWall;
    }

    if (canChangeState && m_state == State::Hit) {
        return Kart::Reaction::WeakWall;
    }

    if (m_state == State::Hit) {
        return Kart::Reaction::None;
    }

    f32 speedRatio = kartObj->speedRatioCapped();
    if ((speedRatio >= HIT_SPEED_RATIO_THRESHOLD && m_state != State::Moving) ||
            m_state == State::Hiding || m_state == State::Hide) {
        if (m_state == State::Idle) {
            startHit(speedRatio, hitDepth);
            m_stateStartFrame = t;
        }

        return Kart::Reaction::WeakWall;
    }

    checkIntraCollision(hitDepth);
    m_state = State::Moving;
    const EGG::Vector3f &zAxis = kartObj->componentZAxis();
    EGG::Vector3f cross = hitDepth.cross(zAxis);
    f32 atan = EGG::Mathf::abs(EGG::Mathf::atan2(cross.length(), hitDepth.dot(zAxis)));

    // Act as a weak wall for angles outside of 30-150 degrees
    if (atan >= PI_OVER_SIX && atan < FIVE_PI_OVER_SIX) {
        return Kart::Reaction::None;
    } else {
        return Kart::Reaction::WeakWall;
    }
}

/// @addr{0x8082E100}
/// @brief Checks collision against floors, walls, and other pylons to prevent clipping
/// @param hitDepth The depth of the kart collision along each axis
/// @details First subtracts the hit depth from the pylon's current position to push it out of the
/// collision. It then checks to see if any neighbors collide with the new position of this pylon,
/// and if so, adjusts THIS pylon's position accordingly. Checks if the pylon has moved outside of
/// its allowed travel radius of `1200.0f` units, and if so, clamps it back within the radius. Next,
/// applies a downward velocity of @ref FALL_VEL units per frame. With the resulting position,
/// performs a collision check against floors and walls, offsetting the pylon on top of the floor if
/// a collision occurs.
/// @desync This function can cause time trial desyncs. Higher in the callstack is
/// @ref ObjectDirector::checkKartObjectCollision, which iterates over each object in the spatial
/// cache. For each object, it updates the AABB and checks for collision. If there was a collision,
/// it calls @ref onCollision() which will in turn call this function. This function will then
/// update its position if it finds itself to be colliding with any of its neighbors. However, it is
/// not guaranteed that its neighbors have had their AABBs updated yet for this frame, as they may
/// not be processed until a later iteration in checkKartObjectCollision. This means that player
/// collision checks with pylons may result in updating ghost pylon AABBs earlier than it would have
/// normally.
void ObjectPylon::checkIntraCollision(const EGG::Vector3f &hitDepth) {
    constexpr f32 TRAVEL_RADIUS = 1200.0f;

    subPos(hitDepth);

    EGG::Vector3f dist;
    for (auto *&neighbor : m_neighbors) {
        if (neighbor && collision()->check(*neighbor->collision(), dist)) {
            addPos(dist);
        }
    }

    EGG::Vector3f delta = pos() - m_initPos;

    // Enforce that cones stay within a TRAVEL_RADIUS radius from their initial position
    if (delta.length() > TRAVEL_RADIUS) {
        delta.x += hitDepth.z;
        delta.z -= hitDepth.x;
        delta.normalise2();

        setPos(m_initPos + delta * TRAVEL_RADIUS);
    }

    subPos(EGG::Vector3f(0.0f, FALL_VEL, 0.0f));

    CollisionInfo info;
    KCLTypeMask mask;
    EGG::Vector3f colPos = pos() + EGG::Vector3f::ey * RADIUS;

    bool hasCol = CollisionDirector::Instance()->checkSphereFullPush(RADIUS, colPos, pos(),
            KCL_TYPE_60E8DFFF, &info, &mask, 0);
    if (hasCol) {
        addPos(info.tangentOff);

        if (info.floorDist > -std::numeric_limits<f32>::min()) {
            setMatrixTangentTo(info.floorNrm, EGG::Vector3f::ez);
        }
    }
}

/// @addr{0x8082E3F0}
/// @brief Runs once when the pylon starts flying after a fast collision with the player
/// @param velFactor Scales down the pylon's velocity depending on the speed of the kart colliding
/// with it
/// @param hitDepth The depth of the kart collision along each axis
/// @details Sets the pylon's @ref m_state to @ref State::Hit. Sets the velocity's direction
/// opposite that of the hit depth, with the Y-component set to zero. Computes @ref m_angVel by
/// multiplying the velocity direction with the provided `velFactor` and a constant `0.5f` scalar.
/// Sets the Y-component of @ref m_vel to `0.85f`. Scales the entire velocity vector by `velFactor *
/// 100.0f`. Finally, normalizes the provided `hitDepth`.
void ObjectPylon::startHit(f32 velFactor, EGG::Vector3f &hitDepth) {
    constexpr f32 ANG_VEL_SCALAR = 0.5f;
    constexpr f32 VEL_SCALAR = 100.0f;
    constexpr f32 INIT_Y_VEL = 0.85f;

    m_state = State::Hit;
    m_vel = EGG::Vector3f(-hitDepth.x, 0.0f, -hitDepth.z);
    m_vel.normalise2();
    m_angVel = m_vel * velFactor * ANG_VEL_SCALAR;
    m_vel.y = INIT_Y_VEL;
    m_vel *= velFactor * VEL_SCALAR;
    hitDepth.normalise2();
}

/// @brief Runs every frame when the pylon is flying after being hit by the player
/// @details Applies a downward gravitational force of `3.0f` to @ref m_vel. If the square of the
/// pylon's velocity is less than `0.5f`, the pylon's y-axis position is negative, or 300 frames
/// have elapsed while in the Hit state, then the pylon will transition to the @ref State::Hiding
/// state and the function returns early. For the first 5 frames of the Hit state, this function
/// only performs collision checks against #KCL_TYPE_OBJECT_WALL. On subsequent frames it checks
/// both walls and floors. When a collision occurs, the cone is redirected away from the direction
/// of impact. If the collision was with the floor, then a velocity dampener reduces the pylon's
/// flying speed by 25%. If 5 collisions have occurred, then the pylon transitions to the @ref
/// State::Hiding state. Regardless of whether a collision occurred, the pylon's position is updated
/// to reflect its new
/// @ref m_vel and its rotation is updated based on @ref m_angVel.
void ObjectPylon::calcHit() {
    constexpr f32 GRAVITY = 3.0f;
    constexpr f32 SQ_VEL_MIN = 0.5f;
    constexpr f32 HIT_DURATION = 300.0f;
    constexpr f32 VEL_DAMPENER = 0.75f;
    constexpr u32 MAX_BOUNCES = 4;
    constexpr f32 SIDEWAYS_SCALAR = 0.6f;
    constexpr f32 FORWARD_SCALAR = 0.4f;
    constexpr f32 MIN_SPEED = 0.0f;

    m_vel.y -= GRAVITY;

    u32 t = System::RaceManager::Instance()->timer();
    u32 stateFrames = t - m_stateStartFrame;
    if (m_vel.squaredLength() < SQ_VEL_MIN || pos().y < 0.0f ||
            static_cast<f32>(stateFrames) > HIT_DURATION) {
        m_stateStartFrame = t;
        m_state = State::Hiding;
        return;
    }

    CollisionInfo info;
    KCLTypeMask maskOut;
    EGG::Vector3f colPos = pos() + m_vel;

    KCLTypeMask mask = KCL_TYPE_OBJECT_WALL;
    if (stateFrames >= STATE_COOLDOWN_FRAMES) {
        mask |= KCL_TYPE_FLOOR;
    }

    bool hasCol = CollisionDirector::Instance()->checkSphereFullPush(
            (RADIUS + FALL_VEL) * scale().x, colPos, pos(), mask, &info, &maskOut, 0);

    if (hasCol) {
        if ((maskOut & KCL_TYPE_FLOOR) && stateFrames > STATE_COOLDOWN_FRAMES) {
            m_vel = AdjustVecForward(SIDEWAYS_SCALAR, FORWARD_SCALAR, MIN_SPEED, m_vel,
                    info.floorNrm);
            m_vel.y = VEL_DAMPENER * -m_vel.y;
        } else if (maskOut & KCL_TYPE_WALL) {
            m_vel = AdjustVecForward(SIDEWAYS_SCALAR, FORWARD_SCALAR, MIN_SPEED, m_vel,
                    info.wallNrm);
        }

        calcRotLock();

        addRot(m_angVel);
        setPos(pos() + m_vel + info.tangentOff);

        if (m_numBounces++ >= MAX_BOUNCES) {
            m_state = State::Hiding;
            m_stateStartFrame = t;
        }
    } else {
        calcRotLock();
        addRot(m_angVel);
        addPos(m_vel);
    }
}

/// @brief Runs every frame that the pylon is shrinking after bouncing
/// @details The pylon shrinks over a duration of 10 frames where the scale is one over the frame.
/// Its rotation continues to be updated based on @ref m_angVel. The pylon's collision is disabled
/// during this state.
void ObjectPylon::calcHiding() {
    constexpr u32 HIDING_DURATION = 10;

    u32 t = System::RaceManager::Instance()->timer();
    u32 stateFrames = t - m_stateStartFrame;
    if (stateFrames > HIDING_DURATION) {
        m_state = State::Hide;
        m_stateStartFrame = t;
        setRot(m_initRot);
    } else {
        calcRotLock();
        addRot(m_angVel);
        f32 scale = 1.0f / static_cast<f32>(stateFrames);
        setScale(scale);
    }

    disableCollision();
}

/// @brief Runs every frame that the pylon is intangible
/// @details Once 900 frames have elapsed, the pylon will transition to the respawning state by
/// setting @ref m_state to @ref State::ComeBack, setting @ref m_stateStartFrame to the current
/// race time, and resetting the cone's rotation to @ref m_initRot.
void ObjectPylon::calcHide() {
    constexpr u32 HIDE_DURATION = 900;

    u32 t = System::RaceManager::Instance()->timer();
    if (t - m_stateStartFrame > HIDE_DURATION) {
        m_state = State::ComeBack;
        m_stateStartFrame = t;
        setRot(m_initRot);
    }
}

/// @brief Runs every frame that the pylon is respawning after being intangible
/// @details First, the pylon's collision is re-enabled.
///
/// If the pylon has been spawning for 10 frames, then the pylon transitions to the @ref State::Idle
/// state. In doing so, it resets @ref m_stateStartFrame to the current race framecount.
/// Additionally, the pylon's position is reset to its initial position with a downward offset of
/// `10.0f` to ensure that a floor collision check will succeed even if the pylon is initialized
/// slightly above the floor. Performs a floor and wall collision check with a radius of `120.0f`
/// times the pylon's scale. If a collision occurred, offsets the pylon's position to push it out of
/// the colliding wall/floor, and if the collision was with the floor, aligns the pylon's tangent to
/// the floor normal.
///
/// If the pylon is still in the come back state (i.e., has not been spawning for 10 frames), it
/// continues to fall towards the floor from an initial height of `900.0f` units above the pylon's
/// initial position. As it falls at `100.0f` units per frame, it performs floor and wall collision
/// checks to ensure it does not clip through the environment.
///
/// Regardless of whether or not the come back state duration has elapsed, @ref m_numBounces, @ref
/// m_vel, and @ref m_angVel are reset to zero, and the pylon's scale and rotation are reset to
/// their initial values.
/// @note If the cone's initial position is 20 or more units above the floor (assuming a scale of
/// `{1.0f, 1.0f, 1.0f}`), then the cone will land above the floor without actually contacting it.
void ObjectPylon::calcComeBack() {
    constexpr u32 COME_BACK_DURATION = 10;
    constexpr f32 COME_BACK_VEL = 10.0f;
    constexpr f32 INIT_DISPLACEMENT = COME_BACK_VEL * static_cast<f32>(COME_BACK_DURATION);

    enableCollision();

    u32 t = System::RaceManager::Instance()->timer();
    u32 stateFrames = t - m_stateStartFrame;

    if (stateFrames > COME_BACK_DURATION) {
        m_numBounces = 0;
        m_state = State::Idle;
        m_stateStartFrame = t;
        m_vel.setZero();
        m_angVel.setZero();

        setPos(EGG::Vector3f(m_initPos.x, m_initPos.y - COME_BACK_VEL, m_initPos.z));
        setScale(m_initScale);
        setRot(m_initRot);

        CollisionInfo info;
        KCLTypeMask mask;
        EGG::Vector3f colPos = pos() + EGG::Vector3f::ey * RADIUS * scale().x;
        EGG::Vector3f prevPos = pos() + EGG::Vector3f::ey * (RADIUS + COME_BACK_VEL) * scale().x;

        bool hasCol = CollisionDirector::Instance()->checkSphereFullPush(RADIUS * scale().x, colPos,
                prevPos, KCL_TYPE_60E8DFFF, &info, &mask, 0);

        if (hasCol) {
            addPos(info.tangentOff);

            if (mask & KCL_TYPE_FLOOR) {
                setMatrixTangentTo(info.floorNrm, EGG::Vector3f::ez);
            }
        }
    } else {
        m_numBounces = 0;
        m_vel.setZero();
        m_angVel.setZero();
        setPos(m_initPos +
                EGG::Vector3f::ey * INIT_DISPLACEMENT *
                        static_cast<f32>(COME_BACK_DURATION - stateFrames));
        setScale(m_initScale);
        setRot(m_initRot);

        CollisionInfo info;
        EGG::Vector3f colPos = pos() + EGG::Vector3f::ey * RADIUS * scale().x;
        EGG::Vector3f prevPos = pos() + EGG::Vector3f::ey * (RADIUS + COME_BACK_VEL) * scale().x;

        bool hasCol = CollisionDirector::Instance()->checkSphereFull(RADIUS * scale().x, colPos,
                prevPos, KCL_TYPE_60E8DFFF, &info, nullptr, 0);

        if (hasCol) {
            addPos(info.tangentOff);
        }
    }
}

/// @brief Runs every frame that the pylon is being pushed by the player
/// @details If 5 frames have elapsed since the cone started moving, transitions the cone back
/// to the @ref State::Idle state.
void ObjectPylon::calcMoving() {
    if (System::RaceManager::Instance()->timer() - m_stateStartFrame > STATE_COOLDOWN_FRAMES) {
        m_state = State::Idle;
    }
}

} // namespace Kinoko::Field
