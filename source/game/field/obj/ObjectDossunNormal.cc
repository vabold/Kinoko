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
    constexpr u32 SHAKE_DURATION = 30;
    constexpr f32 SHAKE_AMPLITUDE = 30.0f;

    m_touchingGround = false;

    if (m_stompState == StompState::Inactive) {
        if (--m_stillTimer == 0) {
            startBeforeFall();
        }

        if (m_stillTimer <= SHAKE_DURATION) {
            m_shakePhase += 30;
            m_flags.setBit(eFlags::Position);
            m_pos.y = m_initialPosY +
                    SHAKE_AMPLITUDE * EGG::Mathf::sin(static_cast<f32>(m_shakePhase) * DEG2RAD);
        }
    } else if (m_stompState == StompState::Active) {
        calcStomp();
    }
}

/// @addr{0x80760820}
void ObjectDossunNormal::startStill() {
    m_anmState = MoveState::Still;
    m_shakePhase = 0;
    m_vel = 0.0f;
    m_flags.setBit(eFlags::Rotation);
    m_rot.y = m_currRot;
    m_stompState = StompState::Inactive;
    m_stillTimer = static_cast<s32>(static_cast<s16>(m_mapObj->setting(3)));
}

/// @addr{0x80760964}
void ObjectDossunNormal::startBeforeFall() {
    m_stompState = StompState::Active;
    m_anmState = MoveState::BeforeFall;
    m_beforeFallTimer = 10;
    m_cycleTimer = static_cast<s32>(m_fullDuration);
}

} // namespace Field
