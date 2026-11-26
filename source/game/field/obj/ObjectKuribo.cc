#include "ObjectKuribo.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/CollisionInfo.hh"
#include "game/field/RailInterpolator.hh"

#include "game/render/AnmMgr.hh"
#include "game/render/DrawMdl.hh"

#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

/// @addr{0x806DB184}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Initializes the Goomba's acceleration based on param setting 2 divided by `100.0f`.
/// Also initializes the animation rate based on param setting 3 divided by `100.0f`. Unlike the
/// base game, also zero-initializes @ref m_smoothedUp and @ref m_floorNrm, since Kinoko's heaps are
/// not zero-initialized.
ObjectKuribo::ObjectKuribo(const System::MapdataGeoObj &params)
    : ObjectCollidable(params),
      StateManager(this, STATE_ENTRIES),
      m_accel(static_cast<f32>(params.setting(1)) / 100.0f),
      m_animRate(static_cast<f32>(params.setting(2)) / 100.0f) {
    m_smoothedUp = EGG::Vector3f::zero;
    m_floorNrm = EGG::Vector3f::zero;
}

/// @addr{0x806DB3A0}
/// @brief Default virtual destructor
ObjectKuribo::~ObjectKuribo() = default;

/// @addr{0x806DB40C}
/// @copybrief ObjectBase::init()
/// @details Updates the Goomba's transformation. Caches the Goomba's forward direction to @ref
/// m_forward. Initializes the rail interpolator to the start of the rail and zeroes its speed. Sets
/// @ref m_currSpeed, @ref m_animTimer, and @ref m_currFrame to zero. Plays the `walk_l` animation
/// and caches its framecount to @ref m_animDuration. Finally, initializes the Goomba to the walking
/// state.
void ObjectKuribo::init() {
    calcTransform();
    m_forward = transform().base(2);

    m_railInterpolator->init(0.0f, 0);
    m_railInterpolator->setSpeed(0.0f);

    m_currSpeed = 0.0f;
    m_animTimer = 0.0f;
    m_currFrame = 0;

    auto *anmMgr = m_drawMdl->anmMgr();
    anmMgr->playAnim(0.0f, m_animRate, 0);
    m_animDuration = anmMgr->activeAnim(Render::AnmType::Chr)->frameCount();
    m_nextStateId = 1;
}

/// @addr{0x806DB5B0}
/// @copybrief ObjectBase::calc()
/// @details Updates the animation timer mod the total animation duration, then evaluates the
/// Goomba's state machine to run state-specific logic depending on whether the Goomba is idle or
/// walking along the rail. Finally, increments @ref m_currFrame.
void ObjectKuribo::calc() {
    calcAnimTimer();
    StateManager::calc();
    ++m_currFrame;
}

/// @addr{0x806dd278}
/// @copybrief ObjectBase::loadAnims()
/// @details Loads the Goomba's walking animations, `walk_l` and `walk_r`.
void ObjectKuribo::loadAnims() {
    std::array<const char *, 2> names = {{
            "walk_l",
            "walk_r",
    }};

    std::array<Render::AnmType, 2> types = {{
            Render::AnmType::Chr,
            Render::AnmType::Chr,
    }};

    linkAnims(names, types);
}

/// @addr{0x806DD2C8}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc and @ref eLoadFlags::Draw so that the object is
/// calculated every frame
ObjectBase::LoadFlags ObjectKuribo::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc, eLoadFlags::Draw);
}

/// @addr{0x806DC220}
/// @brief Called when the Goomba is idle
/// @details The Goomba is idle for the duration specified by the current rail node setting 1.
/// If the Goomba has been idling for this duration, transitions the Goomba to the walking
/// state. While idle, performs a collision check with the floor and updates the Goomba's
/// position, rotation, and transform accordingly.
void ObjectKuribo::calcIdle() {
    if (m_railInterpolator->curPoint().setting[0] < m_currentFrame) {
        m_nextStateId = 1;
    }

    checkSphereFull();
    calcRot();
    calcMatFromRotAndForward();
}

/// @addr{0x806DC3F8}
/// @brief Called when the Goomba is walking along the rail
/// @details Dispatches to @ref calcAnim() to handle the Goomba's walking animation and movement
/// along the rail.
void ObjectKuribo::calcWalk() {
    calcAnim();
}

