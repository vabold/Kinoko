#include "ObjectDossunNormal.hh"

#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

/// @addr{0x80760188}
/// @copydoc ObjectDossun::ObjectDossun(const System::MapdataGeoObj &)
/// @details Caches the Thwomp's "still" duration to @ref m_stillDuration based on param
/// setting 4.
ObjectDossunNormal::ObjectDossunNormal(const System::MapdataGeoObj &params)
    : ObjectDossun(params), m_stillDuration(static_cast<s32>(params.setting(3))) {}

/// @addr{0x80760188}
/// @brief Default virtual destructor
ObjectDossunNormal::~ObjectDossunNormal() = default;

/// @addr{0x8076023C}
/// @copybrief ObjectBase::init()
/// @details Initializes the Thwomp's state, including its stomp state and rotation.
void ObjectDossunNormal::init() {
    ObjectDossun::init();

    m_stompState = StompState::Inactive;
    m_currYaw = rot().y;

    if (m_currYaw <= F_PI) {
        m_currYaw -= F_TAU;
    }
}

/// @addr{0x807602E0}
/// @copybrief ObjectBase::calc()
/// @details Clears the @ref m_touchingGround flag. Then, based on whether the Thwomp is
/// currently stomping or not, either calls @ref calcStomp() or @ref calcInactive().
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
/// @copybrief ObjectDossun::startStill()
/// @details Calls @ref ObjectDossun::startStill() to initialize the Thwomp at the beginning of
/// the still state. Also resets the stomp state to @ref StompState::Inactive and sets the @ref
/// m_stillTimer to @ref m_stillDuration.
void ObjectDossunNormal::startStill() {
    ObjectDossun::startStill();
    m_stompState = StompState::Inactive;
    m_stillTimer = m_stillDuration;
}

/// @addr{0x80760490}
/// @brief Runs once per frame when the Thwomp is not stomping down or resetting
/// @details Causes the Thwomp to shake for 30 frames in a sinusoidal motion with an amplitude
/// of `30.0f` before stomping down.
void ObjectDossunNormal::calcInactive() {
    constexpr s32 SHAKE_DURATION = 30;
    constexpr f32 SHAKE_AMPLITUDE = 30.0f;

    if (--m_stillTimer == 0) {
        startBeforeFall();
    }

    if (m_stillTimer <= SHAKE_DURATION) {
        m_shakePhase += SHAKE_DURATION;
        f32 posY = m_initialPosY +
                SHAKE_AMPLITUDE * EGG::Mathf::sin(static_cast<f32>(m_shakePhase) * DEG2RAD);
        setPos(EGG::Vector3f(pos().x, posY, pos().z));
    }
}

/// @addr{0x80760964}
/// @brief Runs once when the Thwomp begins rising before stomping down
/// @details Sets the stomp state to @ref StompState::Active. Sets the animation state to @ref
/// AnmState::BeforeFall. Initializes @ref m_beforeFallTimer to @ref BEFORE_FALL_DURATION, and
/// sets @ref m_stompDuration to @ref m_fullDuration.
void ObjectDossunNormal::startBeforeFall() {
    m_stompState = StompState::Active;
    m_anmState = AnmState::BeforeFall;
    m_beforeFallTimer = static_cast<s32>(BEFORE_FALL_DURATION);
    m_stompDuration = static_cast<s32>(m_fullDuration);
}

} // namespace Kinoko::Field
