#include "ObjectFireSnakeV.hh"

#include "game/field/CollisionDirector.hh"

#include "game/system/RaceManager.hh"

namespace Kinoko::Field {

/// @addr{0x806C2B70}
/// @copydoc ObjectFireSnake::ObjectFireSnake(const System::MapdataGeoObj &)
/// @details Sets @ref m_delayFrame from param setting 1, @ref m_cycleDuration based on param
/// setting 2, and @ref m_distFromPipe based on param setting 3. Sets @ref m_spawnPos to the
/// object's position. Updates the fire snake's transform and calculates its initial rotation
/// and landing position.
ObjectFireSnakeV::ObjectFireSnakeV(const System::MapdataGeoObj &params)
    : StateManager(this, STATE_ENTRIES),
      ObjectFireSnake(params),
      m_delayFrame(params.setting(0)),
      m_cycleDuration(params.setting(1)),
      m_distFromPipe(static_cast<f32>(params.setting(2))),
      m_fallSpeed(m_distFromPipe / SPAWN_FALL_DURATION) {
    m_spawnPos = pos();

    calcTransform();

    m_initRot = transform().base(0);
    m_initPos = m_spawnPos + m_initRot * m_distFromPipe;
}

/// @addr{0x806C3548}
/// @brief Default virtual destructor
ObjectFireSnakeV::~ObjectFireSnakeV() = default;

/// @addr{0x806C2CBC}
/// @copybrief ObjectBase::init()
/// @details The fire snake starts despawned. Even though @ref m_nextStateId is set to the
/// falling state, the state machine will not actually run in @ref calcSub() until @ref
/// m_delayFrame frames have elapsed. Sets @ref m_trajectoryPos to the fire snake's spawn
/// position and sets @ref m_bounceDir to @ref m_initRot.
void ObjectFireSnakeV::init() {
    m_nextStateId = 1;
    ObjectFireSnake::enterDespawned();

    m_trajectoryPos = m_spawnPos;
    m_bounceDir = m_initRot;
}

/// @addr{0x806C2D54}
/// @copybrief ObjectBase::calc()
/// @details Only starts calculating the fire snake's behavior after @ref m_delayFrame frames
/// have elapsed in the race. Dispatches to @ref calcSub() for the actual calculation.
void ObjectFireSnakeV::calc() {
    if (System::RaceManager::Instance()->timer() >= m_delayFrame) {
        calcSub();
    }
}

/// @addr{0x806C2DA4}
/// @brief Updates state lifecycle and children positions once the spawn delay has elapsed
/// @details Evaluates the fire snake's state machine. If @ref m_cycleDuration frames have elapsed
/// since the initial delay, the fire snake will spawn by transitioning to the falling state. If the
/// fire snake is spawned, increments @ref m_age each frame. If the age of the fire snake exceeds
/// `600` frames and the fire snake is at rest, then despawns the fire snake by transitioning to the
/// despawned state. Finally, dispatches to @ref calcChildren() to update the state of the child
/// objects.
void ObjectFireSnakeV::calcSub() {
    constexpr u16 LIFECYCLE_DURATION = 600;

    StateManager::calc();

    u32 frame = System::RaceManager::Instance()->timer() - m_delayFrame;
    if (frame % m_cycleDuration == 0) {
        m_nextStateId = 1;
    }

    if (m_currentStateId >= 1 && m_currentStateId <= 4) {
        ++m_age;
        if (m_age >= LIFECYCLE_DURATION && m_currentStateId == 3) {
            m_nextStateId = 0;
        }
    }

    calcChildren();
}

/// @addr{0x806C30F0}
/// @brief Runs once when the fire snake despawns
/// @details Calls the base class implementation to handle the fire snake's despawn behavior.
/// @note This is a separate function definition because @ref StateManagerEntry expects a member
/// function of the derived type, thus we cannot pass a @ref ObjectFireSnake pointer directly.
void ObjectFireSnakeV::enterDespawned() {
    ObjectFireSnake::enterDespawned();
}

/// @addr{0x806C30F8}
/// @brief Runs once when the fire snake respawns
/// @details Calls the base class implementation to handle the fire snake's initial falling
/// behavior. Disables the fire snake's collision. Resets the fire snake's age, trajectory position,
/// and bounce direction. In the base game, this function also calculates @ref m_fallSpeed, but its
/// value remains constant and can be determined at the time of construction.
void ObjectFireSnakeV::enterFalling() {
    ObjectFireSnake::enterFalling();

    if (unit()) {
        unregisterCollision();
    }

    m_age = 0;
    m_trajectoryPos = m_spawnPos;
    m_bounceDir = m_initRot;
    m_initPos = m_spawnPos + m_initRot * m_distFromPipe;
}

/// @brief Runs once after landing from a jump
/// @details Calls the base class implementation to handle the fire snake's enter rest behavior.
/// @note This is a separate function definition because @ref StateManagerEntry expects a member
/// function of the derived type, thus we cannot pass a @ref ObjectFireSnake pointer directly.
void ObjectFireSnakeV::enterRest() {
    ObjectFireSnake::enterRest();
}

/// @addr{0x806C31F0}
/// @brief Runs every frame between the fire snake respawning and landing on the ground
/// @details If the fire snake has been falling for more than 5 frames, re-enabled the fire snake's
/// collision. Every frame, updates the fire snake's trajectory position based on its bounce
/// direction and fall speed, and checks for collisions with the floor. If a collision is detected,
/// adjusts the trajectory position and transitions to the high bounce state.
void ObjectFireSnakeV::calcFalling() {
    constexpr f32 INITIAL_Y_VELOCITY = 120.0f;
    constexpr u32 AABB_DELAY_FRAMES = 5;

    if (m_currentFrame > AABB_DELAY_FRAMES && !unit()) {
        loadAABB(0.0f);
    }

    m_trajectoryPos.x += m_bounceDir.x * m_fallSpeed;
    m_trajectoryPos.z += m_bounceDir.z * m_fallSpeed;
    m_trajectoryPos.y += INITIAL_Y_VELOCITY - GRAVITY * static_cast<f32>(m_currentFrame);

    if (m_currentFrame > COL_CHECK_DELAY_FRAMES) {
        CollisionInfo colInfo;

        bool hasCol = CollisionDirector::Instance()->checkSphereFull(RADIUS, m_trajectoryPos,
                EGG::Vector3f::inf, KCL_TYPE_FLOOR, &colInfo, nullptr, 0);

        if (hasCol) {
            m_trajectoryPos += colInfo.tangentOff;
            m_nextStateId = 2;
        }
    }

    setMatrixTangentTo(EGG::Vector3f::ey, m_bounceDir);
    setPos(m_trajectoryPos);
}

/// @addr{0x806C33C4}
/// @brief Runs every frame during the first bounce
/// @details Updates the fire snake's trajectory position based on its bounce direction and fall
/// speed, with an initial upwards velocity of `90.0f`. If @ref COL_CHECK_DELAY_FRAMES have elapsed,
/// checks for collisions with the floor and transitions to the rest state if a collision is
/// detected.
void ObjectFireSnakeV::calcHighBounce() {
    constexpr f32 INITIAL_Y_VELOCITY = 90.0f;

    m_trajectoryPos.z += m_bounceDir.z * m_fallSpeed;
    m_trajectoryPos.x += m_bounceDir.x * m_fallSpeed;
    m_trajectoryPos.y += INITIAL_Y_VELOCITY - GRAVITY * static_cast<f32>(m_currentFrame);

    if (m_currentFrame > COL_CHECK_DELAY_FRAMES) {
        CollisionInfo colInfo;

        bool hasCol = CollisionDirector::Instance()->checkSphereFull(RADIUS, m_trajectoryPos,
                EGG::Vector3f::inf, KCL_TYPE_FLOOR, &colInfo, nullptr, 0);

        if (hasCol) {
            m_trajectoryPos += colInfo.tangentOff;
            m_nextStateId = 3;
        }
    }

    setMatrixTangentTo(EGG::Vector3f::ey, m_bounceDir);
    setPos(m_trajectoryPos);
}

/// @brief Runs every frame while the fire snake is in between jumps
/// @details Calls the base class implementation to handle the fire snake's rest behavior.
/// @note This is a separate function definition because @ref StateManagerEntry expects a member
/// function of the derived type, thus we cannot pass a @ref ObjectFireSnake pointer directly.
void ObjectFireSnakeV::calcRest() {
    ObjectFireSnake::calcRest();
}

/// @addr{0x806C2254}
/// @brief Runs every frame the fire snake is bouncing, except the first bounce
/// @details Calls the base class implementation to handle the fire snake's bounce behavior.
/// @note This is a separate function definition because @ref StateManagerEntry expects a member
/// function of the derived type, thus we cannot pass a @ref ObjectFireSnake pointer directly.
void ObjectFireSnakeV::calcBounce() {
    ObjectFireSnake::calcBounce();
}

} // namespace Kinoko::Field
