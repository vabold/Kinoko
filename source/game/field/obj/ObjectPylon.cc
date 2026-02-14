#include "ObjectPylon.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/ObjectDirector.hh"

#include "game/kart/KartObject.hh"

#include "game/system/RaceManager.hh"

namespace Field {

/// @addr{0x8082CAD8}
ObjectPylon::ObjectPylon(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_initPos(m_pos), m_initScale(m_scale), m_initRot(m_rot) {}

/// @addr{0x8082E500}
ObjectPylon::~ObjectPylon() = default;

/// @addr{0x8082CD60}
void ObjectPylon::init() {
    constexpr f32 NEIGHBOR_SQUARE_RADIUS = 2400.0f;

    m_state = State::Idle;
    m_stateStartFrame = 0;
    m_vel.setZero();
    m_numBounces = 0;
    m_neighbors = {nullptr};
    m_flags.setBit(eFlags::Position);
    m_pos.y -= FALL_VEL;

    CollisionInfo info;
    KCLTypeMask mask;
    EGG::Vector3f pos = m_pos + EGG::Vector3f::ey * RADIUS;
    EGG::Vector3f prevPos = m_pos + EGG::Vector3f::ey * (RADIUS + FALL_VEL);

    bool hasCol = CollisionDirector::Instance()->checkSphereFull(RADIUS, pos, prevPos,
            KCL_TYPE_6CEBDFFF, &info, &mask, 0);

    if (hasCol) {
        m_pos += info.tangentOff;
        m_flags.setBit(eFlags::Position);

        if (mask & KCL_TYPE_FLOOR) {
            setMatrixTangentTo(info.floorNrm, EGG::Vector3f::ez);
        }
    }

    auto *objDir = ObjectDirector::Instance();
    auto &managedObjs = objDir->managedObjects();

    for (auto *&obj : managedObjs) {
        if ((obj->pos() - m_pos).length() >= NEIGHBOR_SQUARE_RADIUS) {
            continue;
        }

        // The base game assumes that each managed object is pylon.
        ASSERT(obj->id() == ObjectId::Pylon);
        auto *&pylon = reinterpret_cast<ObjectPylon *&>(obj);
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
        if (System::RaceManager::Instance()->timer() - m_stateStartFrame > STATE_COOLDOWN_FRAMES) {
            m_state = State::Idle;
        }
        break;
    default:
        break;
    }
}

/// @addr{0x8082DBEC}
/// @details If the player's speed ratio is above 70%, then the pylon enters the "Hit" state where
/// it bounces away. Else, it checks to see the angle of impact between the player and the pylon. If
/// the player's angle falls between 30 and 150 degrees, then the cone will reduce the player's
/// speed down to 82%.
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
/// @warning This function can cause time trial desyncs. Higher in the callstack is
/// ObjectDirector::checkKartObjectCollision, which iterates over each object in the spatial cache.
/// For each object, it updates the AABB and checks for collision. If there was a collision, it
/// calls OnCollision which will in turn call this function. This function will then update its
/// position if it finds itself to be colliding with any of its neighbors. However, it is not
/// guaranteed that its neighbors have had their AABBs updated yet for this frame, as they may not
/// be processed until a later iteration in checkKartObjectCollision.
void ObjectPylon::checkIntraCollision(const EGG::Vector3f &hitDepth) {
    constexpr f32 TRAVEL_RADIUS = 1200.0f;

    m_pos -= hitDepth;
    m_flags.setBit(eFlags::Position);

    EGG::Vector3f dist;
    for (auto *&neighbor : m_neighbors) {
        if (neighbor && collision()->check(*neighbor->collision(), dist)) {
            m_pos += dist;
            m_flags.setBit(eFlags::Position);
        }
    }

    EGG::Vector3f delta = m_pos - m_initPos;

    // Enforce that cones stay within a TRAVEL_RADIUS radius from their initial position
    if (delta.length() > TRAVEL_RADIUS) {
        delta.x += hitDepth.z;
        delta.z -= hitDepth.x;
        delta.normalise2();

        m_pos = m_initPos + delta * TRAVEL_RADIUS;
        m_flags.setBit(eFlags::Position);
    }

    m_pos.y -= FALL_VEL;
    m_flags.setBit(eFlags::Position);

    CollisionInfo info;
    KCLTypeMask mask;
    EGG::Vector3f pos = m_pos + EGG::Vector3f::ey * RADIUS;

    bool hasCol = CollisionDirector::Instance()->checkSphereFullPush(RADIUS, pos, m_pos,
            KCL_TYPE_60E8DFFF, &info, &mask, 0);
    if (hasCol) {
        m_pos += info.tangentOff;
        m_flags.setBit(eFlags::Position);

        if (info.floorDist > -std::numeric_limits<f32>::min()) {
            setMatrixTangentTo(info.floorNrm, EGG::Vector3f::ez);
        }
    }
}

/// @addr{0x8082E3F0}
void ObjectPylon::startHit(f32 velFactor, EGG::Vector3f &hitDepth) {
    constexpr f32 ANG_VEL_SCALAR = 0.5f;
    constexpr f32 VEL_SCALAR = 100.0f;

    m_state = State::Hit;
    m_vel = EGG::Vector3f(-hitDepth.x, 0.0f, -hitDepth.z);
    m_vel.normalise2();
    m_angVel = m_vel * velFactor * ANG_VEL_SCALAR;
    m_vel.y = 0.85f;
    m_vel *= velFactor * VEL_SCALAR;
    hitDepth.normalise2();
}

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
    if (m_vel.squaredLength() < SQ_VEL_MIN || m_pos.y < 0.0f ||
            static_cast<f32>(stateFrames) > HIT_DURATION) {
        m_stateStartFrame = t;
        m_state = State::Hiding;
        return;
    }

    CollisionInfo info;
    KCLTypeMask maskOut;
    EGG::Vector3f pos = m_pos + m_vel;

    KCLTypeMask mask = KCL_TYPE_OBJECT_WALL;
    if (stateFrames >= STATE_COOLDOWN_FRAMES) {
        mask |= KCL_TYPE_FLOOR;
    }

    bool hasCol = CollisionDirector::Instance()->checkSphereFullPush(
            (RADIUS + FALL_VEL) * m_scale.x, pos, m_pos, mask, &info, &maskOut, 0);

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

        m_rot += m_angVel;
        m_flags.setBit(eFlags::Rotation);

        m_pos = m_pos + m_vel + info.tangentOff;
        m_flags.setBit(eFlags::Position);

        if (m_numBounces++ >= MAX_BOUNCES) {
            m_state = State::Hiding;
            m_stateStartFrame = t;
        }
    } else {
        calcRotLock();

        m_rot += m_angVel;
        m_pos += m_vel;
        m_flags.setBit(eFlags::Rotation, eFlags::Position);
    }
}

