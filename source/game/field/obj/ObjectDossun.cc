#include "ObjectDossun.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/ObjectDirector.hh"

#include "game/kart/KartObject.hh"

namespace Field {

/// @addr{0x80764510}
ObjectDossun::ObjectDossun(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_fallDuration(0) {}

/// @addr{0x8075EE68}
ObjectDossun::~ObjectDossun() = default;

/// @addr{0x8075EEA8}
void ObjectDossun::init() {
    m_anmState = MoveState::Still;

    initState();

    if (m_railInterpolator) {
        m_railInterpolator->init(0.0f, 0);
    }

    m_initialPosY = m_pos.y;
    m_vel = 0.0f;

    m_flags.setBit(eFlags::Position);
    m_pos.y = 30.0f + 30.0f + 30.0f + 30.0f + 30.0f + 30.0f + 30.0f + 30.0f + 30.0f +
            (30.0f + m_pos.y);

    bool hasCol = false;
    auto *colDir = CollisionDirector::Instance();
    CollisionInfo info;
    u32 frameCount = 0;

    do {
        m_flags.setBit(eFlags::Position);
        ++frameCount;
        m_vel -= STOMP_ACCEL;
        m_pos.y = m_vel + m_pos.y;

        info.reset();
        EGG::Vector3f pos = m_pos + EGG::Vector3f(0.0f, 20.0f, 0.0f);

        hasCol = colDir->checkSphereFull(20.0f, pos, EGG::Vector3f::inf, KCL_TYPE_FLOOR, &info,
                nullptr, 0);
    } while (!hasCol);

    m_pos += info.tangentOff;
    m_flags.setBit(eFlags::Position);
    m_vel = 0.0f;
    m_fallDuration = frameCount;

    frameCount = 0;

    while (true) {
        f32 pos = 10.0f + m_pos.y;
        ++frameCount;

        if (pos >= m_initialPosY) {
            m_pos.y = m_initialPosY;
            m_flags.setBit(eFlags::Position);
            m_riseDuration = frameCount;
            break;
        } else {
            m_pos.y = pos;
            m_flags.setBit(eFlags::Position);
        }
    }

    m_fullDuration = m_fallDuration + GROUND_DURATION + m_riseDuration + RISE_DURATION;

    EGG::Matrix34f mat;
    mat.makeR(m_rot);
    m_touchingGround = false;
}

/// @addr{0x807648A4}
void ObjectDossun::calcCollisionTransform() {
    constexpr f32 HEIGHT = 400.0f;

    if (!m_collision) {
        return;
    }

    calcTransform();

    EGG::Matrix34f mat = m_transform;
    mat[1, 3] = m_transform[1, 3] + HEIGHT * m_scale.x;

    collision()->transform(mat, m_scale);
}

/// @addr{0x8075FF98}
Kart::Reaction ObjectDossun::onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
        Kart::Reaction /*reactionOnObj*/, EGG::Vector3f & /*hitDepth*/) {
    constexpr f32 SQUISH_DISTANCE = 375.0f;

    EGG::Vector3f xzDist = kartObj->pos() - m_pos;
    xzDist.y = 0.0f;

    if (xzDist.length() < SQUISH_DISTANCE * m_scale.x &&
            (m_anmState == MoveState::Falling || m_touchingGround)) {
        const auto &hitTable = ObjectDirector::Instance()->hitTableKart();
        return hitTable.reaction(hitTable.slot(SOKO_ID));
    }

    return reactionOnKart;
}

/// @addr{0x8075F21C}
void ObjectDossun::initState() {
    m_stillTimer = static_cast<u32>(m_mapObj->setting(2));
    if (m_stillTimer == 0) {
        m_stillTimer = static_cast<u32>(m_mapObj->setting(3));
    }

    m_groundedTimer = 0;
    m_beforeFallTimer = 0;
    m_shakePhase = 0;
}

/// @addr{0x8075F254}
void ObjectDossun::calcStomp() {
    switch (m_anmState) {
    case MoveState::BeforeFall:
        calcBeforeFall();
        break;
    case MoveState::Falling:
        calcFalling();
        break;
    case MoveState::Grounded: {
        calcGrounded();
    } break;
    case MoveState::Rising: {
        calcRising();
    } break;
    default:
        break;
    }

    if (m_cycleTimer-- == 0) {
        startStill();
    }
}

/// @addr{0x8075FB50}
void ObjectDossun::startStill() {
    m_anmState = MoveState::Still;
    m_shakePhase = 0;
    m_vel = 0.0f;
    m_flags.setBit(eFlags::Rotation);
    m_rot.y = m_currRot;
}

/// @addr{0x8075FE8C}
void ObjectDossun::startGrounded() {
    m_anmState = MoveState::Grounded;
    m_groundedTimer = GROUND_DURATION;
}

/// @addr{0x8075F3F4}
void ObjectDossun::calcBeforeFall() {
    constexpr f32 BEFORE_FALL_VEL = 50.0f;

    m_flags.setBit(eFlags::Position);
    m_pos.y = BEFORE_FALL_VEL + m_pos.y;

    if (--m_beforeFallTimer == 0) {
        m_anmState = MoveState::Falling;
    }
}

/// @addr{0x8075F430}
void ObjectDossun::calcFalling() {
    m_flags.setBit(eFlags::Position);
    m_vel -= STOMP_ACCEL;
    m_pos.y = m_vel + m_pos.y;

    checkFloorCollision();
}

/// @addr{0x8075F460}
void ObjectDossun::calcGrounded() {
    if (--m_groundedTimer == 0) {
        m_anmState = MoveState::Rising;
    }
}

/// @addr{0x8075F4D8}
void ObjectDossun::calcRising() {
    constexpr f32 RISING_VEL = 10.0f;

    m_pos.y = std::min(RISING_VEL + m_pos.y, m_initialPosY);
    m_flags.setBit(eFlags::Position);
}

/// @addr{0x8075F76C}
void ObjectDossun::checkFloorCollision() {
    constexpr f32 RADIUS = 20.0f;
    constexpr EGG::Vector3f POS_OFFSET = EGG::Vector3f(0.0f, RADIUS, 0.0f);

    CollisionInfo info;
    EGG::Vector3f pos = m_pos + POS_OFFSET;

    if (!CollisionDirector::Instance()->checkSphereFull(RADIUS, pos, EGG::Vector3f::inf,
                KCL_TYPE_FLOOR, &info, nullptr, 0)) {
        return;
    }

    m_vel = 0.0f;
    m_pos += info.tangentOff;
    m_flags.setBit(eFlags::Position);
    startGrounded();
    m_touchingGround = true;
}

} // namespace Field
