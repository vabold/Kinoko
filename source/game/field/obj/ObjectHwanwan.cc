#include "ObjectHwanwan.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/CollisionInfo.hh"
#include "game/field/RailInterpolator.hh"

#include "game/kart/KartObject.hh"
#include "game/kart/Reaction.hh"

#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

/// @addr{0x806E95B0}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Caches the Chain Chomp's initial position to @ref m_initPos.
ObjectHwanwan::ObjectHwanwan(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), StateManager(this, STATE_ENTRIES), m_initPos(pos()) {}

/// @addr{0x806EC6E0}
/// @brief Default virtual destructor
ObjectHwanwan::~ObjectHwanwan() = default;

/// @addr{0x806E9724}
/// @copybrief ObjectBase::init()
/// @details Initializes the Chain Chomp's position such that the bottom of its spherical body is at
/// the initial position. Resets velocity and orientation vectors. Updates the Chain Chomp's
/// transform and initializes it to the bouncing/walking state.
void ObjectHwanwan::init() {
    m_workPos = m_initPos + EGG::Vector3f::ey * DIAMETER;
    m_extVel.setZero();
    m_bounceVel.setZero();
    m_tangent = EGG::Vector3f::ez;
    m_up = EGG::Vector3f::ey;
    m_targetUp = EGG::Vector3f::ey;
    m_touchingGround = false;
    m_targetY = m_workPos.y;

    calcTransform();

    m_nextStateId = 0;
}

/// @addr{0x806E9A78}
/// @copybrief ObjectBase::calc()
/// @details Evaluates the Chain Chomp's state machine. Applies a downward gravitational force of
/// `2.5f` every frame, updating its velocity and position accordingly. Calls @ref
/// checkFloorCollision() to see if the Chain Chomp should bounce. Finally, it updates the Chain
/// Chomp's transform to reflect its updated position and orientation.
void ObjectHwanwan::calc() {
    constexpr EGG::Vector3f GRAVITY = EGG::Vector3f(0.0f, 2.5f, 0.0f);

    StateManager::calc();

    m_extVel += m_bounceVel - GRAVITY;
    m_workPos += m_extVel;
    m_bounceVel.setZero();

    checkFloorCollision();

    EGG::Matrix34f mat;
    SetRotTangentHorizontal(mat, m_up, m_tangent);
    mat.setBase(3, m_workPos);
    setTransform(mat);
}

/// @addr{0x806EC7A8}
/// @copybrief ObjectBase::loadRail()
/// @details No-op since its position is enforced by @ref ObjectHwanwanManager.
void ObjectHwanwan::loadRail() {}

/// @addr{0x806EC7B8}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectHwanwan::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x806EC7AC}
/// @copybrief ObjectBase::getKclName()
/// @return The model name of the Chain Chomp, `wanwan`
const char *ObjectHwanwan::getKclName() const {
    return "wanwan";
}

/// @addr{0x806E9CAC}
/// @copybrief ObjectCollidable::onCollision()
/// @param kartObj The kart object that collided with this object
/// @param reactionOnKart The reaction that should be applied to the kart upon collision
/// @return @ref Kart::Reaction::WallAllSpeed if the player's speed is under 50%, otherwise @ref
/// Kart::Reaction::KnockbackBumpLoseItem.
/// @details Chain Chomps act like a wall when the kart is driving under 50% of its base speed.
Kart::Reaction ObjectHwanwan::onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
        Kart::Reaction /*reactionOnObj*/, EGG::Vector3f & /*hitDepth*/) {
    return kartObj->speedRatioCapped() < 0.5f ? Kart::Reaction::WallAllSpeed : reactionOnKart;
}

/// @addr{0x806E9E10}
/// @brief Runs every frame that the Chain Chomp is walking/bouncing along the rail
/// @details If the Chain Chomp is touching the ground this frame, induces an upwards bounce
/// velocity of `12.0f`.
/// @note For all intents and purposes, this is run every frame. The only time this is not
/// run is when the Chain Chomp enters a "roll" state. This state is only ever used in the
/// Rainbow Road tournament, when a Chain Chomp falls off the higher path by the split path
/// section after the cannon.
void ObjectHwanwan::calcBounce() {
    if (m_touchingGround) {
        m_bounceVel += EGG::Vector3f::ey * 12.0f;
    }
}

/// @addr{0x806EA784}
/// @brief Checks for a collision between the Chain Chomp and the floor
/// @details If a collision is detected and the Chain Chomp's Y-position is within `300.0f` units of
/// the rail height, sets @ref m_touchingGround to `true`, offsets the Chain Chomp's position to be
/// on the floor, sets @ref m_targetUp to the floor's normal, and resets the Y-component of external
/// velocity to zero.
void ObjectHwanwan::checkFloorCollision() {
    constexpr f32 RADIUS = DIAMETER * 0.5f;

    m_touchingGround = false;

    CollisionInfo colInfo;
    KCLTypeMask mask;
    EGG::Vector3f pos = m_workPos + EGG::Vector3f(0.0f, -DIAMETER, 0.0f);

    bool hasCol = CollisionDirector::Instance()->checkSphereFullPush(RADIUS, pos,
            EGG::Vector3f::inf, KCL_TYPE_FLOOR, &colInfo, &mask, 0);

    if (!hasCol || pos.y - m_targetY >= 300.0f) {
        return;
    }

    m_touchingGround = true;

    f32 len = colInfo.tangentOff.length();
    m_workPos += EGG::Vector3f::ey * len;

    if (colInfo.floorDist > -std::numeric_limits<f32>::min()) {
        m_targetUp = colInfo.floorNrm;
    }
    m_extVel.y = 0.0f;
}

