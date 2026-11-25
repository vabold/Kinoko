#include "ObjectVolcanoBall.hh"

#include "game/field/RailInterpolator.hh"

#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

/// @addr{0x806E2904}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param accel The acceleration of the ball
/// @param finalVelSq The squared final velocity of the ball at impact
/// @param endPosY The end position along the Y-axis
/// @param params The parameters used to initialize the object
/// @param vel The initial velocity of the ball
/// @details Initializes @ref m_burnDuration based on param setting 4. Sets @ref m_accel, @ref
/// m_finalVelSq, and @ref m_endPosY based on the `accel`, `finalVelSq`, and `endPosY` arguments
/// respectively. Finally, computes the squared XZ-plane velocity and saves it to @ref
/// m_sqVelXZ.
ObjectVolcanoBall::ObjectVolcanoBall(f32 accel, f32 finalVelSq, f32 endPosY,
        const System::MapdataGeoObj &params, const EGG::Vector3f &vel)
    : ObjectCollidable(params),
      StateManager(this, STATE_ENTRIES),
      m_burnDuration(params.setting(3)),
      m_accel(accel),
      m_finalVelSq(finalVelSq),
      m_endPosY(endPosY),
      m_sqVelXZ(vel.x * vel.x + vel.z * vel.z) {}

/// @addr{0x806E2BE0}
/// @brief Default virtual destructor
ObjectVolcanoBall::~ObjectVolcanoBall() = default;

/// @addr{0x806E2C4C}
/// @copybrief ObjectBase::init()
/// @details Initializes the rail interpolator to the start of the rail and sets the fireball's
/// position accordingly.
void ObjectVolcanoBall::init() {
    m_railInterpolator->init(0.0f, 0);
    setPos(m_railInterpolator->curPos());
}

/// @addr{0x806E2E08}
/// @copybrief ObjectBase::calc()
/// @details Simply evaluates the fireball's state machine.
void ObjectVolcanoBall::calc() {
    StateManager::calc();
}

/// @addr{0x806E3A7C}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame
ObjectBase::LoadFlags ObjectVolcanoBall::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x806E2F24}
/// @brief Runs when the fireball is despawned/intangible
/// @details Simply calls @ref init() to reinitialize the fireball to its initial state.
void ObjectVolcanoBall::enterDormant() {
    init();
}

/// @addr{0x806E2F38}
/// @brief Runs when the fireball starts falling
/// @details Simply calls @ref init() to reinitialize the fireball to its initial state.
/// @todo Since this is identical to the behavior of @ref enterDormant() and there is no
/// `calcDormant()` function, we can likely omit this function for Kinoko.
void ObjectVolcanoBall::enterFalling() {
    init();
}

/// @addr{0x806E3034}
/// @brief Calculates the ball's velocity and updates its position along the rail
/// @details Derives the ball's velocity based on the kinematic equation:
/// \f[v^2 = v_f^2 - 2a(y - y_{end})\f] where \f$v_f^2\f$ is @ref m_finalVelSq, \f$a\f$ is @ref
/// m_accel, \f$y\f$ is the Y-component of @ref pos(), and \f$y_{end}\f$ is @ref m_endPosY.
/// If the fireball has reached the end of its rail, then it transitions to the burning state.
/// Otherwise, the fireball's transformation matrix and position are updated based on the rail
/// interpolator's current tangent direction and position.
void ObjectVolcanoBall::calcFalling() {
    f32 sqVel = std::max(0.01f, m_finalVelSq - 2.0f * m_accel * (pos().y - m_endPosY));
    m_railInterpolator->setCurrVel(EGG::Mathf::sqrt(m_sqVelXZ + sqVel));

    if (m_railInterpolator->calc() == RailInterpolator::Status::ChangingDirection) {
        m_nextStateId = 2;
    } else {
        EGG::Vector3f tangent = m_railInterpolator->curTangentDir();
        if (EGG::Mathf::abs(tangent.y) > 0.1f) {
            tangent.y = 0.01f;
        }

        tangent.normalise2();
        setMatrixFromOrthonormalBasisAndPos(tangent);

        setPos(m_railInterpolator->curPos());
    }
}

/// @addr{0x806E3324}
/// @brief Runs while the fireball is on the ground and burning
/// @details Transitions the fireball to the dormant state once @ref m_burnDuration frames have
/// elapsed.
void ObjectVolcanoBall::calcBurning() {
    if (m_currentFrame >= m_burnDuration) {
        m_nextStateId = 0;
    }
}

} // namespace Kinoko::Field
