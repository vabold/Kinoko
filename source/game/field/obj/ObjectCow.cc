#include "ObjectCow.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/RailInterpolator.hh"
#include "game/field/RailManager.hh"

#include "game/kart/KartCollide.hh"
#include "game/kart/KartObject.hh"

#include "game/system/RaceManager.hh"

namespace Field {

/// @addr{0x806BBEC0}
ObjectCow::ObjectCow(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_startFrame(static_cast<s32>(params.setting(2))) {}

/// @addr{0x806BBF24}
ObjectCow::~ObjectCow() = default;

/// @addr{0x806BC2AC}
Kart::Reaction ObjectCow::onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
        Kart::Reaction /*reactionOnObj*/, EGG::Vector3f & /*hitDepth*/) {
    return kartObj->speedRatioCapped() < 0.5f ? Kart::Reaction::WallAllSpeed : reactionOnKart;
}

/// @addr{0x806BBF64}
void ObjectCow::setup() {
    m_scale = m_mapObj->scale();
    m_pos = m_mapObj->pos();
    m_rot = m_mapObj->rot() * DEG2RAD;
    m_flags.setBit(eFlags::Position, eFlags::Rotation, eFlags::Scale);
    m_tangent = EGG::Vector3f::ez;
    m_prevTangent = EGG::Vector3f::ez;
    m_up = EGG::Vector3f::ey;
    m_velocity = EGG::Vector3f::zero;
    m_xzSpeed = 0.0f;
    m_tangentFactor = 0.0f;
    m_floorNrm = EGG::Vector3f::ey;
    m_state1TargetPos = m_pos;
    m_targetDir = EGG::Vector3f::ez;
    m_upForce = EGG::Vector3f::zero;
    m_interpRate = 0.05f;
}

/// @addr{0x806BC87C}
void ObjectCow::calcFloor() {
    constexpr f32 RADIUS = 50.0f;
    constexpr EGG::Vector3f POS_OFFSET = EGG::Vector3f(0.0f, RADIUS, 0.0f);

    CollisionInfo info;

    bool hasCol = CollisionDirector::Instance()->checkSphereFull(RADIUS, m_pos + POS_OFFSET,
            EGG::Vector3f::inf, KCL_TYPE_64EBDFFF, &info, nullptr, 0);

    if (hasCol) {
        m_pos += info.tangentOff;
        m_flags.setBit(eFlags::Position);

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
void ObjectCow::calcPos() {
    EGG::Vector3f accel =
            m_tangent * m_tangentFactor + (m_tangent - m_prevTangent) * m_xzSpeed + m_upForce;
    m_velocity += accel - GRAVITY_FORCE;
    m_xzSpeed = EGG::Mathf::sqrt(m_velocity.x * m_velocity.x + m_velocity.z * m_velocity.z);

    if (m_tangent.z * m_velocity.z + m_tangent.x * m_velocity.x < 0.0f) {
        m_velocity.x = 0.0f;
        m_velocity.z = 0.0f;
        m_xzSpeed = 0.0f;
    }

    m_pos += m_velocity;
    m_flags.setBit(eFlags::Position);
    m_tangentFactor = 0.0f;
}

/// @addr{0x806BCDC4}
f32 ObjectCow::setTarget(const EGG::Vector3f &v) {
    m_state1TargetPos = v;
    EGG::Vector3f posDiff = m_state1TargetPos - m_pos;
    f32 dist = posDiff.normalise();
    m_targetDir = m_state1TargetPos + posDiff * 1000.0f - m_pos;
    m_targetDir.normalise2();

    return dist;
}

/// @addr{0x806BD080}
ObjectCowLeader::ObjectCowLeader(const System::MapdataGeoObj &params)
    : ObjectCow(params), StateManager(this, STATE_ENTRIES) {}

/// @addr{0x806BD1F8}
ObjectCowLeader::~ObjectCowLeader() = default;

/// @addr{0x806BD264}
void ObjectCowLeader::init() {
    setup();
    m_railInterpolator->init(0.0f, 0);
    m_pos = m_railInterpolator->curPos();
    m_flags.setBit(eFlags::Position);

    setTarget(m_pos + m_railInterpolator->curTangentDir() * 10.0f);

    ASSERT(m_mapObj);
    m_railInterpolator->setCurrVel(static_cast<f32>(m_mapObj->setting(1)));

    m_railSpeed = 0.0f;
    m_endedRailSegment = false;
    m_state1AnmType = AnmType::EatST;
    m_eatFrames = 0;
    m_interpRate = 1.0f;
    m_nextStateId = 2;
}

/// @addr{0x806BD388}
void ObjectCowLeader::calc() {
    u32 t = System::RaceManager::Instance()->timer();

    if (t >= static_cast<u32>(m_startFrame)) {
        StateManager::calc();
    }

    calcPos();
    calcFloor();

    m_prevTangent = m_tangent;
    m_tangent = Interpolate(m_interpRate, m_tangent, m_targetDir);

    if (m_tangent.squaredLength() > std::numeric_limits<f32>::epsilon()) {
        m_tangent.normalise2();
    } else {
        m_tangent = EGG::Vector3f::ez;
    }

    m_up = Interpolate(0.1f, m_up, m_floorNrm);

    if (m_up.squaredLength() > std::numeric_limits<f32>::epsilon()) {
        m_up.normalise2();
    } else {
        m_up = EGG::Vector3f::ey;
    }

    setMatrixTangentTo(m_up, m_tangent);
}

/// @addr{0x806BDCD8}
void ObjectCowLeader::calcFloor() {
    m_velocity.y = 0.0f;
    m_upForce = GRAVITY_FORCE;
    m_floorNrm = m_railInterpolator->floorNrm(m_railInterpolator->nextPointIdx());
}

/// @addr{0x806BD6B0}
void ObjectCowLeader::enterWait() {
    setTarget(m_railInterpolator->curPos() + m_railInterpolator->curTangentDir() * 10.0f);
}

/// @addr{0x806BD7D8}
void ObjectCowLeader::enterEat() {
    m_state1AnmType = AnmType::EatST;
    u32 rand = System::RaceManager::Instance()->random().getU32(120);
    m_eatFrames = rand + 120;
}

/// @addr{0x806BDA1C}
void ObjectCowLeader::enterRoam() {
    m_endedRailSegment = false;
}

/// @addr{0x806BD738}
void ObjectCowLeader::calcWait() {
    if (m_currentFrame > m_railInterpolator->curPoint().setting[0]) {
        m_nextStateId = 2;
    }
}

/// @addr{0x806BD84C}
void ObjectCowLeader::calcEat() {
    constexpr u16 EAT_ST_FRAMES = 40;
    constexpr u16 EAT_ED_FRAMES = 60;

    switch (m_state1AnmType) {
    case AnmType::EatST: {
        if (m_currentFrame == EAT_ST_FRAMES) {
            m_state1AnmType = AnmType::Eat;
        }
    } break;
    case AnmType::Eat: {
        if (m_currentFrame > static_cast<u16>(m_eatFrames + EAT_ST_FRAMES)) {
            m_state1AnmType = AnmType::EatED;
        }
    } break;
    case AnmType::EatED: {
        if (static_cast<u16>(m_eatFrames + EAT_ST_FRAMES + EAT_ED_FRAMES) == m_currentFrame) {
            m_nextStateId = 2;
        }
    } break;
    default:
        break;
    }
}

/// @addr{0x806BDA70}
void ObjectCowLeader::calcRoam() {
    if (m_endedRailSegment) {
        m_railSpeed -= 0.1f;

        if (m_railSpeed < 0.0f) {
            m_railSpeed = 0.0f;

            if (m_railInterpolator->curPoint().setting[1] == 0) {
                m_nextStateId = 0;
            } else {
                m_nextStateId = 1;
            }
        }
    } else {
        if (m_railSpeed < 4.0f) {
            m_railSpeed += 0.1f;
        } else {
            m_railSpeed = 4.0f;
        }
    }

    m_railInterpolator->setCurrVel(m_railSpeed);

    auto status = m_railInterpolator->calc();
    const auto &curPoint = m_railInterpolator->curPoint();

    if (status == RailInterpolator::Status::SegmentEnd &&
            (curPoint.setting[0] != 0 || curPoint.setting[1] != 0)) {
        m_endedRailSegment = true;
    }

    m_pos = m_railInterpolator->curPos() - EGG::Vector3f::ey * 10.0f;
    m_flags.setBit(eFlags::Position);

    setTarget(m_railInterpolator->curPos() + m_railInterpolator->curTangentDir() * 10.0f);
}

/// @addr{0x806BDD48}
ObjectCowFollower::ObjectCowFollower(const System::MapdataGeoObj &params, const EGG::Vector3f &pos,
        f32 rot)
    : ObjectCow(params), StateManager(this, STATE_ENTRIES), m_posOffset(pos), m_rail(nullptr) {
    m_pos += m_posOffset;
    m_flags.setBit(eFlags::Position, eFlags::Rotation);
    m_rot.y = rot;
}

/// @addr{0x806BDFF4}
ObjectCowFollower::~ObjectCowFollower() = default;

/// @addr{0x806BE060}
void ObjectCowFollower::init() {
    setup();
    m_pos += m_posOffset;
    m_flags.setBit(eFlags::Position);
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
void ObjectCowFollower::calc() {
    u32 t = System::RaceManager::Instance()->timer();

    if (t < static_cast<u32>(m_startFrame)) {
        if (m_currentFrame > m_waitFrames) {
            m_nextStateId = 1;
        }

        if (m_rail->segmentT() > m_railSegThreshold) {
            m_nextStateId = 2;
        }

        setTarget(m_pos + m_posOffset * 2.0f);
    } else {
        StateManager::calc();
    }

    calcPos();
    calcFloor();

    m_prevTangent = m_tangent;
    m_tangent = Interpolate(m_interpRate, m_tangent, m_targetDir);

    if (m_tangent.squaredLength() > std::numeric_limits<f32>::epsilon()) {
        m_tangent.normalise2();
    } else {
        m_tangent = EGG::Vector3f::ez;
    }

    m_up = Interpolate(0.1f, m_up, m_floorNrm);

    if (m_up.squaredLength() > std::numeric_limits<f32>::epsilon()) {
        m_up.normalise2();
    } else {
        m_up = EGG::Vector3f::ey;
    }

    setMatrixTangentTo(m_up, m_tangent);
}

/// @addr{0x806BE4E8}
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
    f32 fVar3 = CheckPointAgainstLineSegment(m_pos, m_rail->curPoint().pos, m_rail->curPos());
    f32 angle = fVar3 > 0.0f ? -dVar2 : dVar2;

    EGG::Vector3f dir = RotateXZByYaw(angle, m_tangent);
    dir.y = 0.0f;
    dir.normalise2();

    f32 distance = BASE_WALK_DISTANCE + rand.getF32(WALK_DISTANCE_VARIANCE);
    setTarget(m_pos + dir * distance);
}

/// @addr{0x806BE930}
void ObjectCowFollower::enterFollowLeader() {
    m_bStopping = false;
    m_interpRate = 0.01f;

    auto &rand = System::RaceManager::Instance()->random();
    m_topSpeed = BASE_TOP_SPEED + rand.getF32(TOP_SPEED_VARIANCE);
}

/// @addr{0x806BE580}
void ObjectCowFollower::calcWait() {
    if (m_currentFrame > m_waitFrames) {
        m_nextStateId = 1;
    }

    if (m_rail->segmentT() > m_railSegThreshold) {
        m_nextStateId = 2;
    }
}

/// @addr{0x806BE794}
void ObjectCowFollower::calcFreeRoam() {
    if (m_bStopping) {
        m_tangentFactor = -0.1f;

        if (m_xzSpeed == 0.0f) {
            m_nextStateId = 0;
        }
    } else {
        if (m_xzSpeed < m_topSpeed) {
            m_tangentFactor = 0.1f;
        }
    }

    EGG::Vector3f local_28 = m_state1TargetPos - m_pos;
    if (local_28.x * local_28.x + local_28.z * local_28.z < DIST_THRESHOLD * DIST_THRESHOLD) {
        m_bStopping = true;
    }

    if (m_rail->segmentT() > m_railSegThreshold) {
        m_nextStateId = 2;
    }
}

/// @addr{0x806BE9CC}
void ObjectCowFollower::calcFollowLeader() {
    f32 dist = 0.0f;

    if (m_bStopping) {
        m_tangentFactor = -0.1f;

        if (m_xzSpeed == 0.0f) {
            m_interpRate = 0.05f;
            m_nextStateId = 0;
        }
    } else {
        dist = setTarget(m_rail->curPos() + m_posOffset);

        if (m_xzSpeed < m_topSpeed) {
            m_interpRate = 0.05f;
            m_tangentFactor = 0.1f;
        }
    }

    if (dist < DIST_THRESHOLD) {
        m_bStopping = true;
    }
}

/// @addr{0x806BEB54}
ObjectCowHerd::ObjectCowHerd(const System::MapdataGeoObj &params) : ObjectCollidable(params) {
    constexpr f32 FOLLOWER_SPACING = 600.0f;

    m_leader = new ObjectCowLeader(params);
    m_leader->load();

    u8 followerCount = static_cast<u8>(params.setting(0));
    m_followers =
            std::span<ObjectCowFollower *>(new ObjectCowFollower *[followerCount], followerCount);

    for (u32 i = 0; i < followerCount; ++i) {
        auto *&child = m_followers[i];
        f32 rot = F_TAU / static_cast<f32>(followerCount) * static_cast<f32>(i);
        f32 z = EGG::Mathf::SinFIdx(RAD2FIDX * rot);
        f32 x = EGG::Mathf::CosFIdx(RAD2FIDX * rot);
        EGG::Vector3f pos = EGG::Vector3f(x, 0.0f, z) * FOLLOWER_SPACING;

        child = new ObjectCowFollower(params, pos, rot);
        child->load();
    }

    auto *rail = RailManager::Instance()->rail(static_cast<size_t>(params.pathId()));
    rail->checkSphereFull();
}

/// @addr{0x806BEFEC}
ObjectCowHerd::~ObjectCowHerd() {
    delete[] m_followers.data();
}

/// @addr{0x806BF02C}
/// @brief Assigns the herd's rail to each child.
void ObjectCowHerd::init() {
    for (auto *&child : m_followers) {
        child->m_rail = m_leader->m_railInterpolator;
    }
}

/// @addr{0x806BF064}
void ObjectCowHerd::calc() {
    constexpr f32 MAX_DIST = 4000.0f; ///< Distance at which a Cow will return to its leader

    checkCollision();

    for (auto *&follower : m_followers) {
        EGG::Vector3f posDelta = follower->pos() - m_leader->pos();

        if (posDelta.squaredLength() > MAX_DIST * MAX_DIST) {
            follower->m_nextStateId = 2;
        }
    }
}

/// @addr{0x806BF114}
/// @brief Prevents cows from walking into each other.
void ObjectCowHerd::checkCollision() {
    constexpr f32 WIDTH = 400.0f;

    for (u32 i = 0; i < m_followers.size() - 1; ++i) {
        auto *iFollower = m_followers[i];

        for (u32 j = i + 1; j < m_followers.size(); ++j) {
            auto *jFollower = m_followers[j];

            EGG::Vector3f posDelta = jFollower->pos() - iFollower->pos();
            f32 length = posDelta.normalise();

            if (length < WIDTH) {
                EGG::Vector3f change = posDelta * (WIDTH - length) * 1.5f;

                jFollower->m_pos += change;
                jFollower->m_flags.setBit(eFlags::Position);

                iFollower->m_pos -= change;
                iFollower->m_flags.setBit(eFlags::Position);
            }
        }
    }

    for (auto *&follower : m_followers) {
        EGG::Vector3f posDelta = m_leader->pos() - follower->pos();
        f32 length = posDelta.normalise();

        if (length < WIDTH) {
            EGG::Vector3f change = posDelta * (WIDTH - length) * 1.5f;

            follower->m_pos -= change;
            follower->m_flags.setBit(eFlags::Position);
        }
    }
}

} // namespace Field
