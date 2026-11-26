#include "ObjectWanwan.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/CollisionInfo.hh"
#include "game/field/ObjectDirector.hh"

#include "game/kart/KartObject.hh"
#include "game/kart/Reaction.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/RaceManager.hh"
#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

/// @addr{Inlined in 0x806E4224}
/// @brief Constructor
/// @param pos The initial position of the stake
/// @param rot The initial rotation of the stake
/// @param scale The initial scale of the stake
ObjectWanwanPile::ObjectWanwanPile(const EGG::Vector3f &pos, const EGG::Vector3f &rot,
        const EGG::Vector3f &scale)
    : ObjectCollidable("pile", pos, rot, scale) {}

/// @addr{0x806E9568}
/// @brief Default virtual destructor
ObjectWanwanPile::~ObjectWanwanPile() = default;

/// @addr{0x806E9560}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame
ObjectBase::LoadFlags ObjectWanwanPile::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x806E9554}
/// @copybrief ObjectBase::getResources()
/// @return The resource name for the Chain Chomp, `wanwan`
const char *ObjectWanwanPile::getResources() const {
    return "wanwan";
}

/// @addr{0x806E9548}
/// @copybrief ObjectBase::getKclName()
/// @return The model name of the stake, `pile`
const char *ObjectWanwanPile::getKclName() const {
    return "pile";
}

/// @addr{0x806E4224}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Initializes @ref m_pitch to zero and @ref m_chainAttachMat to the identity matrix. Sets
/// @ref m_chainLength based on param setting 1, @ref m_attackDistance based on `4800.0f` plus param
/// setting 3, @ref m_attackCenterX based on `10.0f` times param setting 4, and @ref
/// m_attackCenterZ based on `10.0f` times param setting 5. Calculates @ref m_wanderDuration and
/// @ref m_attackArc based on param settings 6 and 7 respectively, with special handling if the
/// setting is zero.
///
/// Constructs the wooden stake object to which the Chain Chomp is attached. Computes the @ref
/// m_anchor point which is effectively the top of the @ref ObjectWanwanPile. Doubles the Chain
/// Chomp's scale. Computes the number of chains based on @ref m_chainLength. Computes the Chain
/// Chomp's @ref m_initPos such that it is half of @ref m_chainLength away from the @ref m_anchor
/// point in both the X and Z direction, with an upwards vertical displacement of `600.0f` units.
/// Computes the wandering constraint point based on the current course being played. Finally, calls
/// @ref initTransformKeyframes() to initialize the chain attachment point's animation cycle.
ObjectWanwan::ObjectWanwan(const System::MapdataGeoObj &params)
    : ObjectCollidable(params),
      StateManager(this, STATE_ENTRIES),
      m_pitch(0.0f),
      m_chainLength(static_cast<f32>(params.setting(0))),
      m_attackDistance(4800.0f + static_cast<f32>(params.setting(2))),
      m_attackCenterX(10.0f * static_cast<f32>(static_cast<s16>(params.setting(3)))),
      m_attackCenterZ(10.0f * static_cast<f32>(static_cast<s16>(params.setting(4)))),
      m_wanderDuration(initWanderDuration(params.setting(5))),
      m_attackArc(initAttackArc(static_cast<f32>(params.setting(6)))),
      m_chainAttachMat(EGG::Matrix34f::ident) {
    constexpr EGG::Vector3f ANCHOR_OFFSET = EGG::Vector3f(0.0f, 20.0f, 0.0f);
    constexpr EGG::Vector3f POS_OFFSET_MCWII = EGG::Vector3f(14500.0f, 1300.0f, 44850.0f);
    constexpr EGG::Vector3f POS_OFFSET_RMC = EGG::Vector3f(8012.0f, 1668.0f, -30150.0f);

    auto *pile = EGG::egg_new<ObjectWanwanPile>(pos(), rot(), scale());
    pile->load();

    m_anchor = pos() + ANCHOR_OFFSET;

    setScale(SCALE);

    f32 sqLen = m_chainLength * m_chainLength + 300.0f * (300.0f * scale().y) * scale().y;
    m_chainCount = static_cast<u32>(EGG::Mathf::sqrt(sqLen) / (CHAIN_LENGTH * scale().y));
    if (m_chainCount > 0) {
        --m_chainCount;
    }

    m_initPos = m_anchor + EGG::Vector3f(0.5f * -m_chainLength, 600.0f, 0.5f * -m_chainLength);

    auto course = System::RaceConfig::Instance()->raceScenario().course;
    if (course == Course::Mario_Circuit) {
        EGG::Vector3f pos = POS_OFFSET_MCWII - m_anchor;
        pos.y = 0.0f;
        pos.normalise2();
        m_wanderConstraintPoint = pos * m_chainLength + m_anchor;
        m_wanderConstraintPoint.y = POS_OFFSET_MCWII.y;
    } else if (course == Course::GCN_Mario_Circuit) {
        EGG::Vector3f pos = POS_OFFSET_RMC - m_anchor;
        pos.y = 0.0f;
        pos.normalise2();
        m_wanderConstraintPoint = pos * m_chainLength + m_anchor;
        m_wanderConstraintPoint.y = POS_OFFSET_RMC.y;
    } else {
        m_wanderConstraintPoint = m_anchor;
        m_wanderConstraintPoint.z += m_chainLength;
    }

    initTransformKeyframes();
}

