#include "ObjectDossunSyuukai.hh"

#include "game/field/RailInterpolator.hh"

namespace Kinoko::Field {

/// @addr{0x80760B20}
/// @copydoc ObjectDossun::ObjectDossun(const System::MapdataGeoObj &)
ObjectDossunSyuukai::ObjectDossunSyuukai(const System::MapdataGeoObj &params)
    : ObjectDossun(params) {}

/// @addr{0x80764B88}
/// @brief Default virtual destructor
ObjectDossunSyuukai::~ObjectDossunSyuukai() = default;

/// @addr{0x80760BD4}
/// @copybrief ObjectBase::init()
/// @details Initializes the Thwomp. including setting its @ref m_state to @ref State::Moving,
/// caching the Thwomp's initial rotation, and setting the @ref m_rotating flag.
void ObjectDossunSyuukai::init() {
    ObjectDossun::init();

    m_state = State::Moving;
    m_initYaw = rot().y;
    m_rotating = true;
}

/// @addr{0x80760C5C}
/// @copybrief ObjectBase::calc()
/// @details Calls the appropriate helper function (@ref calcMoving(), @ref calcRotating(), or
/// @ref ObjectDossun::calcStomp()) based on the Thwomp's current motion state.
void ObjectDossunSyuukai::calc() {
    m_touchingGround = false;

    switch (m_state) {
    case State::Moving:
        calcMoving();
        break;
    case State::RotatingBeforeStomp:
    case State::RotatingBeforeMoving:
        calcRotating();
        break;
    case State::Stomping:
        ObjectDossun::calcStomp();
        break;
    default:
        break;
    }
}

/// @addr{0x8076139C}
/// @copybrief ObjectDossun::startStill()
/// @details Calls @ref ObjectDossun::startStill() to initialize the Thwomp at the beginning of
/// the still state. Also sets the Thwomp's motion state to @ref State::RotatingBeforeMoving.
void ObjectDossunSyuukai::startStill() {
    ObjectDossun::startStill();
    m_state = State::RotatingBeforeMoving;
}

/// @addr{0x80760D18}
/// @brief Runs once per frame while the Thwomp is moving
/// @details Updates the rail interpolator and transitions to the @ref
/// State::RotatingBeforeStomp state if the end of the segment is reached. Finally, updates the
/// Thwomp's position along the rail.
void ObjectDossunSyuukai::calcMoving() {
    if (m_railInterpolator->calc() == RailInterpolator::Status::SegmentEnd) {
        m_state = State::RotatingBeforeStomp;
    }

    setPos(m_railInterpolator->curPos());
}

/// @addr{0x80760D8C}
/// @brief Runs once per frame while the Thwomp is rotating
/// @details Adds 5 degrees to the Thwomp's yaw each frame. The Thwomp rotates every frame until
/// it reaches its target rotation. If the Thwomp is rotating before stomping and reaches its
/// target rotation, it will transition to the @ref State::Stomping state. Otherwise, if the
/// Thwomp is rotating before beginning to move and reaches its target rotation, it will transition
/// to the @ref State::Moving state.
void ObjectDossunSyuukai::calcRotating() {
    constexpr f32 ANG_VEL = 5.0f * DEG2RAD;
    STATIC_ASSERT(ANG_VEL == 0.08726646f);
    constexpr f32 BEFORE_FALL_FRAMES = 10;

    addRot(EGG::Vector3f(0.0f, ANG_VEL, 0.0f));

    if (m_state == State::RotatingBeforeStomp) {
        f32 targetRot = m_initYaw;
        if (targetRot < 0.0f) {
            targetRot += F_TAU;
        } else if (targetRot >= F_TAU) {
            targetRot -= F_TAU;
        }

        if (targetRot < rot().y) {
            if (m_rotating) {
                subRot(EGG::Vector3f(0.0f, F_TAU, 0.0f));
            } else {
                setRot(EGG::Vector3f(rot().x, m_initYaw, rot().z));
                m_anmState = AnmState::BeforeFall;
                m_beforeFallTimer = BEFORE_FALL_FRAMES;

                m_currYaw = rot().y;
                if (m_currYaw >= F_PI) {
                    m_currYaw -= F_TAU;
                }

                m_stompDuration = m_fullDuration;
                m_state = State::Stomping;
            }
        }

        m_rotating = false;
    } else if (m_state == State::RotatingBeforeMoving) {
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

} // namespace Kinoko::Field
