#include "ObjectDossunTsuibiHolder.hh"

#include "game/field/RailInterpolator.hh"
#include "game/field/obj/ObjectDossunTsuibi.hh"

#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

/// @addr{0x807614D0}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Sets @ref m_stillTimer based on param setting 3, @ref m_forwardVel based on param
/// setting 1, and @ref m_stillDuration based on param setting 4. Initializes @ref m_facingBackwards
/// to `false`. Finally, constructs and loads the two @ref ObjectDossunTsuibi instances that this
/// holder managers.
ObjectDossunTsuibiHolder::ObjectDossunTsuibiHolder(const System::MapdataGeoObj &params)
    : ObjectCollidable(params),
      m_stillTimer(static_cast<u32>(params.setting(2))),
      m_facingBackwards(false),
      m_forwardVel(static_cast<f32>(m_mapObj->setting(0))),
      m_stillDuration(static_cast<u32>(m_mapObj->setting(3))) {
    for (auto *&dossun : m_dossuns) {
        dossun = EGG::egg_new<ObjectDossunTsuibi>(params, this);
        dossun->load();
    }
}

/// @addr{0x80764BC8}
/// @brief Default virtual destructor
ObjectDossunTsuibiHolder::~ObjectDossunTsuibiHolder() = default;

/// @addr{0x80761744}
/// @copybrief ObjectBase::init()
/// @details Initializes each @ref ObjectDossunTsuibi instance managed by this holder. Initializes
/// @ref m_initPos and @ref m_initYaw based on the holder's current position. Sets @ref m_state to
/// @ref State::Still. Initializes the rail interpolator to the start of the rail and caches the
/// rail interpolator's speed to @ref m_speed. Sets @ref m_forwardTimer to zero, @ref
/// m_movingForward to `false`, and @ref m_movingSideways to `false`. Calls @ref updatePos() to
/// update the position of each @ref ObjectDossunTsuibi instance managed by this holder. Finally,
/// sets @ref m_lastStompZ based on the holder's current Z position and sets @ref m_flipSideways
/// to 1.
void ObjectDossunTsuibiHolder::init() {
    for (auto *&dossun : m_dossuns) {
        dossun->init();
    }

    m_initPos = pos();
    m_initYaw = rot().y;
    m_state = State::Still;
    m_railInterpolator->init(0.0f, 0);
    m_speed = m_railInterpolator->currVel();
    m_forwardTimer = 0;
    m_movingForward = false;
    m_movingSideways = false;

    updatePos(pos());

    m_lastStompZ = pos().z;
    m_flipSideways = 1;
}

/// @addr{0x8076198C}
/// @copybrief ObjectBase::calc()
/// @details Calculates each @ref ObjectDossunTsuibi instance managed by this holder and calls the
/// appropriate calculation function depending on the holder's @ref m_state.
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
    case State::StillRotating:
        calcResetZ();
        calcRot();

        break;
    }
}

/// @addr{0x80764A1C}
/// @copybrief ObjectBase::calcModel()
/// @details Does nothing since the holder itself does not have a visual model.
void ObjectDossunTsuibiHolder::calcModel() {}

/// @addr{0x80764A2C}
/// @copybrief ObjectBase::loadGraphics()
/// @details This is a no-op in the base game.
void ObjectDossunTsuibiHolder::loadGraphics() {}

/// @addr{0x80764A24}
/// @copybrief ObjectBase::createCollision()
/// @details This is a no-op in the base game.
void ObjectDossunTsuibiHolder::createCollision() {}

/// @addr{0x80764A30}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectDossunTsuibiHolder::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @brief Runs once when the Thwomps enter the Still state, where the Thwomps return to their
/// home position and remain still
/// @details Sets the Thwomp's movement state to @ref State::Backward to indicate the Thwomps
/// are moving backwards towards their home. Also updates the rail interpolator's speed to the
/// Thwomp's speed.
void ObjectDossunTsuibiHolder::startStill() {
    m_state = State::Backward;
    m_railInterpolator->setCurrVel(m_speed);
}

/// @addr{0x807623AC}
/// @brief Runs every frame while the Thwomps remain still at their home position
void ObjectDossunTsuibiHolder::calcStill() {
    if (--m_stillTimer == 0) {
        m_state = State::Forward;
        m_movingSideways = false;
    }
}

/// @addr{0x807638F8}
/// @brief Runs once when transitioning to the StartStomp state
void ObjectDossunTsuibiHolder::enterStartStomp() {
    m_state = State::StartStomp;
    m_forwardTimer = 0;
    m_movingForward = false;
    m_lastStompZ = pos().z;
}