/// @addr{0x806E4AEC}
/// @brief Default virtual destructor
ObjectWanwan::~ObjectWanwan() = default;

/// @addr{0x806E4B9C}
/// @copybrief ObjectBase::init()
/// @details Sets the Chain Chomp's position and @ref m_chainAttachPos to @ref m_initPos. Resets
/// @ref m_vel, @ref m_accel, and @ref m_speed to zero. Sets the orientation vectors (@ref
/// m_tangent, @ref m_up, @ref m_targetUp, @ref m_targetDir, and @ref m_backDir) to their initial
/// values. Resets @ref m_touchingFloor, @ref m_chainTaut, @ref m_retarget, and @ref m_attackStill
/// to false. Initializes @ref m_frame and @ref m_wanderTimer to zero. Finally, transitions the
/// Chain Chomp to the wait state.
void ObjectWanwan::init() {
    setPos(m_initPos);
    m_chainAttachPos = m_initPos;
    m_vel.setZero();
    m_accel.setZero();
    m_speed = 0.0f;
    m_tangent = EGG::Vector3f::ex;
    m_up = EGG::Vector3f::ey;
    m_targetUp = EGG::Vector3f::ey;
    m_touchingFloor = false;
    m_chainTaut = false;
    m_frame = 0;
    m_target.setZero();
    m_targetDir = EGG::Vector3f::ez;
    m_retarget = false;
    m_wanderTimer = 0;
    m_attackStill = false;
    m_backDir = EGG::Vector3f::ex;
    m_nextStateId = 0;
}

/// @addr{0x806E4F2C}
/// @copybrief ObjectBase::calc()
/// @details Evaluates the Chain Chomp's state machine. Updates the Chain Chomp's position, checks
/// for floor collision, refreshes @ref m_chainAttachMat, and updates the Chain Chomp's chain
/// attachment point. Increments @ref m_frame. If the Chain Chomp has fallen `1000.0f` units below
/// @ref m_anchor, then the Chain Chomp snaps upwards.
void ObjectWanwan::calc() {
    StateManager::calc();

    calcPos();
    calcAttackPos();
    calcCollision();
    calcMat();
    calcChainAttachMat();
    calcChain();

    ++m_frame;

    if (pos().y < m_anchor.y - 1000.0f) {
        setPos(EGG::Vector3f(pos().x, m_anchor.y + 1000.0f, pos().z));
        m_vel.y = 0.0f;
    }
}

/// @addr{0x806E94BC}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc and @ref eLoadFlags::Draw so that the object is
/// calculated every frame
ObjectBase::LoadFlags ObjectWanwan::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc, eLoadFlags::Draw);
}

/// @addr{0x806E526C}
/// @copybrief ObjectCollidable::onCollision()
/// @param kartObj The kart object involved in the collision
/// @param reactionOnKart The reaction that should be applied to the kart upon collision
/// @param reactionOnObj The reaction that should be applied to the object upon collision
/// @param hitDepth The depth of the collision between the kart and the object
/// @return @ref Kart::Reaction::WallAllSpeed if the kart is travelling below 50% of its base speed
/// and the Chain Chomp is not lurching forward, otherwise @ref
/// Kart::Reaction::KnockbackBumpLoseItem
/// @details Chain Chomp collisions are treated as wall collisions if the kart is moving below 50%
/// its base speed and if the Chain Chomp is not lurching forward.
Kart::Reaction ObjectWanwan::onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
        Kart::Reaction /*reactionOnObj*/, EGG::Vector3f & /*hitDepth*/) {
    if (m_currentStateId == 1 && !m_attackStill) {
        return reactionOnKart;
    }

    return kartObj->speedRatioCapped() < 0.5f ? Kart::Reaction::WallAllSpeed : reactionOnKart;
}