void ObjectPylon::calcHiding() {
    constexpr u32 HIDING_DURATION = 10;

    u32 t = System::RaceManager::Instance()->timer();
    u32 stateFrames = t - m_stateStartFrame;
    if (stateFrames > HIDING_DURATION) {
        m_state = State::Hide;
        m_stateStartFrame = t;
        m_rot = m_initRot;
        m_rotLock = true;
        m_flags.setBit(eFlags::Position);
    } else {
        calcRotLock();

        m_rot += m_angVel;
        m_scale.set(1.0f / static_cast<f32>(stateFrames));
        m_flags.setBit(eFlags::Rotation, eFlags::Scale);
    }

    disableCollision();
}

void ObjectPylon::calcHide() {
    constexpr u32 HIDE_DURATION = 900;

    u32 t = System::RaceManager::Instance()->timer();
    if (t - m_stateStartFrame > HIDE_DURATION) {
        m_state = State::ComeBack;
        m_stateStartFrame = t;
        m_rot = m_initRot;
        m_rotLock = true;
        m_flags.setBit(eFlags::Rotation);
    }
}

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

        m_pos = m_initPos;
        m_scale = m_initScale;
        m_rot = m_initRot;
        m_pos.y -= COME_BACK_VEL;
        m_flags.setBit(eFlags::Position, eFlags::Rotation, eFlags::Scale);
        m_rotLock = true;

        CollisionInfo info;
        KCLTypeMask mask;
        EGG::Vector3f pos = m_pos + EGG::Vector3f::ey * RADIUS * m_scale.x;
        EGG::Vector3f prevPos = m_pos + EGG::Vector3f::ey * (RADIUS + COME_BACK_VEL) * m_scale.x;

        bool hasCol = CollisionDirector::Instance()->checkSphereFullPush(RADIUS * m_scale.x, pos,
                prevPos, KCL_TYPE_60E8DFFF, &info, &mask, 0);

        if (hasCol) {
            m_pos += info.tangentOff;
            m_flags.setBit(eFlags::Position);

            if (mask & KCL_TYPE_FLOOR) {
                setMatrixTangentTo(info.floorNrm, EGG::Vector3f::ez);
                m_state = State::Idle;
            }
        }
    } else {
        m_numBounces = 0;
        m_vel.setZero();
        m_angVel.setZero();
        m_pos = m_initPos +
                EGG::Vector3f::ey * INIT_DISPLACEMENT *
                        static_cast<f32>(COME_BACK_DURATION - stateFrames);
        m_scale = m_initScale;
        m_rot = m_initRot;
        m_flags.setBit(eFlags::Position, eFlags::Scale, eFlags::Rotation);
        m_rotLock = true;

        CollisionInfo info;
        EGG::Vector3f pos = m_pos + EGG::Vector3f::ey * RADIUS * m_scale.x;
        EGG::Vector3f prevPos = m_pos + EGG::Vector3f::ey * (RADIUS + COME_BACK_VEL) * m_scale.x;

        bool hasCol = CollisionDirector::Instance()->checkSphereFull(RADIUS * m_scale.x, pos,
                prevPos, KCL_TYPE_60E8DFFF, &info, nullptr, 0);

        if (hasCol) {
            m_pos += info.tangentOff;
            m_flags.setBit(eFlags::Position);
        }
    }
}

} // namespace Field
