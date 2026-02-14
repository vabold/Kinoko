#include "ObjectDossunNormal.hh"

namespace Field {

/// @addr{0x80760188}
ObjectDossunNormal::ObjectDossunNormal(const System::MapdataGeoObj &params)
    : ObjectDossun(params) {}

/// @addr{0x80760188}
ObjectDossunNormal::~ObjectDossunNormal() = default;

/// @addr{0x8076023C}
void ObjectDossunNormal::init() {
    ObjectDossun::init();

    m_stompState = StompState::Inactive;
    m_currRot = m_rot.y;

    if (m_currRot <= F_PI) {
        m_currRot -= F_TAU;
    }
}

/// @addr{0x807602E0}
void ObjectDossunNormal::calc() {
    m_touchingGround = false;

    switch (m_stompState) {
    case StompState::Inactive:
        calcInactive();
        break;
    case StompState::Active:
        calcStomp();
        break;
    default:
        break;
    }
}

/// @addr{0x80760820}
void ObjectDossunNormal::startStill() {
    m_anmState = AnmState::Still;
    m_shakePhase = 0;
    m_vel = 0.0f;
    m_flags.setBit(eFlags::Rotation);
    m_rot.y = m_currRot;
    m_stompState = StompState::Inactive;
    ASSERT(m_mapObj);
    m_stillTimer = static_cast<s32>(m_mapObj->setting(3));
}

/// @addr{0x80760964}
void ObjectDossunNormal::startBeforeFall() {
    m_stompState = StompState::Active;
    m_anmState = AnmState::BeforeFall;
    m_beforeFallTimer = static_cast<s32>(BEFORE_FALL_DURATION);
    m_cycleTimer = static_cast<s32>(m_fullDuration);
}

/// @addr{0x80760490}
void ObjectDossunNormal::calcInactive() {
    constexpr s32 SHAKE_DURATION = 30;
    constexpr s32 SHAKE_PHASE_CHANGE = 30;
    constexpr f32 SHAKE_AMPLITUDE = 30.0f;

    if (--m_stillTimer == 0) {
        startBeforeFall();
    }

    if (m_stillTimer <= SHAKE_DURATION) {
        m_shakePhase += SHAKE_PHASE_CHANGE;
        m_flags.setBit(eFlags::Position);
        m_pos.y = m_initialPosY +
                SHAKE_AMPLITUDE * EGG::Mathf::sin(static_cast<f32>(m_shakePhase) * DEG2RAD);
    }
}

} // namespace Field