/// @brief Helper function that handles the case when the param's wander duration is zero
/// @param duration The duration specified by the param settings
/// @return `duration` if it is non-zero, otherwise `300`
u32 ObjectWanwan::initWanderDuration(u32 duration) const {
    return duration == 0 ? 300 : duration;
}

/// @brief Helper function that handles the case when the param's attack arc is zero
/// @param arc The attack arc specified by the param settings
/// @return `arc` if it is non-zero, otherwise `30.0f`
f32 ObjectWanwan::initAttackArc(f32 arc) const {
    if (arc == 0.0f) {
        arc = 30.0f;
    }
    return arc;
}

/// @addr{0x806E8A60}
/// @brief Pre-computes a 15 frame animation cycle that describes how the Chain Chomp's chain
/// attachment point moves relative to its position
/// @note When iterating each of the keyframes to set, this function normally calls @ref
/// EGG::Matrix34f::makeR() and then manually sets the position, but we can optimize slightly by
/// calling @ref EGG::Matrix34f::makeRT() instead.
void ObjectWanwan::initTransformKeyframes() {
    std::array<f32, 15> xRot;
    SampleHermiteInterp(0.0f, 10.0f, 2.0f, -1.111111f, std::span(xRot.begin(), 6));
    SampleHermiteInterp(10.0f, -10.0f, -1.111111f, -1.111111f, std::span(xRot.begin() + 5, 5));
    SampleHermiteInterp(-10.0f, 0.0f, -1.111111f, 2.0f, std::span(xRot.begin() + 9, 6));

    std::array<f32, 15> yPos;
    SampleHermiteInterp(0.0f, -27.35f, -9.1166658f, 3.75f, std::span(yPos.begin(), 4));
    SampleHermiteInterp(-27.35f, 33.75f, 3.75f, 2.4863639f, std::span(yPos.begin() + 3, 7));
    SampleHermiteInterp(33.75f, 0.0f, 2.4863639f, -6.75f, std::span(yPos.begin() + 9, 6));

    std::array<f32, 15> zPos;
    SampleHermiteInterp(0.0f, -33.75f, -6.75f, -4.8214278f, std::span(zPos.begin(), 6));
    SampleHermiteInterp(-33.75f, -33.75f, -4.8214278f, 8.4375f, std::span(zPos.begin() + 5, 3));
    SampleHermiteInterp(-33.75f, 0.0f, 8.4375f, 14.464999f, std::span(zPos.begin() + 7, 3));
    SampleHermiteInterp(0.0f, 24.11f, 14.464999f, 0.0f, std::span(zPos.begin() + 9, 3));
    SampleHermiteInterp(24.11f, 0.0f, 0.0f, -8.0366669f, std::span(zPos.begin() + 11, 4));

    for (u8 i = 0; i < m_transformKeyframes.size(); ++i) {
        EGG::Vector3f rot = EGG::Vector3f(xRot[i] * DEG2RAD, 0.0f, 0.0f);
        EGG::Vector3f pos = EGG::Vector3f(0.0f, yPos[i], zPos[i]);
        EGG::Matrix34f mat;
        mat.makeRT(rot, pos);
        m_transformKeyframes[i] = mat;
    }
}

/// @addr{0x806E6208}
/// @brief Runs once when the Chain Chomp enters the wandering state
/// @details Resets @ref m_retarget to false. Resets the X and Z components of @ref m_vel and @ref
/// m_accel to zero. Determines a random angle within `[~39.6, ~59.4]` degrees for the Chain Chomp
/// to wander towards, biased towards the side away from the anchor. Finally, computes @ref m_target
/// based on the random angle, @ref m_chainLength, @ref m_anchor, and an additional scalar depending
/// on the length of the chain.
void ObjectWanwan::enterWait() {
    constexpr f32 ANGLE_RANGE = 0.33f * 60.0f;
    constexpr f32 ANGLE_NORMALIZATION = 0.66f * 60.0f;
    constexpr f32 LONG_CHAIN_LENGTH = 3000.0f;
    constexpr f32 SHORT_CHAIN_TARGET_SCALAR = 0.5f;
    constexpr f32 LONG_CHAIN_TARGET_SCALAR = 0.7f;

    m_retarget = false;
    m_vel.x = 0.0f;
    m_vel.z = 0.0f;
    m_accel.x = 0.0f;
    m_accel.z = 0.0f;
    m_speed = 0.0f;

    f32 randAngle =
            System::RaceManager::Instance()->random().getF32(ANGLE_RANGE) + ANGLE_NORMALIZATION;

    if (CrossXZ(pos() + m_tangent, pos(), m_anchor) >= 0.0f) {
        randAngle *= -1.0f;
    }

    EGG::Vector3f toAnchor = m_anchor - EGG::Vector3f(pos().x, m_anchor.y, pos().z);
    toAnchor.normalise2();

    EGG::Vector3f wanderDir = RotateXZByYaw(DEG2RAD * randAngle, toAnchor);
    f32 targetDistScalar = m_chainLength < LONG_CHAIN_LENGTH ? SHORT_CHAIN_TARGET_SCALAR :
                                                               LONG_CHAIN_TARGET_SCALAR;
    m_target = wanderDir * m_chainLength * targetDistScalar + m_anchor;
}

