#include "ObjectFireSnake.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/ObjectDirector.hh"

#include "game/system/RaceManager.hh"

namespace Kinoko::Field {

/// @addr{0x806C0D18}
/// @brief Constructor
/// @param params The parameters used to initialize the object
ObjectFireSnakeKid::ObjectFireSnakeKid(const System::MapdataGeoObj &params)
    : ObjectCollidable(params) {}

/// @addr{0x806C2ACC}
/// @brief Default virtual destructor
ObjectFireSnakeKid::~ObjectFireSnakeKid() = default;

/// @addr{0x806C2B60}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectFireSnakeKid::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x806C0F30}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Caches the firesnake's initial position to @ref m_initPos, and sets the firesnake's
/// lifetime based on param setting 2. Registers the fire snake as a managed object, assuming that a
/// @ref ObjectProjectileLauncher has already been registered as a managed object. This effectively
/// enables DS Desert Hills fire snakes to be registered as managed objects due to the existence of
/// @ref ObjectSunDS, whereas Grumble Volcano fire snakes will not be registered as managed objects.
/// Also constructs two @ref ObjectFireSnakeKid follower objects. Their scale is set relative to the
/// fire snake's scale.
ObjectFireSnake::ObjectFireSnake(const System::MapdataGeoObj &params)
    : StateManager(this, STATE_ENTRIES),
      ObjectProjectile(params),
      m_initPos(params.pos()),
      m_maxAge(static_cast<s16>(params.setting(1))) {
    if (ObjectDirector::Instance()->managedObjects().size() > 0) {
        registerManagedObject();
    }

    for (size_t i = 0; i < m_kids.size(); ++i) {
        auto *&kid = m_kids[i];
        kid = EGG::egg_new<ObjectFireSnakeKid>(params);
        kid->load();
        f32 kidScale = (0.75f - 0.25f * static_cast<f32>(i)) * scale().y;
        kid->setScale(kidScale);
    }

    m_prevTransforms.fill(EGG::Matrix34f::ident);
}

/// @addr{0x806C1284}
/// @brief Default virtual destructor
ObjectFireSnake::~ObjectFireSnake() = default;

/// @addr{0x806C13B0}
/// @copybrief ObjectBase::init()
/// @details Initializes the fire snake to the despawned state. Also caches the initial rotation
/// and bounce direction of the fire snake.
void ObjectFireSnake::init() {
    m_nextStateId = 0;
    enterDespawned();

    m_spawnPos.setZero();

    calcTransform();
    m_initRot = transform().base(0);
    m_bounceDir = transform().base(0);
}

/// @addr{0x806C14E4}
/// @copybrief ObjectBase::calc()
/// @details Evaluates the fire snake's state machine. If the fire snake has exceeded its
/// maximum age while in the active state, it transitions to the despawned state. Also updates
/// the state of all children.
void ObjectFireSnake::calc() {
    StateManager::calc();

    if (isCollisionEnabled()) {
        if (++m_age >= m_maxAge && m_currentStateId == 3) {
            m_nextStateId = 0;
        }
    }

    calcChildren();
}

/// @addr{0x806C2A5C}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectFireSnake::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x806C23C8}
/// @brief Callback function called by the managing @ref ObjectSunDS.
/// @param pos The initial spawn position for the fire snake
/// @details Sets @ref m_spawnPos based on the provided `pos`. Calculates the XZ distance between
/// the spawn position and the initial landing position, and sets up the fall direction, speed, and
/// duration accordingly.
void ObjectFireSnake::initProjectile(const EGG::Vector3f &pos) {
    m_spawnPos = pos;

    m_xzSunDist = m_initPos - m_spawnPos;
    m_xzSunDist.y = 0.0f;
    m_xzSunDist.normalise2();

    EGG::Vector3f xzDist = m_spawnPos - m_initPos;
    xzDist.y = 0.0f;
    f32 dist = xzDist.length();

    m_fallDir = RotateXZByYaw(HALF_PI, m_xzSunDist);
    m_xzFallSpeed = dist * EGG::Mathf::sqrt(GRAVITY * 0.5f / (m_spawnPos.y - m_initPos.y));
    m_fallDuration = static_cast<u16>(dist / m_xzFallSpeed);
}

/// @addr{0x806C29FC}
/// @brief Callback function use by the @ref ObjectSunDS that wants to throw this projectile.
/// @details Sets the next state to the falling state when the fire snake is launched.
void ObjectFireSnake::onLaunch() {
    m_nextStateId = 1;
}

/// @addr{0x806C1930}
/// @brief Runs once when the fire snake despawns
/// @details Resets the trajectory position to the spawn position and disables collision.
void ObjectFireSnake::enterDespawned() {
    if (unit()) {
        unregisterCollision();
    }

    m_trajectoryPos = m_spawnPos;
    setPos(m_spawnPos);
}

/// @addr{0x806C19E8}
/// @brief Runs once when the fire snake respawns
/// @details Loads the collision bounding box if it hasn't been loaded yet.
void ObjectFireSnake::enterFalling() {
    if (!unit()) {
        loadAABB(0.0f);
    }
}

/// @addr{0x806C1DCC}
/// @brief Runs once after landing from the sun (for the case of @ref ObjectFireSnake), or runs
/// once upon re-spawning (for the case of @ref ObjectFireSnakeV).
/// @details Sets the trajectory position to the initial landing position, snaps the fire
/// snake's position to the landing position, and determines the bounce direction with a 50%
/// chance of flipping it.
void ObjectFireSnake::enterHighBounce() {
    m_trajectoryPos = m_initPos;
    setPos(m_initPos);
    f32 rand = System::RaceManager::Instance()->random().getF32();
    m_bounceDir = rand >= 0.5f ? m_initRot : -m_initRot;
    m_age = 0;
}

/// @addr{0x806C2000}
/// @brief Runs once after landing from a jump
/// @details If the fire snake strays farther than `1000.0f` units away from its initial landing
/// position, adjusts @ref m_bounceDir to point back towards the initial landing position.
/// Otherwise, applies a random fluctuation to the bounce direction in the range `[-45.0f, 45.0f]`
/// degrees, with a 50% random chance of flipping the resulting direction.
void ObjectFireSnake::enterRest() {
    constexpr f32 XZ_DIST_SQ = 1000.0f * 1000.0f;

    // If it strays too far from original position, start bouncing the opposite direction
    if (m_trajectoryPos.sqDistance(m_initPos) >= XZ_DIST_SQ) {
        m_bounceDir = m_initPos - m_trajectoryPos;
        m_bounceDir.normalise2();
    } else {
        // 90 degree random fluctuation, with a 50% chance to flip direction
        auto &rand = System::RaceManager::Instance()->random();
        f32 angle = rand.getF32(90.0f) - 45.0f;

        if (rand.getF32() >= 0.5f) {
            angle += 180.0f;
        }

        m_bounceDir = RotateXZByYaw(angle * DEG2RAD, m_initRot);
    }
}

/// @addr{0x806C1A88}
/// @brief Runs every frame while falling from the sun
/// @details Applies gravity to the fire snake's position. If the vertical position is at or below
/// @ref m_initPos, the fire snake transitions to the high bounce state. Otherwise, updates the fire
/// snake's position and applies a spiraling motion based on the fall duration.
void ObjectFireSnake::calcFalling() {
    EGG::Vector2f fallVel;
    fallVel.y = -GRAVITY * static_cast<f32>(m_currentFrame);
    f32 fallVelY = fallVel.y;
    f32 posY = m_trajectoryPos.y + fallVel.y;

    if (posY <= m_initPos.y) {
        m_nextStateId = 2;
        setPos(m_trajectoryPos * 0.5f + m_initPos * 0.5f);
    } else {
        fallVel.x = m_xzFallSpeed;
        m_trajectoryPos.y = posY;
        m_trajectoryPos.x += m_xzSunDist.x * fallVel.x;
        m_trajectoryPos.z += m_xzSunDist.z * fallVel.x;
        fallVel.normalise2();

        f32 angle = fallVel.x;
        EGG::Vector3f up = RotateAxisAngle(EGG::Mathf::acos(angle), m_fallDir, EGG::Vector3f::ey);
        auto axis = EGG::Vector3f(m_xzSunDist.x * m_xzFallSpeed, -fallVelY,
                m_xzSunDist.z * m_xzFallSpeed);
        axis.normalise2();
        auto spiral = RotateAxisAngle(50.0f * static_cast<f32>(m_currentFrame) * DEG2RAD, axis, up);

        f32 scaledDuration = 0.9f * static_cast<f32>(m_fallDuration);
        f32 phase = m_currentFrame - static_cast<u16>(scaledDuration);
        f32 spiralRadius = phase < 0 ? 200.0f : 200.0f - (200.0f / scaledDuration) * phase;

        setPos(m_trajectoryPos + spiral * spiralRadius);
    }
}

/// @addr{0x806C1E90}
/// @brief Runs every frame during the first bounce
/// @details Dispatches to calcBounce(float) with a 60% increase in the bounce velocity.
void ObjectFireSnake::calcHighBounce() {
    constexpr f32 HIGH_BOUNCE_SCALAR = 1.6f;

    calcBounce(HIGH_BOUNCE_SCALAR * BOUNCE_VELOCITY);
}

/// @addr{0x806C2138}
/// @brief Runs every frame while the fire snake is in between jumps
/// @details If the fire snake has been resting for at least `30` frames, it transitions to the
/// bouncing state. Regardless, updates the fire snake's transform and interpolates its transform's
/// tangent towards the bounce direction.
void ObjectFireSnake::calcRest() {
    constexpr u32 REST_DURATION = 30;
    constexpr f32 TANGENT_INTERP_FACTOR = 0.3f;

    if (m_currentFrame >= REST_DURATION) {
        m_nextStateId = 4;
    }

    calcTransform();

    EGG::Vector3f tangent = transform().base(2).interpolate(TANGENT_INTERP_FACTOR, m_bounceDir);
    tangent.normalise2();

    setMatrixTangentTo(EGG::Vector3f::ey, tangent);
}

/// @addr{0x806C2254}
/// @brief Runs every frame the fire snake is bouncing, except the first bounce
/// @details Dispatches to calcBounc(float) with a velocity of @ref BOUNCE_VELOCITY.
void ObjectFireSnake::calcBounce() {
    calcBounce(BOUNCE_VELOCITY);
}

/// @addr{0x806C2530}
/// @brief Runs every frame. Updates the children's positions and enables/disables their collision
/// @details Children's positions are set based off previous transform matrices of the parent. The
/// first child takes on the matrix from 10 frames ago, and the second child takes on the matrix
/// from 20 frames ago. The first child's collision is loaded once 10 frames have elapsed and the
/// second child's collision is loaded once 20 frames have elapsed. If the firesnake has despawned
/// this frame, then both children's collision is disabled.
void ObjectFireSnake::calcChildren() {
    // Shift all matrices up by one index
    for (size_t i = m_prevTransforms.size() - 1; i > 0; --i) {
        m_prevTransforms[i] = m_prevTransforms[i - 1];
    }

    calcTransform();
    m_prevTransforms[0] = transform();
    m_prevTransforms[0].setBase(3, pos());

    u32 idx = 10;
    for (auto *&kid : m_kids) {
        if (m_currentStateId == 1 && idx == m_currentFrame) {
            if (!kid->unit()) {
                kid->loadAABB(0.0f);
            }
        } else if (m_currentStateId == 0 && m_currentFrame == 0) {
            if (kid->unit()) {
                kid->unregisterCollision();
            }
        }

        if (m_currentStateId == 1 && m_currentFrame < idx) {
            calcTransform();
            kid->setTransform(transform());
        } else {
            kid->setTransform(m_prevTransforms[idx]);
        }

        idx += 10;
    }
}

/// @brief Helper function since high and regular bounce functions only vary by initial velocity.
/// @param initialVel The initial upwards velocity for the bounce.
/// @details Updates the trajectory position based on the initial velocity and the bounce direction.
/// If at least 10 frames have passed, a collision check is performed to determine if the fire snake
/// has hit a floor or wall. If so, the fire snake enters the rest state. Finally, updates the fire
/// snake's position to @ref m_trajectoryPos.
void ObjectFireSnake::calcBounce(f32 initialVel) {
    // Collision checks only occur after 10 frames in the bounce state.
    constexpr u32 BOUNCE_COL_CHECK_DELAY = 10;
    constexpr f32 RADIUS = 130.0f;
    constexpr f32 BOUNCE_SPEED = 20.0f;

    m_trajectoryPos.x += m_bounceDir.x * BOUNCE_SPEED;
    m_trajectoryPos.z += m_bounceDir.z * BOUNCE_SPEED;
    m_trajectoryPos.y += initialVel - GRAVITY * static_cast<f32>(m_currentFrame);

    CollisionInfo info;

    if (m_currentFrame > BOUNCE_COL_CHECK_DELAY) {
        bool hasCol = CollisionDirector::Instance()->checkSphereFull(RADIUS, m_trajectoryPos,
                EGG::Vector3f::inf, KCL_TYPE_64EBDFFF, &info, nullptr, 0);

        if (hasCol) {
            m_trajectoryPos += info.tangentOff;
            m_nextStateId = 3;
        }
    }

    setPos(m_trajectoryPos);
}

/// @brief Collision is enabled if the snake has landed from the spawner
/// @return `true` if collision is enabled (when bouncing or resting), `false` otherwise.
bool ObjectFireSnake::isCollisionEnabled() const {
    return m_currentStateId == 2 || m_currentStateId == 3 || m_currentStateId == 4;
}

} // namespace Kinoko::Field
