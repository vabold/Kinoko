#include "ObjectRock.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/CollisionInfo.hh"
#include "game/field/RailInterpolator.hh"

#include "game/kart/Reaction.hh"

#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

/// @addr{0x8076F2E0}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Initializes @ref m_cooldownDuration based on param setting 2, @ref m_railSpeed
/// based on param setting 3, and @ref m_initVelY based on param setting 4.
ObjectRock::ObjectRock(const System::MapdataGeoObj &params)
    : ObjectCollidable(params),
      m_cooldownDuration(params.setting(1)),
      m_railSpeed(static_cast<f32>(params.setting(2))),
      m_initVelY(static_cast<f32>(params.setting(3))) {
    m_transformPending = false;
}

/// @addr{0x8076F344}
/// @brief Default virtual destructor
ObjectRock::~ObjectRock() = default;

/// @addr{0x8076F384}
/// @copybrief ObjectBase::init()
/// @details Initializes the rail interpolator to the start of the rail with a speed of @ref
/// m_railSpeed. Updates the rail interpolator once. Sets the rock's @ref m_state to @ref
/// State::Tangible and initializes its position and @ref m_startYPos to the rail interpolator's
/// current height. Sets the rock's transformation matrix based on the rail's tangent direction.
/// Initializes @ref m_angRad to zero, @ref m_angSpd to @ref INITIAL_ANGULAR_SPEED, and sets @ref
/// m_cooldownTimer based on param setting 1. Finally, sets the Y-component of @ref m_vel to @ref
/// m_initVelY.
void ObjectRock::init() {
    m_railInterpolator->init(0.0f, 0);
    m_railInterpolator->setSpeed(m_railSpeed);
    m_railInterpolator->calc();

    m_state = State::Tangible;
    f32 posY = m_startYPos = m_railInterpolator->curPos().y;
    setPos(EGG::Vector3f(pos().x, posY, pos().z));

    EGG::Vector3f curTanDirNorm = m_railInterpolator->curTangentDir();
    curTanDirNorm.normalise();
    setMatrixTangentTo(EGG::Vector3f::ey, curTanDirNorm);
    calcTransform();
    m_angRad = 0.0f;
    m_angSpd = INITIAL_ANGULAR_SPEED;
    m_cooldownTimer = m_mapObj->setting(0);
    m_vel.x = 0.0f;
    m_vel.y = m_initVelY;
    m_vel.z = 0.0f;
    calcTransform();
}

/// @addr{0x8076F590}
/// @copybrief ObjectBase::calc()
/// @details Calls the appropriate calculation function (either @ref calcTangible() or @ref
/// calcIntangible()) based on the rock's current state. Decrements @ref m_cooldownTimer. Sets the X
/// and Z components of the rock's velocity based on the rail's tangent direction and current speed.
void ObjectRock::calc() {
    switch (m_state) {
    case State::Tangible:
        calcTangible();
        break;
    case State::Intangible:
        calcIntangible();
        break;
    }

    --m_cooldownTimer;
    EGG::Vector3f scaledTang =
            m_railInterpolator->curTangentDir() * m_railInterpolator->getCurrSpeed();
    m_vel.x = scaledTang.x;
    m_vel.z = scaledTang.z;
}

/// @addr{0x8076FFC0}
/// @brief Makes the rock tangible again after being intangible
/// @details Sets @ref m_state to @ref State::Tangible. Resets @ref m_angSpd to @ref
/// INITIAL_ANGULAR_SPEED, sets the Y component of @ref m_vel to @ref m_initVelY, and resets @ref
/// m_cooldownTimer. Finally, re-enables the rock's collision.
void ObjectRock::enterTangible() {
    m_state = State::Tangible;
    m_angSpd = INITIAL_ANGULAR_SPEED;
    m_vel.y = m_initVelY;
    m_cooldownTimer = m_cooldownDuration;
    enableCollision();
}

/// @addr{0x8076F768}
/// @brief Runs every frame that the rock is collidable
/// @details Updates the rock's position along the rail. If the rock has reached the end of the
/// rail, it breaks via @ref breakRock() and becomes intangible. Applies a downward gravitational
/// force of `2.0f` and updates the rock's position accordingly. Checks to see if the rock has
/// collided with the floor. Finally, updates the rock's rotation based on its angular velocity.
void ObjectRock::calcTangible() {
    constexpr f32 GRAVITY = 2.0f;

    auto railStatus = m_railInterpolator->calc();
    if (railStatus == RailInterpolator::Status::RailEnd) {
        breakRock();
    }

    const auto &railPos = m_railInterpolator->curPos();
    setPos(EGG::Vector3f(railPos.x, pos().y, railPos.z));
    m_vel.y -= GRAVITY;

    addPos(m_vel);

    checkSphereFull();
    calcTangibleSub();
}