/// @addr{0x806E6EB0}
/// @brief Runs once when the Chain Chomp enters the attacking state
/// @details Resets the X and Z components of @ref m_vel and @ref m_accel to zero. Resets the Chain
/// Chomp's @ref m_speed, @ref m_pitch, and @ref m_wanderTimer to zero. Resets @ref m_attackStill
/// and @ref m_chainTaut to false. Raises the Y component of @ref m_anchor by `30.0f` units.
/// Finally, calculates a random target position for the Chain Chomp to lunge towards.
void ObjectWanwan::enterAttack() {
    m_vel.x = 0.0f;
    m_vel.z = 0.0f;
    m_accel.x = 0.0f;
    m_accel.z = 0.0f;
    m_speed = 0.0f;
    m_pitch = 0.0f;
    m_wanderTimer = 0;
    m_attackStill = false;
    m_chainTaut = false;
    m_anchor.y += 30.0f;

    calcRandomTarget();
}

/// @addr{0x806E73C4}
/// @brief Runs once when the Chain Chomp enters the retreating state
/// @details Resets the X and Z components of @ref m_vel and @ref m_accel to zero. Sets @ref m_speed
/// to `15.0f`. Sets @ref m_pitch based on @ref ObjectDirector::WanwanMaxPitch(). Sets the X and Z
/// components of @ref m_backDir based on the difference between @ref m_anchor and the Chain Chomp's
/// position. Resets @ref m_attackStill and @ref m_chainTaut to `false`. Lowers the Y component of
/// @ref m_anchor by `30.0f`.
void ObjectWanwan::enterBack() {
    m_vel.x = 0.0f;
    m_vel.z = 0.0f;
    m_accel.x = 0.0f;
    m_accel.z = 0.0f;
    m_speed = 15.0f;
    m_pitch = -ObjectDirector::Instance()->WanwanMaxPitch();
    m_backDir = m_anchor - pos();
    m_backDir.y = 0.0f;
    m_backDir.normalise2();
    m_attackStill = false;
    m_chainTaut = false;
    m_anchor.y -= 30.0f;
}

/// @addr{0x806E63A4}
/// @brief Runs once per frame when the Chain Chomp is in the wandering state
/// @details Updates @ref m_targetDir based on the Chain Chomp's current position relative to @ref
/// m_target. Determines if the Chain Chomp needs to change its wandering target position if it has
/// wandered close enough to the previous target position. Interpolates @ref m_tangent and @ref m_up
/// towards their targets. Finally, updates the Chain Chomp's speed, checks to see if the Chain
/// Chomp is bouncing off the floor, and handles wandering behavior.
/// @bug If the Chain Chomp's position is exactly equal to the target position, then the local
/// variable `distFromTarget` will be uninitialized. To reflect base game behavior, we observe that
/// `r31` contains `F_PI`, thus we mimic that behavior by initializing `distFromTarget` to `F_PI`.
void ObjectWanwan::calcWait() {
    constexpr f32 RETARGET_DISTANCE_THRESHOLD = 0.55f;
    constexpr f32 ANGLE_RANGE = 0.33f * 0.5f * 60.0f;
    constexpr f32 ANGLE_NORMALIZATION = 0.66f * 0.5f * 60.0f;
    constexpr f32 TANGENT_INTERP_RATE = 0.04f;
    constexpr f32 UP_INTERP_RATE = 0.1f;

    EGG::Vector3f targetDir = m_target - pos();
    targetDir.y = 0.0f;

    // In the base game, if the dot product is less than epsilon, then r31 contains F_PI
    f32 distFromTarget = F_PI;
    if (targetDir.squaredLength() > std::numeric_limits<f32>::epsilon()) {
        distFromTarget = targetDir.normalise();
    }

    if (!m_retarget && distFromTarget < RETARGET_DISTANCE_THRESHOLD * m_chainLength) {
        EGG::Vector3f relTarget = m_target - m_anchor;
        auto &rand = System::RaceManager::Instance()->random();
        f32 angle = rand.getF32(ANGLE_RANGE) + ANGLE_NORMALIZATION;
        if (CrossXZ(m_target, m_wanderConstraintPoint, m_anchor) >= 0.0f) {
            angle *= -1.0f;
        }

        m_target = RotateXZByYaw(angle * DEG2RAD, relTarget) + m_anchor;
        m_retarget = true;
    }

    m_targetDir = targetDir;

    calcTangent(TANGENT_INTERP_RATE);
    calcUp(UP_INTERP_RATE);

    calcSpeed();
    calcBounce();

    calcWander();
}

