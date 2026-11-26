#include "ObjectChoropu.hh"

#include "game/field/ObjectCollisionSphere.hh"
#include "game/field/ObjectDirector.hh"
#include "game/field/Rail.hh"
#include "game/field/RailInterpolator.hh"
#include "game/field/RailManager.hh"

#include "game/kart/Reaction.hh"

#include "game/system/RaceManager.hh"
#include "game/system/map/MapdataGeoObj.hh"

#include <cstring>

namespace Kinoko::Field {

/// @addr{0x806B96A0}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Initializes @ref m_startFrameOffset based off param setting 2, @ref m_idleDuration
/// based off param setting 1, and @ref m_isStationary based off the object's name. If the mole
/// moves along a rail, precomputes all floor normals along the rail to align the mole's orientation
/// when peeking above the dirt hole. If the mole is not stationary, creates the associated @ref
/// ObjectChoropuGround instances for the dirt trail and computes the ground height. Finally,
/// creates the @ref ObjectChoropuHoll instance for the mole's hole.
ObjectChoropu::ObjectChoropu(const System::MapdataGeoObj &params)
    : ObjectCollidable(params),
      StateManager(this, STATE_ENTRIES),
      m_startFrameOffset(static_cast<s16>(params.setting(1))),
      m_idleDuration(params.setting(0)),
      m_isStationary(strcmp(getName(), "choropu") != 0) {
    constexpr f32 MAX_SPEED = 20.0f;

    s16 railIdx = params.pathId();
    if (railIdx != -1) {
        auto *rail = RailManager::Instance()->rail(railIdx);
        rail->checkSphereFull();
    }

    // If the mole moves around, then we need to create the dirt trail.
    if (!m_isStationary) {
        const auto &flowTable = ObjectDirector::Instance()->flowTable();
        const auto *collisionSet =
                flowTable.set(flowTable.slot(flowTable.getIdFromName("choropu_ground")));
        ASSERT(collisionSet);

        s16 height = parse<s16>(collisionSet->params.cylinder.height);
        size_t groundCount = static_cast<size_t>(MAX_GROUND_LEN / EGG::Mathf::abs(height * 2)) + 1;
        m_groundObjs = owning_span<ObjectChoropuGround *>(groundCount);

        for (auto *&obj : m_groundObjs) {
            obj = EGG::egg_new<ObjectChoropuGround>(pos(), rot(), scale());
            obj->load();
            obj->resize(RADIUS, MAX_SPEED);
        }

        m_groundHeight = m_groundObjs.front()->height();
    }

    m_objHoll = EGG::egg_new<ObjectChoropuHoll>(params);
    m_objHoll->load();
}

/// @addr{0x806B9B8C}
/// @brief Default virtual destructor
ObjectChoropu::~ObjectChoropu() = default;

/// @addr{0x806B9BF8}
/// @copybrief ObjectBase::init()
/// @details Initializes the mole's state based on whether it is stationary or moves along a rail.
/// If the mole is stationary, it disables collision initially, scales the mole based off its hole's
/// scale, and sets up the transform matrix. If the mole moves along a rail, it initializes the rail
/// interpolator, sets the initial position and orientation along the rail, and disables collision
/// for both the mole and its hole. In either case, the mole starts in the digging state.
void ObjectChoropu::init() {
    if (m_isStationary) {
        disableCollision();

        m_objHoll->setScale(EGG::Vector3f(1.0f, m_objHoll->scale().y, 1.0f));
        m_nextStateId = 0;
        m_groundLength = 0.0f;

        calcTransform();
        m_initRt = transform();
    } else {
        if (m_mapObj->pathId() == -1) {
            return;
        }

        m_railInterpolator->init(0.0f, 0);
        m_railInterpolator->setPerPointVelocities(true);

        m_railPos = m_railInterpolator->curPos();
        setPos(m_railPos);

        disableCollision();
        m_objHoll->disableCollision();

        m_nextStateId = 0;
    }
}

/// @addr{0x806B9E60}
/// @copybrief ObjectBase::calc()
/// @details If the mole hasn't spawned yet (which is the case when the race duration is below @ref
/// m_startFrameOffset plus `300`), then returns early. If the mole moves along a rail, then stores
/// the rail's current position in @ref m_railPos. Evaluates the mole's state machine. Finally,
/// resets the X and Z component of the hole scale to `1.0f`.
void ObjectChoropu::calc() {
    constexpr u32 START_DELAY = 300;

    // Nothing to do if the mole hasn't spawned yet
    u32 t = System::RaceManager::Instance()->timer();
    if (t < m_startFrameOffset + START_DELAY) {
        return;
    }

    if (!m_isStationary) {
        if (m_mapObj->pathId() == -1) {
            return;
        }

        // Kinoko-specific: the base game computes RailOrthonormalBasis() here, but only its
        // translation is ever used, so we skip computing the rotation.
        m_railPos = m_railInterpolator->curPos();
    }

    StateManager::calc();

    m_objHoll->setScale(EGG::Vector3f(1.0f, m_objHoll->scale().y, 1.0f));
}

/// @addr{0x806BBE34}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectChoropu::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x806BA144}
/// @copybrief ObjectCollidable::onCollision
/// @param kartObj The kart object that collided with the mole (unused)
/// @param reactionOnKart The reaction that should be applied to the kart upon collision
/// @param reactionOnObj The reaction that should be applied to the mole upon collision (unused)
/// @param hitDepth The depth of the collision (unused)
/// @return @ref Kart::Reaction::SmallBump if the mole is poking its head out, otherwise @ref
/// Kart::Reaction::SmallLaunch.
Kart::Reaction ObjectChoropu::onCollision(Kart::KartObject * /*kartObj*/,
        Kart::Reaction reactionOnKart, Kart::Reaction /*reactionOnObj*/,
        EGG::Vector3f & /*hitDepth*/) {
    return m_currentStateId == 1 ? Kart::Reaction::SmallBump : reactionOnKart;
}

/// @addr{0x806BA6D8}
/// @brief Runs once when the mole lands back in its hole after a jump
/// @details If the mole is stationary, then this just disables collision. If the mole moves along a
/// rail, then it re-enables collision for the trailing dirt objects while disabling collision for
/// both the mole and the hole and resetting the trailing dirt length to zero.
void ObjectChoropu::enterDigging() {
    if (m_isStationary) {
        disableCollision();
    } else {
        for (auto *&obj : m_groundObjs) {
            obj->enableCollision();
        }

        disableCollision();
        m_objHoll->disableCollision();
        m_groundLength = 0.0f;
    }
}

/// @addr{0x806BABEC}
/// @brief Runs once when the mole peeks out of its hole before jumping
/// @details If the mole is stationary, then it simply sets its position based on the
/// stored transform and enables collision. If the mole moves along a rail, then it sets its
/// position based on the rail and updates the hole's transform and enable's the hole's collision.
void ObjectChoropu::enterPeeking() {
    if (m_isStationary) {
        setPos(m_initRt.base(3));
        setRot(EGG::Vector3f(rot().x, rot().y, 0.0f));

        enableCollision();
    } else {
        setPos(m_railPos);
        setRot(EGG::Vector3f(rot().x, 0.0f, 0.0f));
        enableCollision();

        const auto &curTanDir = m_railInterpolator->curTangentDir();
        s16 curPointIdx = m_railInterpolator->curPointIdx();
        EGG::Matrix34f mat;
        SetRotTangentHorizontal(mat, m_railInterpolator->floorNrm(curPointIdx), curTanDir);
        mat.setBase(3, m_railInterpolator->curPos());

        m_objHoll->setTransform(mat);
        m_objHoll->enableCollision();
    }
}

/// @addr{0x806BB39C}
/// @brief Runs once when the mole jumps out of its hole
/// @details Enables collision for the mole. Sets its position based on whether it
/// is stationary or on a rail, and sets its roll to zero.
void ObjectChoropu::enterJumping() {
    enableCollision();

    setPos(m_isStationary ? m_initRt.base(3) : m_railPos);
    setRot(EGG::Vector3f(rot().x, rot().y, 0.0f));
}

/// @addr{0x806BA7FC}
/// @brief Runs once every frame while the mole is neither peeking nor jumping
/// @details If the mole is stationary, then it simply checks if the state duration has passed and
/// advances to the peeking state. If the mole moves along a rail, then it updates its position
/// along the rail. If the mole has reached the end of a rail segment and the new rail node has
/// setting 2 enabled, then the mole transitions to the peeking state. Otherwise, updates the state
/// of the dirt trail. If the next point is a peek point and the distance remaining in the current
/// rail segment is shorter than `250.0f` units, then it skips updating the dirt trail.
void ObjectChoropu::calcDigging() {
    constexpr f32 DIRT_CALC_THRESHOLD = 250.0f;

    if (m_isStationary) {
        if (m_currentFrame > m_idleDuration) {
            m_nextStateId = 1;
        }

        return;
    }

    setPos(m_railInterpolator->curPos());

    if (m_railInterpolator->calc() == RailInterpolator::Status::SegmentEnd) {
        if (m_railInterpolator->curPoint().setting[1] == 1) {
            m_nextStateId = 1;
        } else {
            calcGround();
        }
    } else {
        bool skipGroundCalc = false;

        if (m_railInterpolator->nextPoint().setting[1] == 1) {
            f32 invT = 1.0f - m_railInterpolator->segmentT();
            if (invT * m_railInterpolator->getCurrSegmentLength() < DIRT_CALC_THRESHOLD) {
                skipGroundCalc = true;
            }
        }

        if (!skipGroundCalc) {
            calcGround();
        }
    }
}

/// @addr{0x806BB144}
/// @brief Runs once every frame while the mole is peeking out of its hole and before it jumps out
/// @details If the mole is not stationary, then it updates @ref m_groundLength to reflect the
/// distance traveled along the rail this frame and recalculates the positions of the ground objects
/// accordingly. If the mole has been in this state for more than 100 frames, advances to the
/// jumping state. Finally, disables collision if the mole has finished peeking out of its hole.
void ObjectChoropu::calcPeeking() {
    constexpr s16 PEEK_DURATION = 40;
    constexpr s16 STATE_DURATION = 100;

    if (!m_isStationary) {
        m_groundLength = std::max(0.0f, m_groundLength - m_railInterpolator->speed());

        calcGroundObjs();
    }

    if (m_currentFrame > STATE_DURATION) {
        m_nextStateId = 3;
    }

    if (m_currentFrame > PEEK_DURATION) {
        disableCollision();
    }
}

/// @addr{0x806BB5F0}
/// @brief Runs once every frame while the mole is jumping out of its hole
/// @details If the mole is not stationary, then it updates @ref m_groundLength to reflect the
/// distance traveled along the rail this frame and recalculates the positions of the ground objects
/// accordingly. If the mole has landed back in its hole, it sets @ref m_nextStateId to 0, advancing
/// it to the digging state. Finally, it sets the mole's height to reflect its current jump height.
void ObjectChoropu::calcJumping() {
    if (!m_isStationary) {
        m_groundLength = std::max(0.0f, m_groundLength - m_railInterpolator->speed());
        calcGroundObjs();
    }

    f32 posY = calcJumpHeight();

    if (posY < 0.0f) {
        m_nextStateId = 0;
    }

    posY += (m_isStationary ? m_initRt.base(3).y : m_railInterpolator->curPos().y);
    setPos(EGG::Vector3f(pos().x, posY, pos().z));
}

/// @addr{0x806BBA7C}
/// @brief Calculates the total length of the dirt trail behind the monty moles on MMM
/// @details The ground length increases based off how fast the mole is traveling, which is
/// represented by the rail interpolator's current speed. If @ref m_groundLength exceeds @ref
/// MAX_GROUND_LEN, it is clamped to one unit below the maximum. Finally, calls @ref
/// calcGroundObjs() to update the position of the dirt trail objects.
void ObjectChoropu::calcGround() {
    m_groundLength += m_railInterpolator->getCurrSpeed();
    if (m_groundLength > MAX_GROUND_LEN) {
        m_groundLength = MAX_GROUND_LEN - 1.0f;
    }

    calcGroundObjs();
}

/// @addr{0x806BB840}
/// @brief Calculates the position and orientation of the dirt trail behind the monty moles on MMM
/// @details Dirt objects that lie beyond the dirt trail length have their collision disabled. As
/// the monty mole moves and the dirt trail length increases, the dirt objects are repositioned and
/// re-scaled and additional dirt objects will have their collision enabled.
void ObjectChoropu::calcGroundObjs() {
    size_t idx =
            std::min(static_cast<size_t>(m_groundLength / m_groundHeight) + 1, m_groundObjs.size());

    for (auto *&obj : m_groundObjs) {
        obj->enableCollision();
    }

    for (size_t i = idx; i < m_groundObjs.size(); ++i) {
        m_groundObjs[i]->disableCollision();
    }

    if (m_groundLength > RADIUS) {
        f32 height = std::min(m_groundHeight, m_groundLength) - RADIUS;
        m_groundObjs[0]->calcPosAndMat(height, calcInterpolatedPose(RADIUS + 0.5f * height));
    }

    for (size_t i = 1; i < idx - 1; ++i) {
        f32 height = 0.5f * m_groundHeight + m_groundHeight * static_cast<f32>(i);
        m_groundObjs[i]->calcPosAndMat(m_groundHeight, calcInterpolatedPose(height));
    }

    f32 height = m_groundLength - m_groundHeight * static_cast<f32>(idx - 1);
    EGG::Matrix34f mat =
            calcInterpolatedPose(0.5f * height + m_groundHeight * static_cast<f32>(idx - 1));
    m_groundObjs[idx - 1]->calcPosAndMat(height, mat);
}

/// @addr{0x806B46F8}
/// @brief Calculates position and rotation along the bezier curve of the rail at a given t
/// @param t The parameter along the bezier curve of the rail at which to calculate the pose
/// of the mole.
/// @return The interpolated rotation and translation matrix at the given `t` along the
/// bezier curve.
EGG::Matrix34f ObjectChoropu::calcInterpolatedPose(f32 t) const {
    EGG::Vector3f curDir;
    EGG::Vector3f curTanDir;
    m_railInterpolator->evalPositionAndTangentBehind(t, curDir, curTanDir);
    EGG::Matrix34f mat = OrthonormalBasis(curTanDir);
    mat.setBase(3, curDir);
    return mat;
}

/// @addr{0x806BBB14}
/// @brief Calculates the current height offset of the mole in its parabolic jump curve
/// @return The height offset of the mole in its parabolic jump curve.
/// @details Follows a parabolic trajectory defined by
/// \f$ y = -1.35t^2 + 65.0t \f$
/// where \f$t\f$ is the current frame of the jump.
f32 ObjectChoropu::calcJumpHeight() const {
    constexpr f32 JUMP_LINEAR_COEFFICIENT = 65.0f;
    constexpr f32 JUMP_QUADRATIC_COEFFICIENT = 2.7f;

    return JUMP_LINEAR_COEFFICIENT * static_cast<f32>(m_currentFrame) -
            static_cast<f32>(m_currentFrame) * 0.5f * JUMP_QUADRATIC_COEFFICIENT *
            static_cast<f32>(m_currentFrame);
}

/// @addr{0x806B8F94}
/// @brief Constructor
/// @param pos The initial position of the object
/// @param rot The initial rotation of the object
/// @param scale The initial scale of the object
/// @details Initializes the height of the object to be twice the absolute value of the cylinder
/// height specified in the collision set.
ObjectChoropuGround::ObjectChoropuGround(const EGG::Vector3f &pos, const EGG::Vector3f &rot,
        const EGG::Vector3f &scale)
    : ObjectCollidable("choropu_ground", pos, rot, scale) {
    const auto &flowTable = ObjectDirector::Instance()->flowTable();
    const auto *collisionSet =
            flowTable.set(flowTable.slot(flowTable.getIdFromName("choropu_ground")));
    ASSERT(collisionSet);

    s16 height = parse<s16>(collisionSet->params.cylinder.height);
    m_height = 2.0f * EGG::Mathf::abs(static_cast<f32>(height));
}

/// @addr{0x806BBE6C}
/// @brief Default virtual destructor
ObjectChoropuGround::~ObjectChoropuGround() = default;

/// @addr{0x806BBEB0}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectChoropuGround::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x806B9164}
/// @copybrief ObjectCollidable::onCollision
/// @param kartObj The kart object that collided with the ground segment (unused)
/// @param reactionOnKart The reaction that should be applied to the kart upon collision
/// @param reactionOnObj The reaction that should be applied to the ground segment upon collision
/// (unused)
/// @param hitDepth The depth of the collision between the kart and the object
/// @return @ref Kart::Reaction::Wall
/// @details Also sets the hit depth to zero.
Kart::Reaction ObjectChoropuGround::onCollision(Kart::KartObject * /*kartObj*/,
        Kart::Reaction reactionOnKart, Kart::Reaction /*reactionOnObj*/, EGG::Vector3f &hitDepth) {
    hitDepth.setZero();
    return reactionOnKart;
}

/// @addr{0x806B9274}
/// @brief Sets the ground object's transformation matrix based off of the provided pose
/// @param height The height to scale the ground segment to
/// @param mat The transformation matrix to base the ground segment's position and
/// orientation on
void ObjectChoropuGround::calcPosAndMat(f32 height, const EGG::Matrix34f &mat) {
    EGG::Matrix34f matTemp;
    SetRotTangentHorizontal(matTemp, mat.base(2), EGG::Vector3f::ey);
    matTemp.setBase(1, matTemp.base(1) * (height / m_height));
    matTemp.setBase(3, mat.base(3));
    setTransform(matTemp);
}

/// @addr{0x806B93CC}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
ObjectChoropuHoll::ObjectChoropuHoll(const System::MapdataGeoObj &params)
    : ObjectCollidable(params) {}

/// @addr{0x806BBE6C}
/// @brief Default virtual destructor
ObjectChoropuHoll::~ObjectChoropuHoll() = default;

/// @addr{0x806B94A0}
/// @copybrief ObjectBase::init()
/// @details Resizes the hole object's @ref BoxColUnit to have a radius of @ref RADIUS.
void ObjectChoropuHoll::init() {
    resize(RADIUS, 0.0f);
}

/// @addr{0x806B9428}
/// @copybrief ObjectBase::createCollision()
/// @details Create a stationary collision sphere with radius @ref RADIUS.
void ObjectChoropuHoll::createCollision() {
    m_collision = EGG::egg_new<ObjectCollisionSphere>(RADIUS, EGG::Vector3f::zero);
}

/// @addr{0x806BBE40}
/// @copybrief ObjectBase::loadRail()
/// @details No-op because the hole is managed by the @ref ObjectChoropu, not a rail.
void ObjectChoropuHoll::loadRail() {}

/// @addr{0x806BBE4C}
/// @copybrief ObjectBase::getName()
/// @return The name of the object, `holl`
const char *ObjectChoropuHoll::getName() const {
    return "holl";
}

/// @addr{0x806BBE64}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectChoropuHoll::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x806BBE58}
/// @copybrief ObjectBase::getKclName()
/// @return The name of the hole object, `holl`
const char *ObjectChoropuHoll::getKclName() const {
    return "holl";
}

/// @addr{0x806B9594}
/// @copybrief ObjectCollidable::onCollision
/// @param kartObj The kart object that collided with the hole (unused)
/// @param reactionOnKart The reaction that should be applied to the kart upon collision (unused)
/// @param reactionOnObj The reaction that should be applied to the hole upon collision (unused)
/// @param hitDepth The depth of the collision between the kart and the object
/// @return @ref Kart::Reaction::Wall
/// @details Also sets the hit depth to zero.
Kart::Reaction ObjectChoropuHoll::onCollision(Kart::KartObject * /*kartObj*/,
        Kart::Reaction /*reactionOnKart*/, Kart::Reaction /*reactionOnObj*/,
        EGG::Vector3f &hitDepth) {
    hitDepth.setZero();
    return Kart::Reaction::Wall;
}

} // namespace Kinoko::Field
