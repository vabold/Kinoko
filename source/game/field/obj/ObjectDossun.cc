#include "ObjectDossun.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/CollisionInfo.hh"
#include "game/field/ObjectCollisionBase.hh"
#include "game/field/ObjectDirector.hh"
#include "game/field/RailInterpolator.hh"

#include "game/kart/KartObject.hh"

#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

/// @addr{0x80764510}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Initializes @ref m_touchingGround to `false`.
ObjectDossun::ObjectDossun(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_touchingGround(false) {}

/// @addr{0x8075EE68}
/// @brief Default virtual destructor
ObjectDossun::~ObjectDossun() = default;

/// @addr{0x8075EEA8}
/// @copybrief ObjectBase::init()
/// @details Initializes the Thwomp's animation state to @ref AnmState::Still and calls @ref
/// initState(). Caches the Thwomp's initial vertical position to @ref m_initialPosY and clears @ref
/// m_vel. Mainly, this function determines the total duration of the Thwomp's stomp cycle based off
/// of how long it takes the Thwomp to stomp down onto the floor beneath it.
/// @note Since @ref m_fullDuration computes the expected duration of the stomp animation, Thwomps
/// will end stomping at this duration, even if the floor beneath it has been lowered. This may
/// result in Thwomp stomps ending mid-air on some custom tracks.
void ObjectDossun::init() {
    constexpr f32 BEFORE_FALL_VEL = 30.0f;

    m_anmState = AnmState::Still;

    initState();

    if (m_railInterpolator) {
        m_railInterpolator->init(0.0f, 0);
    }

    m_initialPosY = pos().y;
    m_vel = 0.0f;

    for (u32 i = 0; i < BEFORE_FALL_DURATION; ++i) {
        setPos(EGG::Vector3f(pos().x, BEFORE_FALL_VEL + pos().y, pos().z));
    }

    bool hasCol = false;
    auto *colDir = CollisionDirector::Instance();
    CollisionInfo info;
    u32 frameCount = 0;

    do {
        ++frameCount;
        m_vel -= STOMP_ACCEL;
        setPos(EGG::Vector3f(pos().x, m_vel + pos().y, pos().z));

        info.reset();
        EGG::Vector3f colPos = pos() + STOMP_POS_OFFSET;

        hasCol = colDir->checkSphereFull(STOMP_RADIUS, colPos, EGG::Vector3f::inf, KCL_TYPE_FLOOR,
                &info, nullptr, 0);
    } while (!hasCol);

    addPos(info.tangentOff);
    m_vel = 0.0f;

    f32 fallDuration = frameCount;

    frameCount = 0;
    f32 riseDuration;

    while (true) {
        f32 posY = RISING_VEL + pos().y;
        ++frameCount;

        if (posY >= m_initialPosY) {
            setPos(EGG::Vector3f(pos().x, m_initialPosY, pos().z));
            riseDuration = frameCount;
            break;
        } else {
            setPos(EGG::Vector3f(pos().x, posY, pos().z));
        }
    }

    m_fullDuration = fallDuration + GROUND_DURATION + riseDuration + BEFORE_FALL_DURATION;
    m_touchingGround = false;
}

/// @addr{0x807648A4}
/// @copybrief ObjectBase::calcCollisionTransform()
/// @details Scales up the height of the Thwomp's transformation matrix by `400.0f`.
void ObjectDossun::calcCollisionTransform() {
    constexpr f32 HEIGHT = 400.0f;

    if (!m_collision) {
        return;
    }

    calcTransform();
    EGG::Matrix34f mat = transform();
    mat[1, 3] += HEIGHT * scale().x;
    collision()->transform(mat, scale());
}

/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectDossun::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x8075FF98}
/// @copybrief ObjectCollidable::onCollision()
/// @param kartObj The kart object that collided with this object
/// @param reactionOnKart The reaction that should be applied to the kart upon collision
/// @param reactionOnObj The reaction that should be applied to the object upon collision (unused)
/// @param hitDepth The depth of the collision between the kart and the object
/// @return @ref Kart::Reaction::LongCrushLoseItem if the player should be crushed, otherwise @ref
/// Kart::Reaction::WallAllSpeed.
/// @details Squishes the player if they are within a certain distance of a falling Thwomp. A squish
/// can still technically occur when the Thwomp is touching the ground, but this is not possible in
/// practice because the player will be pushed away by the Thwomp's collision before it gets within
/// the squish radius.
Kart::Reaction ObjectDossun::onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
        Kart::Reaction /*reactionOnObj*/, EGG::Vector3f & /*hitDepth*/) {
    constexpr f32 SQUISH_DISTANCE = 375.0f;

    EGG::Vector3f xzDist = kartObj->pos() - pos();
    xzDist.y = 0.0f;

    if (xzDist.length() < SQUISH_DISTANCE * scale().x &&
            (m_anmState == AnmState::Falling || m_touchingGround)) {
        const auto &hitTable = ObjectDirector::Instance()->hitTableKart();
        return hitTable.reaction(hitTable.slot(ObjectId::DossuncSoko));
    }

    return reactionOnKart;
}