/// @addr{0x806E6F6C}
/// @brief Runs once per frame when the Chain Chomp is in the attacking state
/// @details If the Chain Chomp has been attacking for 2 seconds, then it transitions to the
/// retreating state. Updates the Chain Chomp's pitch so that it approaches the maximum pitch
/// defined by @ref ObjectDirector::WanwanMaxPitch(). Interpolates @ref m_tangent and @ref m_up
/// towards their targets. If the chain has become taut, then enforces the Chain Chomp's position is
/// clamped to the chain length. If the chain is taut or if the Chain Chomp is stationary in the
/// attack state, then sets @ref m_attackStill to `true`, sets the Y component of @ref m_vel to
/// zero, and flips and decays @ref m_vel by `0.85f`. Otherwise, if the chain is not taut and the
/// Chain Chomp is still lurching forward, the X and Z components of @ref m_vel are set based on
/// @ref m_tangent with a speed of `120.0f`.
void ObjectWanwan::calcAttack() {
    constexpr u32 ATTACK_DURATION = 120;
    constexpr f32 PITCH_STEP = 25.0f;
    constexpr f32 TANGENT_INTERP_RATE = 10.0f * 0.04f;
    constexpr f32 UP_INTERP_RATE = 0.1f;
    constexpr f32 STILL_VEL_DECAY = -0.85f;
    constexpr f32 ATTACK_SPEED = 120.0f;

    if (m_currentFrame > ATTACK_DURATION) {
        m_nextStateId = 2;
    }

    f32 maxPitch = ObjectDirector::Instance()->WanwanMaxPitch();
    if (EGG::Mathf::abs(m_pitch) < EGG::Mathf::abs(maxPitch)) {
        m_pitch -= maxPitch / PITCH_STEP;
    }

    calcTangent(TANGENT_INTERP_RATE);
    calcUp(UP_INTERP_RATE);

    if (m_chainTaut || m_attackStill) {
        if (!m_attackStill) {
            m_target = m_anchor + (m_chainAttachPos - m_anchor) * SCALE;
            m_targetDir = m_target - pos();
            m_targetDir.y = 0.0f;
            m_targetDir.normalise2();
            EGG::Vector3f posOffset = pos() - m_chainAttachPos;
            posOffset.y = 0.0f;
            f32 radius = posOffset.length();
            EGG::Vector3f chainDir = m_chainAttachPos - m_anchor;
            chainDir.y = 0.0f;
            chainDir.normalise2();
            EGG::Vector3f nextPos = m_chainAttachPos + chainDir * radius;
            setPos(EGG::Vector3f(nextPos.x, pos().y, nextPos.z));
            EGG::Vector3f tangent = m_tangent + chainDir;
            tangent.y = 0.0f;
            if (tangent.squaredLength() > std::numeric_limits<f32>::epsilon()) {
                tangent.normalise2();
            }

            m_tangent = tangent;
        }

        m_attackStill = true;
        m_vel.y = 0.0f;
        m_vel *= STILL_VEL_DECAY;
    } else {
        m_vel.x = m_tangent.x * ATTACK_SPEED;
        m_vel.z = m_tangent.z * ATTACK_SPEED;
    }
}

/// @addr{0x806E7494}
/// @brief Runs once per frame when the Chain Chomp is in the retreating state
/// @details If the absolute value of the Chain Chomp's pitch is greater than `2.0f`, then
/// it interpolates the pitch back towards zero by stepping towards it by @ref
/// ObjectDirector::WanwanMaxPitch() divided by `15.0f` every frame. Updates the X and Z components
/// of @ref m_vel in the direction of @ref m_backDir with a speed of `1.5f` times @ref m_speed.
/// Checks to see if the Chain Chomp is bouncing off the floor. Finally, if the Chain Chomp has been
/// retreating for more than 90 frames, then it transitions to the wandering state.
void ObjectWanwan::calcBack() {
    constexpr u32 BACK_DURATION = 90;

    if (EGG::Mathf::abs(m_pitch) > 2.0f) {
        m_pitch += ObjectDirector::Instance()->WanwanMaxPitch() / 15.0f;
    }

    m_vel.x = m_backDir.x * m_speed * 1.5f;
    m_vel.z = m_backDir.z * m_speed * 1.5f;

    calcBounce();

    if (m_currentFrame > BACK_DURATION) {
        m_nextStateId = 0;
    }
}

