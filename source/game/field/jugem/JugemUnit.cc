#include "JugemUnit.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/CollisionInfo.hh"
#include "game/field/jugem/JugemMove.hh"
#include "game/field/jugem/JugemSwitch.hh"

#include "game/kart/KartMove.hh"
#include "game/kart/KartObject.hh"

namespace Kinoko::Field {

/// @addr{Inlined in 0x80721514}
/// @brief Constructor which initializes an array of keyframes
/// @param count The number of keyframes
JugemInterp::JugemInterp(u32 count) {
    m_keyframes = owning_span<InterpKeyframe>(count);
    m_time = 0.0f;
    m_rate = 0.0f;
}

/// @addr{0x80721E64}
/// @brief Default destructor
JugemInterp::~JugemInterp() = default;

/// @addr{0x8072370C}
/// @brief Initializes the keyframes for the interpolation sequence
/// @param startRate The interpolation rate for the first keyframe
/// @param endRate The interpolation rate for the last keyframe
/// @param tDelta The time delta between the first and last keyframe
void JugemInterp::initKeyframes(f32 startRate, f32 endRate, f32 tDelta) {
    m_numKeyframes = 0;

    setStartKeyframe(startRate);
    addKeyframe(endRate, tDelta);

    m_rate = startRate;
    m_time = 0.0f;
}

/// @addr{0x80723AF8}
/// @brief Advances the interpolation sequence and calculates the current interpolation rate
/// @param tDelta The time delta to advance the interpolation sequence
/// @return Whether the interpolation sequence has reached the end of the last keyframe
bool JugemInterp::calc(f32 tDelta) {
    m_rate = calcInterpRate(m_time);
    m_time += tDelta;

    bool endOfKeyframe = m_time > m_keyframes[1].m_time;
    if (endOfKeyframe) {
        m_time = m_keyframes[1].m_time;
    }

    return endOfKeyframe;
}

/// @addr{0x8074C048}
/// @brief Adds a keyframe at time 0 with the specified interpolation rate
/// @param rate The interpolation rate for the keyframe
void JugemInterp::setStartKeyframe(f32 rate) {
    m_keyframes[m_numKeyframes].m_interpRate = rate;
    m_keyframes[m_numKeyframes].m_time = 0.0f;
    ++m_numKeyframes;
}

/// @addr{0x8074C0B4}
/// @brief Adds a keyframe with the specified interpolation rate and time delta
/// @param rate The interpolation rate for the keyframe
/// @param tDelta The time delta from the previous keyframe to this keyframe
void JugemInterp::addKeyframe(f32 rate, f32 tDelta) {
    m_keyframes[m_numKeyframes].m_interpRate = rate;
    m_keyframes[m_numKeyframes].m_time = tDelta + m_keyframes[m_numKeyframes - 1].m_time;
    ++m_numKeyframes;
}

/// @addr{0x8074C1E0}
/// @brief Calculates the current interpolation rate based on the current time and keyframes
/// @param time The current frame in the interpolation sequence
/// @return The current interpolation rate
f32 JugemInterp::calcInterpRate(f32 time) const {
    const InterpKeyframe *currKeyframe = nullptr;
    const InterpKeyframe *nextKeyframe = nullptr;

    for (u32 idx = 0; idx < m_numKeyframes - 1; ++idx) {
        currKeyframe = &m_keyframes[idx];
        nextKeyframe = &m_keyframes[idx + 1];
        if (currKeyframe->m_time <= time && time < nextKeyframe->m_time) {
            break;
        }
    }

    f32 timePastKeyframe = time - currKeyframe->m_time;
    f32 keyframeDuration = nextKeyframe->m_time - currKeyframe->m_time;

    timePastKeyframe = std::clamp(timePastKeyframe, 0.0f, keyframeDuration);

    return SmoothLerp(currKeyframe->m_interpRate, nextKeyframe->m_interpRate,
            timePastKeyframe / keyframeDuration);
}

/// @addr{0x8074C3F0}
/// @brief Performs a smooth interpolation between two values based on a parameter t
/// @param a The starting value
/// @param b The ending value
/// @param t The interpolation parameter `[0.0f, 1.0f]`
/// @return The interpolated value
f32 JugemInterp::SmoothLerp(f32 a, f32 b, f32 t) {
    f32 sin = EGG::Mathf::SinFIdx(RAD2FIDX * (-HALF_PI + t * (HALF_PI - -HALF_PI)));
    f32 interpFactor = std::clamp(0.5f * (1.0f + sin), 0.0f, 1.0f);

    return a + interpFactor * (b - a);
}

/// @addr{0x80721514}
/// @brief Constructs the movement and interpolation controllers for the Lakitu unit
/// @param kartObj Pointer to the player's kart object
/// @details Saves a pointer to the @ref Kart::KartObject to @ref m_kartObj. Initializes @ref
/// m_switchReverse to `nullptr`. Constructs @ref m_move and @ref m_interp with two keyframes.
JugemUnit::JugemUnit(const Kart::KartObject *kartObj)
    : StateManager(this, STATE_ENTRIES), m_kartObj(kartObj), m_switchReverse(nullptr) {
    m_move = EGG::egg_new<JugemMove>(kartObj);
    m_interp = EGG::egg_new<JugemInterp>(2);
}

/// @addr{0x80721D2C}
/// @brief Virtual destructor that destroys the switch, movement, and interpolation controllers
JugemUnit::~JugemUnit() {
    EGG::egg_delete(m_switchReverse);
    EGG::egg_delete(m_move);
    EGG::egg_delete(m_interp);
}

/// @addr{0x80722100}
/// @brief Initializes the internal state of the Lakitu unit
/// @details Initializes the reverse switch, resets @ref m_pos to zero, and initializes the movement
/// controller.
void JugemUnit::init() {
    ASSERT(m_switchReverse);
    m_switchReverse->init();

    m_pos.setZero();
    m_move->init();
}

/// @addr{0x80721EC0}
/// @brief Creates the switches for the Lakitu
/// @details For the purposes of syncing time trials in Kinoko, we only have to implement the
/// reverse switch.
void JugemUnit::createSwitchRace() {
    m_switchReverse = EGG::egg_new<JugemSwitchReverse>();
}

/// @addr{0x807221C4}
/// @brief Updates switches, runs the state machine, and updates Lakitu's position if not idle
void JugemUnit::calc() {
    calcSwitches();

    StateManager::calc();

    // Perform a collision check if Lakitu is not idle
    if (m_currentStateId != 0) {
        setPosFromTransform(m_move->transform());
        calcCollision();
    }
}

/// @addr{0x80723458}
/// @brief Runs once when the Lakitu finishes ascending
/// @details Sets @ref m_state to @ref State::Away and resets @ref m_ascendTimer to zero.
void JugemUnit::enterIdle() {
    m_state = State::Away;
    m_ascendTimer = 0;
}

/// @addr{0x80724794}
/// @brief Runs once when the reverse switch is toggled and the Lakitu begins descending
/// @details Sets @ref m_state to @ref State::Descending. Initializes the interpolator's keyframes
/// and the movement controller. Sets the initial position of the Lakitu based on a local offset
/// `2000.0f` units above the player.
void JugemUnit::enterReverse() {
    constexpr EGG::Vector3f INIT_POS_OFFSET = EGG::Vector3f(0.0f, 2000.0f, 0.0f);

    m_state = State::Descending;
    m_interp->initKeyframes(0.0f, 1.0f, 40.0f);
    m_move->init();
    EGG::Vector3f pos = transformLocalToWorldUpright(INIT_POS_OFFSET);
    m_move->setPos(pos, true);
    m_move->calcForwardFromKartObjPosDelta(true);
}

/// @addr{0x807234A4}
/// @brief Runs every frame when the Lakitu is idle and not visible, checking for switch toggles
/// @details If the reverse switch is toggled on, the Lakitu will transition to the reverse
/// state.
void JugemUnit::calcIdle() {
    if (m_switchReverse && m_switchReverse->isOn()) {
        m_nextStateId = 1;
    }
}

/// @addr{0x80724880}
/// @brief Runs every frame when the reverse switch is toggled
/// @details Lakitu spawns `2000.0f` units above the player when in the @ref State::Descending
/// state, where its anchor is interpolated from `500.0f` units above the player to the stay
/// position (`250.0f` units above and `350.0f` units in front) over `40` frames. It then follows
/// the player in the @ref State::Stay state until the reverse switch toggles off or the player is
/// more than `2000.0f` units away, at which point it transitions to the @ref State::Ascending state
/// and rises for `70` frames before returning to idle.
void JugemUnit::calcReverse() {
    constexpr EGG::Vector3f DESCEND_LOCAL_POS = EGG::Vector3f(0.0f, 500.0f, 0.0f);
    constexpr EGG::Vector3f SLOW_RISE_VEL = EGG::Vector3f(0.0f, 30.0f, 0.0f);
    constexpr EGG::Vector3f FAST_RISE_VEL = EGG::Vector3f(0.0f, 80.0f, 0.0f);
    constexpr EGG::Vector3f STAY_LOCAL_POS = EGG::Vector3f(0.0f, 250.0f, 350.0f);

    constexpr f32 INTERP_RATE_START = 0.0f;
    constexpr f32 INTERP_RATE_END = 1.0f;
    constexpr f32 STAY_INTERP_RATE_DURATION = 70.0f;

    switch (m_state) {
    case State::Descending: {
        bool isDoneInterpolating = m_interp->calc(1.0f);
        EGG::Vector3f localPos = DESCEND_LOCAL_POS.interpolate(m_interp->rate(), STAY_LOCAL_POS);
        m_move->setAwayOrDescending(true);
        m_move->setAnchorPos(transformLocalToWorldUpright(localPos));
        m_move->calcForwardFromKartObjMainRot(true);

        if (isDoneInterpolating) {
            m_state = State::Stay;
        }
    } break;
    case State::Stay: {
        m_move->setAwayOrDescending(false);
        m_move->setAnchorPos(transformLocalToWorldUpright(STAY_LOCAL_POS));
        m_move->calcForwardFromKartObjPosDelta(false);

        EGG::Vector3f kartToLakitu = m_kartObj->pos() - m_move->transPos();

        if (!m_switchReverse->isOn() || kartToLakitu.length() > 2000.0f) {
            m_interp->initKeyframes(INTERP_RATE_START, INTERP_RATE_END, STAY_INTERP_RATE_DURATION);
            m_ascendTimer = 0;
            m_state = State::Ascending;
        }
    } break;
    case State::Ascending: {
        bool isDoneInterpolating = m_interp->calc(1.0f);

        m_move->setRiseVel(++m_ascendTimer < 10 ? SLOW_RISE_VEL : FAST_RISE_VEL);
        m_move->setRising(true);

        if (isDoneInterpolating) {
            m_nextStateId = 0;
        }
    } break;
    default:
        break;
    }

    m_move->calc();
}

/// @addr{0x80722ED8}
/// @brief Evaluates the switches for the Lakitu unit, checking if it should be toggled on/off
/// @details For the purposes of syncing time trials in Kinoko, we only need to evaluate the
/// reverse switch.
void JugemUnit::calcSwitches() {
    if (m_switchReverse) {
        m_switchReverse->calc();
    }
}

/// @addr{0x80722F6C}
/// @brief Updates the Lakitu's position based on the provided transformation matrix, while
/// keeping the y-coordinate fixed to the player's position
/// @param mat The transformation matrix used to update the Lakitu's position
void JugemUnit::setPosFromTransform(const EGG::Matrix34f &mat) {
    m_pos = mat.base(3);
    m_pos.y = m_kartObj->pos().y;
}

/// @addr{0x807230D4}
/// @brief Transforms a vector from Lakitu's local space to world space, keeping the y-axis upright
/// @param v The vector in Lakitu's local space to be transformed to world space
/// @return The transformed vector in world space
EGG::Vector3f JugemUnit::transformLocalToWorldUpright(const EGG::Vector3f &v) const {
    const EGG::Vector3f &vel1Dir = m_kartObj->move()->vel1Dir();
    const EGG::Quatf &mainRot = m_kartObj->mainRot();

    EGG::Vector3f kartForward = mainRot.rotateVector(EGG::Vector3f::ez);
    const EGG::Vector3f &pos = m_kartObj->pos();
    EGG::Vector3f up = EGG::Vector3f::ey;
    EGG::Vector3f forward = vel1Dir + kartForward;
    forward.normalise2();
    EGG::Vector3f right = up.cross(forward);
    right.normalise2();

    if (right.squaredLength() <= std::numeric_limits<f32>::epsilon()) {
        right = EGG::Vector3f::ex;
    }

    forward = right.cross(up);
    forward.normalise2();

    EGG::Matrix34f mat;
    mat.setBase(0, right);
    mat.setBase(1, up);
    mat.setBase(2, forward);
    mat.setBase(3, pos);

    return mat.ps_multVector(v);
}

/// @addr{0x807232E4}
/// @brief Checks for collisions between the Lakitu and the floor
/// @note This function is the entire reason we have to implement Lakitu physics for Kinoko - This
/// collision check can induce an update to the @ref ObjectTownBridge's transformation matrix. See
/// @ref JugemDirector for more information.
void JugemUnit::calcCollision() {
    constexpr f32 RADIUS = 150.0f;

    CollisionInfoPartial colInfo;
    KCLTypeMask maskOut;

    // We ignore the result of the collision check. This check is simply here so that any
    // resulting collisions can cause updates to nearby objects.
    std::ignore = CollisionDirector::Instance()->checkSpherePartialPush(RADIUS, m_pos,
            EGG::Vector3f::inf, KCL_TYPE_FLOOR, &colInfo, &maskOut, 0);
}

} // namespace Kinoko::Field