/// @addr{0x8075FB50}
/// @brief Runs once when the Thwomp resets after a stomp
/// @details Sets @ref m_anmState to @ref AnmState::Still, resets @ref m_shakePhase to zero and
/// @ref m_vel to zero, and resets the Thwomp's yaw to @ref m_currYaw.
void ObjectDossun::startStill() {
    m_anmState = AnmState::Still;
    m_shakePhase = 0;
    m_vel = 0.0f;
    setRot(EGG::Vector3f(rot().x, m_currYaw, rot().z));
}

/// @addr{0x8075F254}
/// @brief Updates the Thwomp's animation state related to stomping
/// @details Calls the appropriate calculation function based on the current animation state. If the
/// stomp duration reaches zero, transitions the Thwomp to the still state.
void ObjectDossun::calcStomp() {
    switch (m_anmState) {
    case AnmState::BeforeFall:
        calcBeforeFall();
        break;
    case AnmState::Falling:
        calcFalling();
        break;
    case AnmState::Grounded:
        calcGrounded();
        break;
    case AnmState::Rising:
        calcRising();
        break;
    default:
        break;
    }

    if (m_stompDuration-- == 0) {
        startStill();
    }
}

/// @addr{0x8075F21C}
/// @brief Initializes @ref m_stillTimer to param setting 3 (or param setting 4 if setting 3 is
/// 0). Also resets @ref m_groundedTimer, @ref m_beforeFallTimer, and @ref m_shakePhase to zero.
void ObjectDossun::initState() {
    ASSERT(m_mapObj);
    m_stillTimer = static_cast<u32>(m_mapObj->setting(2));
    if (m_stillTimer == 0) {
        m_stillTimer = static_cast<u32>(m_mapObj->setting(3));
    }

    m_groundedTimer = 0;
    m_beforeFallTimer = 0;
    m_shakePhase = 0;
}

/// @addr{0x8075F3F4}
/// @brief Called when the Thwomp lurches upwards before crushing down
/// @details Raises the Thwomp's height by `50.0f` every frame while in this animation state.
/// Once @ref m_beforeFallTimer reaches zero, the Thwomp transitions to the falling animation
/// state.
void ObjectDossun::calcBeforeFall() {
    constexpr f32 BEFORE_FALL_VEL = 50.0f;

    setPos(EGG::Vector3f(pos().x, BEFORE_FALL_VEL + pos().y, pos().z));

    if (--m_beforeFallTimer == 0) {
        m_anmState = AnmState::Falling;
    }
}

/// @addr{0x8075F430}
/// @brief Runs every frame while the Thwomp is stomping downwards
/// @details Decreases the Thwomp's vertical velocity by @ref STOMP_ACCEL and updates its
/// height accordingly. Checks for floor collision and sets @ref m_touchingGround if a collision
/// occurs.
void ObjectDossun::calcFalling() {
    m_vel -= STOMP_ACCEL;
    setPos(EGG::Vector3f(pos().x, m_vel + pos().y, pos().z));
    checkFloorCollision();
}

/// @addr{0x8075FE8C}
/// @brief Runs once when the Thwomp hits the ground after a stomp
/// @details Resets @ref m_anmState to @ref AnmState::Grounded and @ref m_groundedTimer to @ref
/// GROUND_DURATION.
void ObjectDossun::startGrounded() {
    m_anmState = AnmState::Grounded;
    m_groundedTimer = GROUND_DURATION;
}

/// @addr{0x8075F460}
/// @brief Runs every frame while the Thwomp is on the ground after a stomp
/// @details Decrements @ref m_groundedTimer each frame and transitions to the rising state
/// once it reaches zero.
void ObjectDossun::calcGrounded() {
    if (--m_groundedTimer == 0) {
        m_anmState = AnmState::Rising;
    }
}

/// @addr{0x8075F4D8}
/// @brief Runs every frame while the Thwomp is rising to its initial position after a stomp
/// @details Increases the Thwomp's height by @ref RISING_VEL each frame until it reaches @ref
/// m_initialPosY.
void ObjectDossun::calcRising() {
    f32 posY = std::min(RISING_VEL + pos().y, m_initialPosY);
    setPos(EGG::Vector3f(pos().x, posY, pos().z));
}

/// @addr{0x8075F76C}
/// @brief Checks for collision with the floor while the Thwomp is falling
/// @details Uses a sphere radius of size @ref STOMP_RADIUS to check for collisions with the floor.
/// If a collision occurs, sets @ref m_vel to zero, applies the collision offset to the Thwomp's
/// position to prevent it from clipping into the floor, calls @ref startGrounded(), and sets @ref
/// m_touchingGround to true.
void ObjectDossun::checkFloorCollision() {
    CollisionInfo info;
    EGG::Vector3f colPos = pos() + STOMP_POS_OFFSET;

    if (!CollisionDirector::Instance()->checkSphereFull(STOMP_RADIUS, colPos, EGG::Vector3f::inf,
                KCL_TYPE_FLOOR, &info, nullptr, 0)) {
        return;
    }

    m_vel = 0.0f;
    addPos(info.tangentOff);

    startGrounded();
    m_touchingGround = true;
}

} // namespace Kinoko::Field
