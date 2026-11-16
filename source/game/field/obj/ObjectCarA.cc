#include "ObjectCarA.hh"

#include "game/field/ObjectCollisionCylinder.hh"
#include "game/field/RailInterpolator.hh"

#include "game/kart/KartObject.hh"
#include "game/kart/Reaction.hh"

#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

/// @addr{0x806B7710}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Computes @ref m_finalSpeed based on param setting 1, @ref m_accel based on param
/// setting 2 divided by `10.0f`, and @ref m_stopDuration based on param setting 3.
ObjectCarA::ObjectCarA(const System::MapdataGeoObj &params)
    : ObjectCollidable(params),
      StateManager(this, STATE_ENTRIES),
      m_finalSpeed(static_cast<f32>(params.setting(0))),
      m_accel(static_cast<f32>(params.setting(1)) / 10.0f),
      m_stopDuration(static_cast<u32>(params.setting(2))) {}

/// @addr{0x806B78CC}
/// @brief Default virtual destructor
ObjectCarA::~ObjectCarA() = default;

/// @addr{0x806B7CE0}
/// @copybrief ObjectBase::init()
/// @details Initializes the rail interpolator to the beginning of the rail and sets the car's
/// position to the current position on the rail. Calculates @ref m_cruiseTime based on the rail
/// length, @ref m_finalSpeed, and @ref m_accel. Initializes the car's @ref m_currVel to zero, @ref
/// m_motionState to @ref MotionState::Accelerating, @ref m_currUp to world up, @ref m_currTangent
/// to the rail interpolator's tangent direction, and the car's collision radius to `400.0f`.
/// Finally, transitions the car to the stop state.
void ObjectCarA::init() {
    constexpr f32 RADIUS = 400.0f;

    m_railInterpolator->init(0, 0);

    // Time to reach final velocity
    f32 finalTime = m_finalSpeed / m_accel;
    m_cruiseTime =
            (m_railInterpolator->railLength() - finalTime * m_accel * finalTime) / m_finalSpeed;

    setMatrixFromOrthonormalBasisAndPos(m_railInterpolator->curTangentDir());

    setPos(m_railInterpolator->curPos());
    m_currVel = 0.0f;
    m_motionState = MotionState::Accelerating;
    m_changingDir = false;
    m_currUp = EGG::Vector3f::ey;
    m_currTangent = m_railInterpolator->curTangentDir();
    resize(RADIUS, 0.0f);
    m_nextStateId = 0;
}

/// @addr{0x806B82CC}
/// @copybrief ObjectBase::calc()
/// @details Evaluates the car's state machine, updates its rail, and updates its position along
/// the rail.
void ObjectCarA::calc() {
    StateManager::calc();
    calcRail();
    calcPos();
}

/// @addr{0x806B8F44}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectCarA::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x806B7B44}
/// @copybrief ObjectBase::createCollision()
/// @details Creates a cylindrical collision object for the car with radius `210.0f` and height
/// `200.0f`.
void ObjectCarA::createCollision() {
    constexpr f32 RADIUS = 210.0f;
    constexpr f32 HEIGHT = 200.0f;

    m_collision = EGG::egg_new<ObjectCollisionCylinder>(RADIUS, HEIGHT, collisionCenter());
}

/// @addr{0x806B7BC4}
/// @copybrief ObjectBase::calcCollisionTransform()
/// @details Modifies the collision object's transform based on the car's current position, scale,
/// and orientation with an upwards vertical offset of `50.0f` units, possibly to account for the
/// tire height.
void ObjectCarA::calcCollisionTransform() {
    constexpr f32 HEIGHT_OFFSET = 50.0f;

    ObjectCollisionBase *objCol = collision();
    if (!objCol) {
        return;
    }

    calcTransform();

    EGG::Matrix34f mat;
    SetRotTangentHorizontal(mat, transform().base(2), EGG::Vector3f::ey);
    mat.setBase(3, transform().base(3) + HEIGHT_OFFSET * transform().base(1));

    objCol->transform(mat, scale(),
            -m_railInterpolator->curTangentDir() * m_railInterpolator->getCurrVel());
}

/// @addr{0x806B7E60}
/// @copybrief ObjectCollidable::onCollision()
/// @param kartObj The kart object that collided with this car
/// @param reactionOnKart The reaction that should be applied to the kart
/// @param reactionOnObj The reaction that should be applied to the car itself (unused)
/// @param hitDepth The depth of the collision (unused)
/// @return @ref Kart::Reaction::WallAllSpeed if the kart's speed is below 50%, otherwise returns
/// @ref Kart::Reaction::KnockbackSomeSpeedLoseItem.
Kart::Reaction ObjectCarA::onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
        Kart::Reaction /*reactionOnObj*/, EGG::Vector3f & /*hitDepth*/) {
    return kartObj->speedRatioCapped() < 0.5f ? Kart::Reaction::WallAllSpeed : reactionOnKart;
}