/// @addr{0x807624F0}
/// @brief Runs every frame while the Thwomps are moving forward down the hallway
/// @details Increments @ref m_forwardTimer. If the `45` frame delay has elapsed, sets @ref
/// m_movingForward to `true` so that the Thwomps will begin to move forward (away from their home
/// position). If @ref m_movingForward was already `true`, then calls @ref calcForwardRail() to
/// update the Thwomps' position along the rail. If the Thwomps are moving sideways (indicated by
/// @ref m_movingSideways), then calls @ref calcForwardOscillation() to update their sideways
/// movement. Finally, if the current rail node's second setting is set to 1 and the Thwomps are
/// facing forward, then sets @ref m_facingBackwards to `true` and resets @ref m_backwardsCounter to
/// zero so that the Thwomps continue to move forward while facing towards their home before
/// stomping.
void ObjectDossunTsuibiHolder::calcForward() {
    constexpr u32 FORWARD_DELAY_FRAMES = 45;

    ++m_forwardTimer;

    if (m_movingForward) {
        calcForwardRail();
    } else if (m_forwardTimer == FORWARD_DELAY_FRAMES) {
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

/// @brief Runs once when the Thwomps begin to stomp
/// @details For each @ref ObjectDossunTsuibi instance, sets ObjectDossun::m_anmState to @ref
/// ObjectDossun::AnmState::BeforeFall, @ref ObjectDossun::m_beforeFallTimer to 10,  @ref
/// ObjectDossun::m_currYaw based on the current rotation of the Thwomp clamped to the range `[-180,
/// 180]` degrees, and @ref ObjectDossun::m_stompDuration based on @ref
/// ObjectDossun::m_fullDuration. Finally, sets the holder's @ref m_state to @ref State::Stomping.
void ObjectDossunTsuibiHolder::calcStartStomp() {
    for (auto *&dossun : m_dossuns) {
        dossun->m_anmState = ObjectDossun::AnmState::BeforeFall;
        dossun->m_beforeFallTimer = 10;
        f32 rot = dossun->rot().y;
        dossun->m_currYaw = rot;

        if (rot >= F_PI) {
            dossun->m_currYaw = rot - F_TAU;
        }

        dossun->m_stompDuration = dossun->m_fullDuration;
    }

    m_state = State::Stomping;
}

/// @brief Runs every frame while the Thwomps are stomping downwards
/// @details Calls @ref ObjectDossunTsuibi::calcStomp() for each managed Thwomp.
void ObjectDossunTsuibiHolder::calcStomp() {
    for (auto *&dossun : m_dossuns) {
        dossun->calcStomp();
    }
}

/// @addr{0x80762EEC}
/// @brief Runs every frame while the Thwomps are moving backwards up the hallway towards home
/// @details Updates the rail interpolator. Sets the Thwomp's X and Y position based on the rail
/// interpolator's current position, keeping the Thwomp's Z position fixed at @ref m_lastStompZ.
///
/// Once the Thwomps reach the end of the rail (effectively when they stomp), computes @ref
/// m_resetZVel as the time it takes to move from @ref m_lastStompZ to the beginning of the rail in
/// @ref HOME_RESET_FRAMES frames. Sets @ref m_resetAngVel based on whether the Thwomps are facing
/// backwards, resets @ref m_backwardsCounter to 0, sets @ref m_state to @ref State::StillRotating,
/// and sets @ref m_facingBackwards to `false`. Finally, re-initializes the rail interpolator to the
/// start of the rail.
void ObjectDossunTsuibiHolder::calcBackwards() {
    if (m_railInterpolator->calc() == RailInterpolator::Status::ChangingDirection) {
        m_resetZVel = (m_lastStompZ - m_railInterpolator->curPos().z) /
                static_cast<f32>(HOME_RESET_FRAMES);

        m_resetAngVel = m_facingBackwards ? 5.0f : 10.0f;
        m_backwardsCounter = 0;
        m_state = State::StillRotating;
        m_facingBackwards = false;
        m_railInterpolator->init(0.0f, 0);
    }

    updatePos(EGG::Vector3f(pos().x, pos().y, m_lastStompZ));
}

/// @addr{0x807630D0}
/// @brief Nudges the Thwomps' Z-pos back to their home before moving forward
void ObjectDossunTsuibiHolder::calcResetZ() {
    updatePos(EGG::Vector3f(pos().x, pos().y, pos().z - m_resetZVel));
}

/// @addr{0x807625F0}
/// @brief Runs every frame in order to update the Thwomps' rotation
/// @details If the Thwomps are facing backwards, then they rotate for 36 frames at 5 degrees per
/// frame, so that they rotate 180 degrees overall. When they are done rotation, sets @ref
/// m_movingSideways to `true` so that the Thwomps will begin oscillating side-to-side and inverts
/// @ref m_flipSideways so that their oscillation is mirrored from the last oscillation. Otherwise,
/// if the Thwomps finished stomping and are rotating to face their home position, they rotate at
/// the same 5 degrees per frame speed until they face the home direction. Once they face home, @ref
/// m_state is set to @ref State::Still and @ref m_stillTimer is reset to @ref m_stillDuration.
void ObjectDossunTsuibiHolder::calcRot() {
    constexpr f32 YAW_SPEED = DEG2RAD * 5.0f;
    STATIC_ASSERT(YAW_SPEED == 0.08726646f);

    if (m_facingBackwards) {
        if (++m_backwardsCounter == HOME_RESET_FRAMES) {
            updateRot(rot().y + F_PI);

            m_movingSideways = true;
            m_sidewaysPhase = 0;

            if (m_flipSideways == 0) {
                m_flipSideways = 1;
            } else {
                m_flipSideways = 0;
            }
        } else if (m_backwardsCounter < HOME_RESET_FRAMES) {
            updateRot(rot().y + YAW_SPEED);
        }
    } else if (m_state == State::StillRotating) {
        updateRot(rot().y + m_resetAngVel * DEG2RAD);

        if (++m_backwardsCounter == HOME_RESET_FRAMES) {
            updateRot(m_initYaw);

            m_state = State::Still;
            m_stillTimer = m_stillDuration;
        }
    }
}

/// @addr{0x8076321C}
/// @brief Updates position from the rail every frame while the Thwomps are moving forward
void ObjectDossunTsuibiHolder::calcForwardRail() {
    m_railInterpolator->setCurrVel(m_forwardVel);

    if (m_railInterpolator->calc() == RailInterpolator::Status::ChangingDirection) {
        enterStartStomp();
    }

    updatePos(m_railInterpolator->curPos());
}

/// @addr{0x807634C0}
/// @brief Calculates sideways oscillation of the Thwomps while moving forward down the hallway
/// @details Updates the Thwomps' lateral position based on a sinusoidal function to create a
/// side-to-side oscillation effect. Checks if the Thwomps should begin stomping and reverses the
/// rail direction if so.
void ObjectDossunTsuibiHolder::calcForwardOscillation() {
    constexpr f32 AMPLITUDE = 1500.0f;
    constexpr f32 STOMP_PHASE = 170.0f;

    m_sidewaysPhase += 2;

    u32 phase = m_flipSideways ? m_sidewaysPhase + 180 : m_sidewaysPhase;
    f32 posOffsetZ = AMPLITUDE * EGG::Mathf::sin(static_cast<f32>(phase) * DEG2RAD);
    updatePos(EGG::Vector3f(pos().x, pos().y, pos().z + posOffsetZ));

    if (m_sidewaysPhase == STOMP_PHASE) {
        if (m_state != State::StartStomp) {
            m_railInterpolator->reverseDirection();
        }

        enterStartStomp();
    }
}

/// @addr{0x80762054}
/// @brief Synchronizes the position of the Thwomps based off the provided position
/// @param pos The new position to set for the Thwomps
/// @details Sets each @ref ObjectDossunTsuibi instance's position to match the provided `pos`.
void ObjectDossunTsuibiHolder::updatePos(const EGG::Vector3f &pos) {
    setPos(pos);
    m_dossuns[0]->setPos(EGG::Vector3f(pos.x, pos.y, pos.z + DOSSUN_POS_OFFSET));
    m_dossuns[1]->setPos(EGG::Vector3f(pos.x, pos.y, pos.z - DOSSUN_POS_OFFSET));
}

/// @addr{0x80762190}
/// @brief Synchronizes the rotation of the Thwomps based off the provided yaw
/// @param yaw The new yaw angle to set for the Thwomps
/// @details Sets each @ref ObjectDossunTsuibi instance's yaw to match the provided `yaw`.
void ObjectDossunTsuibiHolder::updateRot(f32 yaw) {
    setRot(EGG::Vector3f(rot().x, yaw, rot().z));

    const auto &firstRot = m_dossuns[0]->rot();
    const auto &secondRot = m_dossuns[1]->rot();
    m_dossuns[0]->setRot(EGG::Vector3f(firstRot.x, rot().y, firstRot.z));
    m_dossuns[1]->setRot(EGG::Vector3f(secondRot.x, rot().y, secondRot.z));
}

} // namespace Kinoko::Field