/// @addr{0x806EAAE8}
/// @brief Smoothly interpolates the Chain Chomp's up vector towards the target up vector
/// @details Uses an interpolation rate of `0.1f`.
void ObjectHwanwan::calcUp() {
    constexpr f32 INTERP_RATE = 0.1f;

    m_up = m_up.interpolate(INTERP_RATE, m_targetUp);
    if (m_up.squaredLength() > std::numeric_limits<f32>::epsilon()) {
        m_up.normalise2();
    } else {
        m_up = EGG::Vector3f::ey;
    }
}

/// @addr{0x806C5354}
/// @brief Constructor
/// @param params The parameters used to initialize the object
/// @details Constructs the underlying @ref ObjectHwanwan object, doubles its scale, and loads
/// it.
ObjectHwanwanManager::ObjectHwanwanManager(const System::MapdataGeoObj &params)
    : ObjectCollidable(params) {
    m_hwanwan = EGG::egg_new<ObjectHwanwan>(params);
    m_hwanwan->setScale(2.0f);
    m_hwanwan->load();
}

/// @addr{0x806C56DC}
/// @brief Default virtual destructor
ObjectHwanwanManager::~ObjectHwanwanManager() = default;

/// @addr{0x806C571C}
/// @copybrief ObjectBase::init()
/// @details Initializes the rail interpolator to the beginning of the rail. Sets the Chain Chomp's
/// position and tangent to match the rail's initial position and orientation, and calls @ref
/// ObjectHwanwan::calc() twice to update its state accordingly. Finally, sets the rail
/// interpolator's speed based on param setting 1.
/// @note It is not clear why @ref ObjectHwanwan::calc() is called twice, but this must be done in
/// Kinoko to match the base game's behavior.
void ObjectHwanwanManager::init() {
    m_railInterpolator->init(0.0f, 0);
    m_hwanwan->m_tangent = m_railInterpolator->curTangentDir();
    const auto &curPos = m_railInterpolator->curPos();
    m_hwanwan->m_workPos.x = curPos.x;
    m_hwanwan->m_workPos.z = curPos.z;
    m_hwanwan->m_targetY = curPos.y;

    m_hwanwan->calc();
    m_hwanwan->calc();

    ASSERT(m_mapObj);
    m_railInterpolator->setCurrVel(static_cast<f32>(m_mapObj->setting(0)));
}

/// @addr{0x806C5AC4}
/// @copybrief ObjectBase::calc()
/// @details Updates the Chain Chomp's state and position along the rail every frame.
void ObjectHwanwanManager::calc() {
    calcState();
    calcPosAndTangent();
}

/// @addr{0x806C69B4}
/// @copybrief ObjectBase::loadGraphics()
/// @details No-op since this is just a manager class.
void ObjectHwanwanManager::loadGraphics() {}

/// @addr{0x806C69B0}
/// @copybrief ObjectBase::createCollision()
/// @details No-op since this is just a manager class.
void ObjectHwanwanManager::createCollision() {}

/// @addr{0x806C69B8}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectHwanwanManager::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x806C5DE0}
/// @brief Updates the Chain Chomp's state based on the current rail segment
/// @details If the Chain Chomp has reached the end of the current rail segment and the next
/// rail point's second setting is set to 1, then transitions to the rolling state. Otherwise, if
/// the Chain Chomp is in the rolling state and has been rolling for 60 frames, it transitions back
/// to the walking state.
/// @note In practice, for Nintendo tracks, this does nothing. Rail point setting 2, which
/// represents the Chain Chomp entering a roll animation is only ever set to 1 in the Rainbow
/// Road tournament.
void ObjectHwanwanManager::calcState() {
    constexpr u32 ROLL_DURATION = 60.0f;

    if (m_railInterpolator->calc() == RailInterpolator::Status::SegmentEnd &&
            m_railInterpolator->curPoint().setting[1] == 1 && m_hwanwan->m_currentStateId != 2) {
        m_hwanwan->m_nextStateId = 1;
    }

    if (m_hwanwan->m_currentStateId == 1 && m_hwanwan->m_currentFrame >= ROLL_DURATION) {
        m_hwanwan->m_nextStateId = 0;
    }
}

/// @addr{0x806C6148}
/// @brief Updates the Chain Chomp's position and tangent
/// @details Snaps the Chain Chomp's X and Z position to the rail and sets the target Y position
/// to the rail's height. Also updates the Chain Chomp's tangent direction.
void ObjectHwanwanManager::calcPosAndTangent() {
    const auto &curPos = m_railInterpolator->curPos();
    m_hwanwan->m_workPos.x = curPos.x;
    m_hwanwan->m_workPos.z = curPos.z;
    m_hwanwan->m_targetY = curPos.y;
    m_hwanwan->m_tangent = m_railInterpolator->curTangentDir();
}

} // namespace Kinoko::Field