/// @addr{0x806E59BC}
/// @brief Applies acceleration to velocity and adds velocity to the position
/// @details Updates @ref m_vel based on @ref m_accel and applies a downward gravitational force
/// of @ref GRAVITY. Updates the Chain Chomp's position accordingly and resets @ref m_accel to
/// zero.
void ObjectWanwan::calcPos() {
    m_vel += m_accel - GRAVITY;
    addPos(m_vel);
    m_accel.setZero();
}

/// @addr{0x806E5A8C}
/// @brief Performs a collision check to see if the Chain Chomp is touching the floor
/// @details If the Chain Chomp is touching the floor, then it is pushed out of the floor, updates
/// @ref m_accel to counteract the force of @ref GRAVITY, and sets @ref m_targetUp based on the
/// colliding floor's normal vector.
void ObjectWanwan::calcCollision() {
    constexpr EGG::Vector3f POS_OFFSET = EGG::Vector3f(0.0f, -570.0f, 0.0f);

    m_touchingFloor = false;
    CollisionInfo info;
    KCLTypeMask mask;
    EGG::Vector3f colPos = pos() + POS_OFFSET;
    auto *colDir = CollisionDirector::Instance();

    bool hasCol = colDir->checkSphereFullPush(30.0f, colPos, EGG::Vector3f::inf, KCL_TYPE_FLOOR,
            &info, &mask, 0);

    if (!hasCol) {
        return;
    }

    m_touchingFloor = true;
    EGG::Vector3f tangentOff = info.tangentOff;
    f32 scale = tangentOff.normalise();
    addPos(EGG::Vector3f::ey * scale);

    if (info.floorDist > -std::numeric_limits<f32>::min()) {
        m_targetUp = info.floorNrm;
    }

    m_accel += GRAVITY;
}

/// @addr{0x806E5D70}
/// @brief Sets the transform of the Chain Chomp based on its current position, pitch, and tangent
/// @details If the Chain Chomp is in the attacking state, then its orientation is based on the
/// world up vector and @ref m_tangent. Otherwise, its orientation is based on the Chain Chomp's
/// current @ref m_up and @ref m_tangent. Updates the Chain Chomp's transformation matrix
/// accordingly.
void ObjectWanwan::calcMat() {
    EGG::Matrix34f mat;
    if (m_currentStateId == 1) {
        SetRotTangentHorizontal(mat, EGG::Vector3f::ey, m_tangent);
    } else {
        SetRotTangentHorizontal(mat, m_up, m_tangent);
    }
    mat.setBase(3, EGG::Vector3f::zero);

    EGG::Vector3f rot = EGG::Vector3f(m_pitch * DEG2RAD, 0.0f, 0.0f);
    EGG::Matrix34f rtMat;
    rtMat.makeRT(rot, EGG::Vector3f::zero);
    mat = mat.multiplyTo(rtMat);
    mat.setBase(3, pos());
    setTransform(mat);
}

/// @addr{0x806E5EDC}
/// @brief Calculates the world transform matrix of the chain attachment point on the Chain Chomp
/// @details The chain attachment point's world transform is calculated based on the Chain Chomp's
/// current transformation matrix and the keyframe position of the chain attachment point. If the
/// Chain Chomp is in the attacking state, then the first keyframe is used. Otherwise, the keyframe
/// is obtained by taking @ref m_frame modulo `15`. The result is stored in @ref
/// m_chainAttachMat.
void ObjectWanwan::calcChainAttachMat() {
    u32 idx = m_currentStateId == 1 ? 0 : m_frame % 15;

    calcTransform();

    EGG::Matrix34f mat = transform();
    mat.setBase(3, EGG::Vector3f::zero);
    EGG::Vector3f keyFramePos = m_transformKeyframes[idx].base(3);
    EGG::Vector3f posOffset = mat.ps_multVector(keyFramePos) * SCALE;
    mat.setBase(3, posOffset + pos());

    m_chainAttachMat = mat;
}