/// @addr{0x806DCDDC}
/// @brief Handles the Goomba's walking animation and movement along the rail
/// @details This function defines during which intervals of the animation the Goomba should
/// actually move, so that the Goomba's movement matches up with its animation. Goombas move while
/// @ref m_animTimer is in `(15.0f, 25.0f)` going forward and `(45.0f, 55.0f)` going backward. If
/// it's determined that the Goomba should move, its @ref m_currSpeed is computed based on @ref
/// m_accel and clamped to a maximum value of `10.0f`. If the Goomba should not move, it decelerates
/// to a speed of `0.0f`. Sets the rail interpolator's speed and sets the Goomba's position
/// accordingly. If the Goomba has reached the end of the rail, transitions the Goomba to the idle
/// state and zeroes its speed. Finally, performs a collision check with the floor, updates the
/// Goomba's rotation, and updates its transform accordingly.
void ObjectKuribo::calcAnim() {
    bool shouldMove;

    if (m_railInterpolator->isMovementDirectionForward()) {
        shouldMove = m_animTimer > 15.0f && m_animTimer < 25.0f;
    } else {
        shouldMove = m_animTimer > 45.0f && m_animTimer < 55.0f;
    }

    if (shouldMove) {
        m_currSpeed = std::min(10.0f, m_currSpeed + m_accel);
    } else {
        m_currSpeed = std::max(0.0f, m_currSpeed - m_accel);
    }

    m_railInterpolator->setSpeed(m_currSpeed);
    const auto &curPos = m_railInterpolator->curPos();
    setPos(EGG::Vector3f(curPos.x, pos().y, curPos.z));

    if (m_railInterpolator->calc() == RailInterpolator::Status::RailEnd) {
        m_nextStateId = 0;
        m_currSpeed = 0.0f;
    }

    checkSphereFull();
    calcRot();
    calcMatFromRotAndForward();
}

/// @addr{0x806DCC9C}
/// @brief Smoothly interpolates the Goomba's up vector to match the floor normal beneath it
/// @details Linearly interpolates @ref m_smoothedUp towards @ref m_floorNrm. It then normalizes
/// @ref m_smoothedUp, falling back to @ref EGG::Vector3f::ey if the length of the @ref m_smoothedUp
/// vector is too small.
void ObjectKuribo::calcRot() {
    constexpr f32 INTERP_RATE = 0.1f;

    m_smoothedUp = m_smoothedUp.interpolate(INTERP_RATE, m_floorNrm);

    if (m_smoothedUp.squaredLength() > std::numeric_limits<f32>::epsilon()) {
        m_smoothedUp.normalise2();
    } else {
        m_smoothedUp = EGG::Vector3f::ey;
    }
}

/// @addr{0x806DCB58}
/// @brief Checks for floor collision beneath the Goomba
/// @details If the Goomba is walking, applies a downward gravitational force of `2.0f` every frame.
/// If the Goomba has collided with the floor, updates the Goomba's position accordingly and caches
/// the colliding floor's normal to @ref m_floorNrm.
void ObjectKuribo::checkSphereFull() {
    constexpr f32 RADIUS = 50.0f;
    constexpr f32 GRAVITY = 2.0f;

    // Apply gravity if we're not idle
    if (m_currentStateId != 0) {
        subPos(EGG::Vector3f(0.0f, GRAVITY, 0.0f));
    }

    CollisionInfo colInfo;
    EGG::Vector3f colPos = pos();
    colPos.y += RADIUS;

    bool hasCol = CollisionDirector::Instance()->checkSphereFull(RADIUS, colPos, EGG::Vector3f::inf,
            KCL_TYPE_FLOOR, &colInfo, nullptr, 0);

    if (hasCol) {
        addPos(colInfo.tangentOff);

        if (colInfo.floorDist > -std::numeric_limits<f32>::min()) {
            m_floorNrm = colInfo.floorNrm;
        }
    }
}

/// @addr{0x806DCDD0}
/// @brief Sets the transformation matrix based on rotation and forward direction
void ObjectKuribo::calcMatFromRotAndForward() {
    setMatrixTangentTo(m_smoothedUp, m_forward);
}

/// @addr{0x806DD038}
/// @brief Updates the animation timer based on the current frame and animation rate
/// @details Calculated as \f[ m\_animTimer = (m\_currFrame \times m\_animRate) \bmod
/// m\_animDuration
/// \f]
void ObjectKuribo::calcAnimTimer() {
    m_animTimer = ::fmodf(static_cast<f32>(m_currFrame) * m_animRate, m_animDuration);
}

} // namespace Kinoko::Field
