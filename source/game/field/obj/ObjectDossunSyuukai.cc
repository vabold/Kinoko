#include "ObjectDossunSyuukai.hh"

namespace Field {

/// @addr{0x80760B20}
ObjectDossunSyuukai::ObjectDossunSyuukai(const System::MapdataGeoObj &params)
    : ObjectDossun(params) {}

/// @addr{0x80764B88}
ObjectDossunSyuukai::~ObjectDossunSyuukai() = default;

/// @addr{0x80760BD4}
void ObjectDossunSyuukai::init() {
    ObjectDossun::init();

    m_state = State::Moving;
    m_initRotY = m_rot.y;
    m_rotating = true;
}

/// @addr{0x80760C5C}
void ObjectDossunSyuukai::calc() {
    m_touchingGround = false;

    switch (m_state) {
    case State::Moving:
        calcMoving();
        break;
    case State::RotatingBeforeStomp:
    case State::RotatingAfterStomp:
        calcRotating();
        break;
    case State::Stomping:
        ObjectDossun::calcStomp();
        break;
    default:
        break;
    }
}

/// @addr{0x80760D18}
void ObjectDossunSyuukai::calcMoving() {
    if (m_railInterpolator->calc() == RailInterpolator::Status::SegmentEnd) {
        m_state = State::RotatingBeforeStomp;
    }

    m_pos = m_railInterpolator->curPos();
    m_flags.setBit(eFlags::Position);
}

/// @addr{0x80760D8C}
void ObjectDossunSyuukai::calcRotating() {
    constexpr f32 ANG_VEL = 0.08726646f; /// Approximately 5 degrees
    constexpr f32 BEFORE_FALL_FRAMES = 10;

    m_flags.setBit(eFlags::Rotation);
    m_rot.y += ANG_VEL;

    if (m_state == State::RotatingBeforeStomp) {
        f32 targetRot = m_initRotY;
        if (targetRot < 0.0f) {
            targetRot += F_TAU;
        } else if (targetRot >= F_TAU) {
            targetRot -= F_TAU;
        }

        if (targetRot < m_rot.y) {
            m_flags.setBit(eFlags::Rotation);

            if (m_rotating) {
                m_rot.y -= F_TAU;
            } else {
                m_rot.y = m_initRotY;
                m_anmState = AnmState::BeforeFall;
                m_beforeFallTimer = BEFORE_FALL_FRAMES;

                m_currRot = m_rot.y;
                if (m_currRot >= F_PI) {
                    m_currRot -= F_TAU;
                }

                m_cycleTimer = m_fullDuration;
                m_state = State::Stomping;
            }
        }

        m_rotating = false;
    } else if (m_state == State::RotatingAfterStomp) {
        const auto &curTan = m_railInterpolator->curTangentDir();
        f32 targetRot = FIDX2RAD * EGG::Mathf::Atan2FIdx(curTan.x, curTan.z);

        if (targetRot < 0.0f) {
            targetRot += F_TAU;
        } else if (targetRot >= F_TAU) {
            targetRot -= F_TAU;
        }

        if (targetRot < m_rot.y) {
            m_flags.setBit(eFlags::Rotation);
            m_rot.y = targetRot;
            m_state = State::Moving;
            m_rotating = true;
        }
    }
}

} // namespace Field