/// @addr{0x806B8CCC}
/// @brief Updates the rail and checks if the car is changing direction
/// @details Sets the rail interpolator's speed to the car's @ref m_currVel. Updates the rail
/// interpolator. Finally, sets @ref m_changingDir based on whether the car has reached the end of
/// its rail.
void ObjectCarA::calcRail() {
    m_railInterpolator->setCurrVel(m_currVel);

    auto status = m_railInterpolator->calc();
    m_changingDir = (status == RailInterpolator::Status::ChangingDirection);
}

/// @addr{0x806B8D3C}
/// @brief Helper function that updates the car's position based on the rail interpolator
/// @details Interpolates @ref m_currUp towards world up. Updates the car's transformation
/// matrix accordingly, and updates the car's position to the rail interpolator's current
/// position.
void ObjectCarA::calcPos() {
    constexpr f32 INTERP_RATE = 0.1f;

    m_currUp = m_currUp.interpolate(INTERP_RATE, EGG::Vector3f::ey);
    m_currUp.normalise2();
    setMatrixTangentTo(m_currUp, m_currTangent);
    setPos(m_railInterpolator->curPos());
}

/// @addr{0x806B84FC}
/// @brief Runs once when the car has entered the stop state
/// @details Sets the car's @ref m_currVel to zero.
void ObjectCarA::enterStop() {
    m_currVel = 0.0f;
}

/// @addr{0x806B8838}
/// @brief Runs once when the car has entered the cruising state
/// @details Sets the car's @ref m_currVel to @ref m_finalSpeed.
void ObjectCarA::enterCruising() {
    m_currVel = m_finalSpeed;
}

/// @addr{0x806B8588}
/// @brief Runs once per frame when the car is in the stop state
/// @details Checks if @ref m_stopDuration frames have elapsed and transitions to the
/// accelerating state if so, setting @ref m_motionState to @ref MotionState::Accelerating in
/// the process.
void ObjectCarA::calcStop() {
    if (m_currentFrame > m_stopDuration) {
        m_motionState = MotionState::Accelerating;
        m_currentStateId = 1;
    }
}

/// @addr{0x806B86F0}
/// @brief Runs once per frame when the car is in the accelerating or decelerating state
/// @details If the car is accelerating, @ref m_currVel increases by @ref m_accel until
/// it reaches @ref m_finalSpeed at which point it transitions to the cruising state. If the car is
/// decelerating, @ref m_currVel decreases by @ref m_accel until it reaches `0.01f`, preventing
/// the car from stopping before reaching the end of the rail. If the car reaches the end of the
/// rail, @ref m_currVel is set to zero, the car transitions to the stop state, and if the car was
/// moving forward, the rail interpolator is reset to the start of the rail.
void ObjectCarA::calcAccel() {
    if (m_motionState == MotionState::Accelerating) {
        m_currVel += m_accel;

        if (m_currVel > m_finalSpeed) {
            m_currVel = m_finalSpeed;
            m_motionState = MotionState::Cruising;
            m_nextStateId = 2;
        }
    } else if (m_motionState == MotionState::Decelerating) {
        // The cruising time might've had decimals and needed to end early.
        // If we undershot it, keep a velocity of 0.01f so we can reach the end.
        m_currVel = std::max(0.01f, m_currVel - m_accel);

        if (m_changingDir) {
            m_currVel = 0.0f;

            if (m_railInterpolator->isMovementDirectionForward()) {
                m_railInterpolator->init(0.0f, 0);
            }

            m_motionState = MotionState::Cruising;
            m_nextStateId = 0;
        }
    }
}

/// @addr{0x806B8844}
/// @brief Runs once per frame when the car is in the cruising state
/// @details Checks if @ref m_cruiseTime frames have elapsed and transitions to the decelerating
/// state if so, setting @ref m_motionState to @ref MotionState::Decelerating in the process.
void ObjectCarA::calcCruising() {
    // We might've had decimals, better to undershoot the cruising time and handle it in decel
    if (static_cast<f32>(m_currentFrame) > m_cruiseTime - 1.0f) {
        m_motionState = MotionState::Decelerating;
        m_nextStateId = 1;
    }
}

} // namespace Kinoko::Field