/// @addr{0x8076F868}
/// @brief Runs every frame that the rock is intangible
/// @details Waits for the cooldown timer to expire before making the rock tangible again.
void ObjectRock::calcIntangible() {
    if (m_cooldownTimer < 0) {
        enterTangible();
    }
}

/// @addr{0x8076F91C}
/// @brief Updates the rock's rotation based on its angular velocity
/// @details Increments the rock's @ref m_angRad based on @ref m_angSpd. OPT: Kinoko defers updating
/// the rock's rotation matrix until @ref calcCollisionTransform(), since the transform is only read
/// when checking collision against the kart.
void ObjectRock::calcTangibleSub() {
    m_angRad += m_angSpd * DEG2RAD;
    m_transformPending = true;
}

/// @copybrief ObjectCollidable::calcCollisionTransform()
/// @details Computes the rock's transform if it was deferred by @ref calcTangibleSub(), then
/// updates the GJK collision transform.
/// @note This override does not exist in the base game. We override it to handle the deferred
/// transform computation for the rock.
void ObjectRock::calcCollisionTransform() {
    if (m_transformPending) {
        m_transformPending = false;

        // m_angRad and the rail tangent are unchanged since the last calcTangibleSub() call, so we
        // don't need to cache them separately.
        EGG::Vector3f tangDir = m_railInterpolator->curTangentDir();
        tangDir.y = 0.0f;
        tangDir.normalise();
        EGG::Matrix34f basis = OrthonormalBasis(tangDir);

        EGG::Matrix34f mat;
        mat.makeR(EGG::Vector3f(m_angRad, 0.0f, 0.0f));
        mat = basis.multiplyTo(mat);
        mat.setBase(3, pos());
        setTransform(mat);
    }

    ObjectCollidable::calcCollisionTransform();
}

/// @addr{0x8076FD90}
/// @brief Runs when a collision occurs or when the rock reaches the end of its rail path
/// @details Sets @ref m_state to @ref State::Intangible, reinitializes the rail interpolator to
/// the start of the rail with a speed of @ref m_railSpeed, resets the rock's vertical position
/// to @ref m_startYPos, and disables the rock's collision.
void ObjectRock::breakRock() {
    m_state = State::Intangible;
    m_railInterpolator->init(0.0f, 0);
    m_railInterpolator->setSpeed(m_railSpeed);
    setPos(EGG::Vector3f(pos().x, m_startYPos, pos().z));
    disableCollision();
}

/// @addr{0x8076FA60}
/// @brief Checks for collisions with the floor and applies a bounce effect if a collision occurs
/// @details Offsets the rock's position downward so that the collision check occurs with the
/// bottommost part of the rock. If a floor collision occurs, bounces the rock upwards with a 70%
/// dampener applied to its velocity, increases the rock's @ref m_angSpd (with a minimum value based
/// on the rail's speed), and updates the rock's position accordingly.
void ObjectRock::checkSphereFull() {
    constexpr f32 RADIUS = 50.0f;

    CollisionInfo info;

    EGG::Vector3f offset(0.0f, -(scale().x * ROCK_RADIUS - RADIUS), 0.0f);
    EGG::Vector3f colPos = pos() + offset;

    if (CollisionDirector::Instance()->checkSphereFull(RADIUS, colPos, EGG::Vector3f::inf,
                KCL_TYPE_FLOOR, &info, nullptr, 0)) {
        m_vel.y *= -0.3f;
        m_angSpd = std::max(360.0f * m_railSpeed / ((2.0f * ROCK_RADIUS) * scale().x * F_PI),
                m_angSpd + 1.0f);
        addPos(info.tangentOff);
    }
}

/// @addr{0x8077037C}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectRock::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x8077036C}
/// @copybrief ObjectCollidable::getCollisionTranslation()
/// @return @ref m_vel, so that it is applied on top of the object's world transform for
/// collision checks
const EGG::Vector3f &ObjectRock::getCollisionTranslation() const {
    return m_vel;
}

/// @addr{0x80770068}
/// @copybrief ObjectCollidable::onCollision()
/// @param reactionOnKart The reaction that should be applied to the kart upon collision
/// @return @ref Kart::Reaction::None if the rock is small, otherwise @ref
/// Kart::Reaction::LaunchSpin.
/// @details Collision with small rocks breaks the rock without affecting the player.
Kart::Reaction ObjectRock::onCollision(Kart::KartObject * /*kartObj*/,
        Kart::Reaction reactionOnKart, Kart::Reaction /*reactionOnObj*/,
        EGG::Vector3f & /*hitDepth*/) {
    if (scale().x < 1.0f) {
        breakRock();
        return Kart::Reaction::None;
    }
    return reactionOnKart;
}

} // namespace Kinoko::Field
