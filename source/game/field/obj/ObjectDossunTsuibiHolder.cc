#include "ObjectDossunTsuibiHolder.hh"

#include "game/field/obj/ObjectDossunTsuibi.hh"

namespace Field {

/// @addr{0x807614D0}
ObjectDossunTsuibiHolder::ObjectDossunTsuibiHolder(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_stillTimer(static_cast<s32>(params.setting(2))),
      m_facingBackwards(false), m_forwardVel(static_cast<f32>(params.setting(0))),
      m_stillDuration(static_cast<s32>(params.setting(3))) {
    for (auto *&dossun : m_dossuns) {
        dossun = new ObjectDossunTsuibi(params, this);
        dossun->load();
    }
}

/// @addr{0x80764BC8}
ObjectDossunTsuibiHolder::~ObjectDossunTsuibiHolder() = default;

/// @addr{0x80761744}
void ObjectDossunTsuibiHolder::init() {
    for (auto *&dossun : m_dossuns) {
        dossun->init();
    }

    m_initPos = m_pos;
    m_initRotY = m_rot.y;
    m_state = State::Still;
    m_railInterpolator->init(0.0f, 0);
    m_vel = m_railInterpolator->currVel();
    m_forwardTimer = 0;
    m_movingForward = false;
    m_movingSideways = false;

    updatePos(m_pos);

    m_lastStompZ = m_pos.z;
    m_flipSideways = 1;
}

/// @addr{0x8076198C}
void ObjectDossunTsuibiHolder::calc() {
    for (auto &dossun : m_dossuns) {
        dossun->calc();
    }

    switch (m_state) {
    case State::Still:
        calcStill();
        break;
    case State::Forward:
        calcForward();
        calcRot();
        break;
    case State::StartStomp:
        calcStartStomp();
        break;
    case State::Stomping:
        calcStomp();
        calcRot();

        break;
    case State::Backward:
        calcBackwards();
        calcRot();

        break;
    case State::SillRotating:
        calcStillRotating();
        calcRot();

        break;
    }
}

/// @addr{0x807624F0}
void ObjectDossunTsuibiHolder::calcForward() {
    ++m_forwardTimer;

    if (m_movingForward) {
        calcForwardRail();
    } else if (m_forwardTimer == 45) {
        m_movingForward = true;
    }

    if (m_movingSideways) {
        calcForwardOscillation();
    }

    if (m_railInterpolator->curPoint().setting[1] == 1 && !m_facingBackwards) {
        m_facingBackwards = true;
        m_backwardsCounter = 0;
    }
}

void ObjectDossunTsuibiHolder::calcStartStomp() {
    for (auto *&dossun : m_dossuns) {
        dossun->m_anmState = ObjectDossun::AnmState::BeforeFall;
        dossun->m_beforeFallTimer = 10;
        f32 rot = dossun->m_rot.y;
        dossun->m_currRot = rot;

        if (rot >= F_PI) {
            dossun->m_currRot = rot - F_TAU;
        }

        dossun->m_cycleTimer = dossun->m_fullDuration;
    }

    m_state = State::Stomping;
}

void ObjectDossunTsuibiHolder::calcStomp() {
    for (auto *&dossun : m_dossuns) {
        dossun->calcStomp();
    }
}

/// @addr{0x80762EEC}
void ObjectDossunTsuibiHolder::calcBackwards() {
    if (m_railInterpolator->calc() == RailInterpolator::Status::ChangingDirection) {
        m_resetZVel = (m_lastStompZ - m_railInterpolator->curPos().z) /
                static_cast<f32>(HOME_RESET_FRAMES);

        m_resetAngVel = m_facingBackwards ? 5.0f : 10.0f;
        m_backwardsCounter = 0;
        m_state = State::SillRotating;
        m_facingBackwards = false;
        m_railInterpolator->init(0.0f, 0);
    }

    updatePos(EGG::Vector3f(m_pos.x, m_pos.y, m_lastStompZ));
}

/// @addr{0x807625F0}
void ObjectDossunTsuibiHolder::calcRot() {
    constexpr f32 DEGREES_5_RAD = DEG2RAD * 5.0f;
    STATIC_ASSERT(DEGREES_5_RAD == 0.08726646f);

    if (m_facingBackwards) {
        if (++m_backwardsCounter == HOME_RESET_FRAMES) {
            updateRot(m_rot.y + F_PI);

            m_movingSideways = true;
            m_sidewaysPhase = 0;

            if (m_flipSideways == 0) {
                m_flipSideways = 1;
            } else {
                m_flipSideways = 0;
            }
        } else if (m_backwardsCounter < HOME_RESET_FRAMES) {
            updateRot(m_rot.y + DEGREES_5_RAD);
        }
    } else if (m_state == State::SillRotating) {
        updateRot(m_rot.y + m_resetAngVel * DEG2RAD);

        if (++m_backwardsCounter == HOME_RESET_FRAMES) {
            updateRot(m_initRotY);

            m_state = State::Still;
            m_stillTimer = m_stillDuration;
        }
    }
}

/// @addr{0x8076321C}
void ObjectDossunTsuibiHolder::calcForwardRail() {
    m_railInterpolator->setCurrVel(m_forwardVel);

    if (m_railInterpolator->calc() == RailInterpolator::Status::ChangingDirection) {
        m_state = State::StartStomp;
        m_forwardTimer = 0;
        m_movingForward = false;
        m_lastStompZ = m_pos.z;
    }

    updatePos(m_railInterpolator->curPos());
}

/// @addr{0x807634C0}
void ObjectDossunTsuibiHolder::calcForwardOscillation() {
    constexpr f32 AMPLITUDE = 1500.0f;
    constexpr f32 STOMP_PHASE = 170.0f;

    m_sidewaysPhase += 2;

    u32 phase = m_flipSideways ? m_sidewaysPhase + 180 : m_sidewaysPhase;
    f32 posOffsetZ = AMPLITUDE * EGG::Mathf::sin(static_cast<f32>(phase) * DEG2RAD);
    updatePos(EGG::Vector3f(m_pos.x, m_pos.y, m_pos.z + posOffsetZ));

    if (m_sidewaysPhase == STOMP_PHASE) {
        if (m_state != State::StartStomp) {
            m_railInterpolator->reverseDirection();
        }

        m_state = State::StartStomp;
        m_forwardTimer = 0;
        m_movingForward = false;
        m_lastStompZ = m_pos.z;
    }
}

/// @addr{0x80762054}
void ObjectDossunTsuibiHolder::updatePos(const EGG::Vector3f &pos) {
    m_flags.setBit(eFlags::Position);
    m_pos = pos;

    m_dossuns[0]->setPos(EGG::Vector3f(pos.x, pos.y, pos.z + DOSSUN_POS_OFFSET));
    m_dossuns[1]->setPos(EGG::Vector3f(pos.x, pos.y, pos.z - DOSSUN_POS_OFFSET));
}

/// @addr{0x80762190}
void ObjectDossunTsuibiHolder::updateRot(f32 rot) {
    m_flags.setBit(eFlags::Rotation);
    m_rot.y = rot;

    m_dossuns[0]->m_flags.setBit(eFlags::Rotation);
    m_dossuns[0]->m_rot.y = m_rot.y;
    m_dossuns[1]->m_flags.setBit(eFlags::Rotation);
    m_dossuns[1]->m_rot.y = m_rot.y;
}

} // namespace Field