/// @addr{0x806E78B0}
/// @brief Calculates the next speed of the Chain Chomp based on its current speed and tangent
/// @details If the Chain Chomp is at its max speed of `8.0f` units per frame, then @ref m_speed is
/// capped at this value, the X and Z components of @ref m_accel are reset to zero, and the X and Z
/// components of @ref m_vel are updated to reflect the capped @ref m_speed. If the Chain Chomp is
/// not at its max speed, then its @ref m_speed accelerates by `0.5f` and the X and Z components of
/// @ref m_accel are updated accordingly.
void ObjectWanwan::calcSpeed() {
    constexpr f32 MAX_SPEED = 8.0f;
    constexpr f32 ACCEL = 0.5f;

    if (m_speed < MAX_SPEED) {
        m_speed += ACCEL;
        m_accel.x = m_tangent.x * ACCEL;
        m_accel.z = m_tangent.z * ACCEL;
    } else {
        m_speed = MAX_SPEED;
        m_accel.x = 0.0f;
        m_accel.z = 0.0f;
        m_vel.x = m_tangent.x * MAX_SPEED;
        m_vel.z = m_tangent.z * MAX_SPEED;
    }
}

/// @addr{0x806E794C}
/// @brief Checks if the Chain Chomp is touching the floor and applies a bounce if it is
/// @details If the Chain Chomp is not touching the floor, then clears the Y-component of @ref
/// m_accel. Otherwise, if the Chain Chomp is touching the floor, then clears the Y-component of
/// @ref m_vel and applies an upward acceleration of `12.0f`.
void ObjectWanwan::calcBounce() {
    constexpr f32 BOUNCE_ACCEL = 12.0f;

    if (m_touchingFloor) {
        m_vel.y = 0.0f;
        m_accel += EGG::Vector3f::ey * BOUNCE_ACCEL;
    } else {
        m_accel.y = 0.0f;
    }
}

/// @addr{0x806E79E4}
/// @brief Calculates the wandering behavior of the Chain Chomp
/// @details In the base game, this behavior changes depending on whether we are in a race or a
/// time trial. For Kinoko, we simply call @ref calcWanderTimeTrial().
void ObjectWanwan::calcWander() {
    calcWanderTimeTrial();
}

/// @addr{0x806E7B7C}
/// @brief Calculates the wandering behavior of the Chain Chomp specifically for time trial mode
/// @details Increments @ref m_wanderTimer and transitions the Chain Chomp to the attack state
/// if the timer exceeds @ref m_wanderDuration.
void ObjectWanwan::calcWanderTimeTrial() {
    if (m_wanderTimer++ >= m_wanderDuration) {
        m_nextStateId = 1;
    }
}

/// @addr{0x806E7BA4}
/// @brief Calculates the world position of the chain attachment point on the Chain Chomp
/// @details Simply calls @ref calcChainAttachPos() with the current chain attachment matrix.
void ObjectWanwan::calcChain() {
    if (m_chainAttachMat.base(3).squaredLength() > std::numeric_limits<f32>::epsilon()) {
        calcChainAttachPos(m_chainAttachMat);
    }
}

/// @addr{0x806E7E38}
/// @brief Interpolates the Chain Chomp's tangent vector towards the target direction
/// @param interpRate The interpolation factor
/// @details Updates @ref m_tangent by interpolating it towards @ref m_targetDir.
void ObjectWanwan::calcTangent(f32 interpRate) {
    m_tangent = m_tangent.interpolate(interpRate, m_targetDir);
    if (m_tangent.squaredLength() > std::numeric_limits<f32>::epsilon()) {
        m_tangent.normalise2();
    } else {
        m_tangent = m_targetDir;
    }
}

/// @addr{0x806E7F64}
/// @brief Interpolates the Chain Chomp's up vector towards the target up direction
/// @param interpRate The interpolation factor
/// @details Updates @ref m_up by interpolating it towards @ref m_targetUp.
void ObjectWanwan::calcUp(f32 interpRate) {
    m_up = m_up.interpolate(interpRate, m_targetUp);
    if (m_up.squaredLength() > std::numeric_limits<f32>::epsilon()) {
        m_up.normalise2();
    } else {
        m_up = EGG::Vector3f::ey;
    }
}

/// @addr{0x806E87C8}
/// @brief Calculates a random target position for the Chain Chomp to lunge towards
/// @details The target position is calculated by rotating the vector defined by @ref
/// m_attackCenterX and @ref m_attackCenterZ by a random angle in the range [-@ref
/// m_attackArc, @ref m_attackArc]. Updates @ref m_targetDir to reflect the new @ref m_target.
void ObjectWanwan::calcRandomTarget() {
    f32 angle = System::RaceManager::Instance()->random().getF32(m_attackArc * 2.0f);
    EGG::Vector3f attackArcTarget = EGG::Vector3f(m_attackCenterX, 0.0f, m_attackCenterZ);
    EGG::Vector3f attackArcDir = attackArcTarget - m_anchor;
    attackArcDir.y = 0.0f;
    attackArcDir.normalise2();
    EGG::Vector3f attackTargetDir = RotateXZByYaw((angle - m_attackArc) * DEG2RAD, attackArcDir);
    m_target = m_anchor + attackTargetDir * m_attackDistance;
    m_targetDir = m_target - pos();
    m_targetDir.y = 0.0f;
    m_targetDir.normalise2();
}

