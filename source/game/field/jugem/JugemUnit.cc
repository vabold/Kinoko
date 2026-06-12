#include "JugemUnit.hh"

#include "game/field/CollisionDirector.hh"

#include "game/kart/KartMove.hh"
#include "game/kart/KartObjectManager.hh"

#include "game/system/RaceManager.hh"

namespace Kinoko::Field {

/// @addr{0x80721514}
JugemUnit::JugemUnit(const Kart::KartObject *kartObj)
    : StateManager(this, STATE_ENTRIES), m_kartObj(kartObj), m_switchReverse(nullptr) {
    m_move = new JugemMove(kartObj);
    m_interp = new JugemInterp(2);
}

/// @addr{0x80721D2C}
JugemUnit::~JugemUnit() {
    delete m_switchReverse;
    delete m_move;
    delete m_interp;
}

/// @addr{0x807221C4}
void JugemUnit::calc() {
    calcSwitches();

    StateManager::calc();

    // Perform a collision check if Lakitu is not idle
    if (m_currentStateId != 0) {
        setPosFromTransform(m_move->transform());
        calcCollision();
    }
}

/// @addr{0x80724794}
void JugemUnit::enterReverse() {
    constexpr EGG::Vector3f INIT_POS_OFFSET = EGG::Vector3f(0.0f, 2000.0f, 0.0f);

    m_state = State::Descending;
    m_interp->initKeyframes(0.0f, 1.0f, 40.0f, 3);
    m_move->init();
    EGG::Vector3f pos = transformLocalToWorldUpright(INIT_POS_OFFSET);
    m_move->setPos(pos, true);
    m_move->setForwardFromKartObjPosDelta(true);
    m_move->setDescending(false);
}

/// @addr{0x80724880}
void JugemUnit::calcReverse() {
    constexpr EGG::Vector3f AWAY_LOCAL_POS = EGG::Vector3f(0.0f, 2000.0f, 0.0f);
    constexpr EGG::Vector3f DESCEND_LOCAL_POS = EGG::Vector3f(0.0f, 500.0f, 0.0f);
    constexpr EGG::Vector3f SLOW_RISE_VEL = EGG::Vector3f(0.0f, 30.0f, 0.0f);
    constexpr EGG::Vector3f FAST_RISE_VEL = EGG::Vector3f(0.0f, 80.0f, 0.0f);
    constexpr EGG::Vector3f STAY_LOCAL_POS = EGG::Vector3f(0.0f, 250.0f, 350.0f);

    constexpr f32 INTERP_RATE_START = 0.0f;
    constexpr f32 INTERP_RATE_END = 1.0f;
    constexpr f32 AWAY_INTERP_RATE_DURATION = 40.0f;
    constexpr f32 STAY_INTERP_RATE_DURATION = 70.0f;

    EGG::Vector3f local_40 = m_kartObj->pos() - m_move->transPos();

    switch (m_state) {
    case State::Away: {
        bool isDoneInterpolating = m_interp->calc(1.0f);
        EGG::Vector3f localPos = Interpolate(m_interp->rate(), AWAY_LOCAL_POS, DESCEND_LOCAL_POS);
        m_move->setAwayOrDescending(true);
        m_move->setAnchorPos(transformLocalToWorldUpright(localPos));
        m_move->setForwardFromKartObjMainRot(true);

        if (isDoneInterpolating) {
            m_move->setDescending(true);
            m_interp->initKeyframes(INTERP_RATE_START, INTERP_RATE_END, AWAY_INTERP_RATE_DURATION,
                    3);
            m_state = State::Descending;
        }
    } break;
    case State::Descending: {
        bool isDoneInterpolating = m_interp->calc(1.0f);
        EGG::Vector3f localPos = Interpolate(m_interp->rate(), DESCEND_LOCAL_POS, STAY_LOCAL_POS);
        m_move->setAwayOrDescending(true);
        m_move->setAnchorPos(transformLocalToWorldUpright(localPos));
        m_move->setForwardFromKartObjMainRot(true);

        if (isDoneInterpolating) {
            m_state = State::Stay;
        }
    } break;
    case State::Stay: {
        m_move->setAwayOrDescending(false);
        m_move->setAnchorPos(transformLocalToWorldUpright(STAY_LOCAL_POS));
        m_move->setForwardFromKartObjPosDelta(false);

        if (!m_switchReverse->isOn() || local_40.length() > 2000.0f) {
            m_interp->initKeyframes(INTERP_RATE_START, INTERP_RATE_END, STAY_INTERP_RATE_DURATION,
                    3);
            m_move->setDescending(false);
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
    }

    m_move->calc();
}

/// @addr{0x807230D4}
EGG::Vector3f JugemUnit::transformLocalToWorldUpright(const EGG::Vector3f &v) const {
    const EGG::Vector3f &vel1Dir = m_kartObj->move()->vel1Dir();
    const EGG::Quatf &mainRot = m_kartObj->mainRot();

    EGG::Vector3f vStack88 = mainRot.rotateVector(EGG::Vector3f::ez);
    const EGG::Vector3f &pos = m_kartObj->pos();
    EGG::Vector3f up = EGG::Vector3f::ey;
    EGG::Vector3f local_a0 = vel1Dir + vStack88;
    local_a0.normalise2();
    EGG::Vector3f right = up.cross(local_a0);
    right.normalise2();

    if (right.squaredLength() <= std::numeric_limits<f32>::epsilon()) {
        right = EGG::Vector3f::ex;
    }

    EGG::Vector3f forward = right.cross(up);
    forward.normalise2();

    EGG::Matrix34f mat;
    mat.setBase(0, right);
    mat.setBase(1, up);
    mat.setBase(2, forward);
    mat.setBase(3, pos);

    return mat.ps_multVector(v);
}

/// @addr{0x807232E4}
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
