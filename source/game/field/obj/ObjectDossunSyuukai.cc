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
    m_initRotY = rot().y;
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

    setPos(m_railInterpolator->curPos());
}

/// @addr{0x80760D8C}
void ObjectDossunSyuukai::calcRotating() {
    constexpr f32 ANG_VEL = 0.08726646f; /// Approximately 5 degrees
    constexpr f32 BEFORE_FALL_FRAMES = 10;

    addRot(EGG::Vector3f(0.0f, ANG_VEL, 0.0f));

    if (m_state == State::RotatingBeforeStomp) {
        f32 targetRot = m_initRotY;
        if (targetRot < 0.0f) {
            targetRot += F_TAU;
        } else if (targetRot >= F_TAU) {
            targetRot -= F_TAU;
        }

        if (targetRot < rot().y) {
            if (m_rotating) {
                subRot(EGG::Vector3f(0.0f, F_TAU, 0.0f));
            } else {
                setRot(EGG::Vector3f(rot().x, m_initRotY, rot().z));
                m_anmState = AnmState::BeforeFall;
                m_beforeFallTimer = BEFORE_FALL_FRAMES;

                m_currRot = rot().y;
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

        if (targetRot < rot().y) {
            setRot(EGG::Vector3f(rot().x, targetRot, rot().z));
            m_state = State::Moving;
            m_rotating = true;
        }
    }
}

} // namespace Field
