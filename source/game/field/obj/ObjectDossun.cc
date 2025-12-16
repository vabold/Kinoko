#include "ObjectDossun.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/ObjectDirector.hh"

#include "game/kart/KartObject.hh"

namespace Field {

/// @addr{0x80764510}
ObjectDossun::ObjectDossun(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_touchingGround(false) {}

/// @addr{0x8075EE68}
ObjectDossun::~ObjectDossun() = default;

/// @addr{0x8075EEA8}
void ObjectDossun::init() {
    constexpr f32 BEFORE_FALL_VEL = 30.0f;

    m_anmState = AnmState::Still;

    initState();

    if (m_railInterpolator) {
        m_railInterpolator->init(0.0f, 0);
    }

    m_initialPosY = m_pos.y;
    m_vel = 0.0f;

    m_flags.setBit(eFlags::Position);
    for (u32 i = 0; i < BEFORE_FALL_DURATION; ++i) {
        m_pos.y = BEFORE_FALL_VEL + m_pos.y;
    }

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
        EGG::Vector3f pos = m_pos + STOMP_POS_OFFSET;

        hasCol = colDir->checkSphereFull(STOMP_RADIUS, pos, EGG::Vector3f::inf, KCL_TYPE_FLOOR,
                &info, nullptr, 0);
    } while (!hasCol);

    m_pos += info.tangentOff;
    m_flags.setBit(eFlags::Position);
    m_vel = 0.0f;

    f32 fallDuration = frameCount;

    frameCount = 0;
    f32 riseDuration;

    while (true) {
        f32 pos = RISING_VEL + m_pos.y;
        ++frameCount;

        if (pos >= m_initialPosY) {
            m_pos.y = m_initialPosY;
            m_flags.setBit(eFlags::Position);
            riseDuration = frameCount;
            break;
        } else {
            m_pos.y = pos;
            m_flags.setBit(eFlags::Position);
        }
    }

    m_fullDuration = fallDuration + GROUND_DURATION + riseDuration + BEFORE_FALL_DURATION;
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
    mat[1, 3] += HEIGHT * m_scale.x;

    collision()->transform(mat, m_scale);
}

/// @addr{0x8075FF98}
Kart::Reaction ObjectDossun::onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
        Kart::Reaction /*reactionOnObj*/, EGG::Vector3f & /*hitDepth*/) {
    constexpr f32 SQUISH_DISTANCE = 375.0f;

    EGG::Vector3f xzDist = kartObj->pos() - m_pos;
    xzDist.y = 0.0f;

    if (xzDist.length() < SQUISH_DISTANCE * m_scale.x &&
            (m_anmState == AnmState::Falling || m_touchingGround)) {
        const auto &hitTable = ObjectDirector::Instance()->hitTableKart();
        return hitTable.reaction(hitTable.slot(ObjectId::DossuncSoko));
    }

    return reactionOnKart;
}

/// @addr{0x8075F21C}
void ObjectDossun::initState() {
    ASSERT(m_mapObj);
    m_stillTimer = static_cast<s32>(m_mapObj->setting(2));
    if (m_stillTimer == 0) {
        m_stillTimer = static_cast<s32>(m_mapObj->setting(3));
    }

    m_groundedTimer = 0;
    m_beforeFallTimer = 0;
    m_shakePhase = 0;
}

/// @addr{0x8075F254}
void ObjectDossun::calcStomp() {
    switch (m_anmState) {
    case AnmState::BeforeFall:
        calcBeforeFall();
        break;
    case AnmState::Falling:
        calcFalling();
        break;
    case AnmState::Grounded:
        calcGrounded();
        break;
    case AnmState::Rising:
        calcRising();
        break;
    default:
        break;
    }

    if (m_cycleTimer-- == 0) {
        startStill();
    }
}

/// @addr{0x8075F3F4}
void ObjectDossun::calcBeforeFall() {
    constexpr f32 BEFORE_FALL_VEL = 50.0f;

    m_flags.setBit(eFlags::Position);
    m_pos.y = BEFORE_FALL_VEL + m_pos.y;

    if (--m_beforeFallTimer == 0) {
        m_anmState = AnmState::Falling;
    }
}

/// @addr{0x8075F430}
void ObjectDossun::calcFalling() {
    m_flags.setBit(eFlags::Position);
    m_vel -= STOMP_ACCEL;
    m_pos.y = m_vel + m_pos.y;

    checkFloorCollision();
}

/// @addr{0x8075F76C}
void ObjectDossun::checkFloorCollision() {
    CollisionInfo info;
    EGG::Vector3f pos = m_pos + STOMP_POS_OFFSET;

    if (!CollisionDirector::Instance()->checkSphereFull(STOMP_RADIUS, pos, EGG::Vector3f::inf,
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