/// @addr{0x806E9084}
/// @brief Clamps the Chain Chomp's position to the chain's length while attacking
/// @details If the Chain Chomp is not in the attack state, this function early returns. Otherwise,
/// it updates the Chain Chomp's transformation matrix based on its current @ref m_tangent and @ref
/// m_pitch and updates @ref m_chainAttachPos to reflect the Chain Chomp's current position and
/// orientation. Checks to see if the Chain Chomp's chain is taut and adjusts its position
/// accordingly (with an additional `35.0f` unit offset in the direction of the chain away from the
/// anchor), setting @ref m_chainTaut to `true` in the process.
void ObjectWanwan::calcAttackPos() {
    constexpr f32 SCALED_CHAIN_LENGTH = CHAIN_LENGTH * SCALE;
    constexpr f32 TAUT_POS_OFFSET = 35.0f;

    if (m_currentStateId != 1) {
        return;
    }

    calcMat();
    calcTransform();
    calcChainAttachPos(transform());

    EGG::Vector3f dir = m_chainAttachPos - m_anchor;
    f32 dist = dir.normalise();
    dist -= SCALED_CHAIN_LENGTH * static_cast<f32>(m_chainCount);

    if (dist > 0.0f || m_attackStill) {
        m_chainTaut = true;
        subPos(dir * dist);
        addPos(dir * TAUT_POS_OFFSET);
    }
}

/// @brief Calculates the world position of the chain attachment point on the Chain Chomp
/// @param mat The world transform matrix of the Chain Chomp
/// @details The attachment point is found by taking the Chain Chomp's position, subtracting a
/// neck offset of `250.0f` from the Chain Chomp's forward direction, subtracting an additional
/// offset of `140.0f` from the forward direction, and adding a vertical offset of `20.0f` to
/// the up direction. Saves the result to @ref m_chainAttachPos.
void ObjectWanwan::calcChainAttachPos(EGG::Matrix34f mat) {
    constexpr f32 NECK_OFFSET = 250.0f;
    constexpr f32 CHAIN_BACK_OFFSET = 140.0f;
    constexpr f32 CHAIN_UP_OFFSET = 20.0f;

    EGG::Vector3f pos = mat.base(3);
    pos -= mat.base(2) * NECK_OFFSET * scale().x;
    mat.setBase(3, pos);

    EGG::Vector3f backOffset = mat.base(2) * CHAIN_BACK_OFFSET * scale().x;
    EGG::Vector3f verticalOffset = mat.base(1) * CHAIN_UP_OFFSET * scale().x;
    m_chainAttachPos = pos - backOffset + verticalOffset;
}

/// @addr{0x806B38A8}
/// @brief Calculates the cross product of the XZ components of three vectors
/// @param v0 The first vector
/// @param v1 The second vector
/// @param v2 The third vector
/// @return The cross product of the XZ components of the three vectors
f32 ObjectWanwan::CrossXZ(const EGG::Vector3f &v0, const EGG::Vector3f &v1,
        const EGG::Vector3f &v2) {
    return (v2.x - v1.x) * (v0.z - v1.z) - (v0.x - v1.x) * (v2.z - v1.z);
}

/// @addr{0x806E8F4C}
/// @brief Samples a Hermite interpolation between two values with the number of samples
/// specified by the size of the destination span
/// @param start The starting value of the interpolation
/// @param end The ending value of the interpolation
/// @param startTangent The tangent at the starting value
/// @param endTangent The tangent at the ending value
/// @param dst The destination span to store the sampled values
void ObjectWanwan::SampleHermiteInterp(f32 start, f32 end, f32 startTangent, f32 endTangent,
        std::span<f32> dst) {
    dst.front() = start;
    dst.back() = end;

    f32 scalar = 1.0f / static_cast<f32>(dst.size() - 1);

    for (u8 i = 1; i < dst.size() - 1; ++i) {
        dst[i] = EGG::Mathf::Hermite(start, startTangent, end, endTangent,
                scalar * static_cast<f32>(i));
    }
}

} // namespace Kinoko::Field
