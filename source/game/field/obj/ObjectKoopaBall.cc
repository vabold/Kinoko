#include "ObjectKoopaBall.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/CollisionInfo.hh"
#include "game/field/RailInterpolator.hh"

#include "game/kart/Reaction.hh"

#include "game/render/AnmMgr.hh"
#include "game/render/DrawMdl.hh"

#include "game/system/ResourceManager.hh"

#include <abstract/g3d/ResFile.hh>

namespace Kinoko::Field {

/// @addr{0x80770384}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Initializes the fireball's velocity to zero.
ObjectKoopaBall::ObjectKoopaBall(const System::MapdataGeoObj &params) : ObjectCollidable(params) {
    m_vel = EGG::Vector3f::zero;
}

/// @addr{0x80771F70}
/// @brief Virtual destructor that destroys the associated bomb core draw model
ObjectKoopaBall::~ObjectKoopaBall() {
    EGG::egg_delete(m_bombCoreDrawMdl);
}

/// @addr{0x807703D0}
/// @copybrief ObjectBase::init()
/// @details Initializes the fireball's state to @ref State::Intangible. Initializes the cooldown
/// timer to `221` frames. Initializes the rail interpolator to the beginning of the rail, sets its
/// speed to @ref INITIAL_SPEED, and advances the fireball's position using the rail interpolator.
/// Caches the object's initial vertical position along the rail to @ref m_initPosY. Sets the
/// fireball's orientation based on the rail's tangent direction. Initializes @ref m_angleRad to
/// zero, @ref m_angSpeed to @ref INITIAL_ANGULAR_SPEED, and the vertical component of @ref m_vel to
/// @ref INITIAL_Y_SPEED. Constructs the @ref Render::DrawMdl for the bomb animation and caches the
/// animation's framecount to @ref m_animDuration. Initializes @ref m_explodeTimer to `-1`. Finally,
/// resizes the object's @ref BoxColUnit radius to @ref RADIUS_AABB.
void ObjectKoopaBall::init() {
    m_state = State::Intangible;
    initCooldownTimer();

    m_railInterpolator->init(0.0f, 0);
    m_railInterpolator->setCurrVel(INITIAL_SPEED);
    m_railInterpolator->calc();

    m_initPosY = m_railInterpolator->curPos().y;
    setPos(EGG::Vector3f(pos().x, m_initPosY, pos().z));

    EGG::Vector3f curTanDirNorm = m_railInterpolator->curTangentDir();
    curTanDirNorm.normalise();
    setMatrixFromOrthonormalBasisAndPos(curTanDirNorm);
    calcTransform();

    m_angleRad = 0.0f;
    m_angSpeed = INITIAL_ANGULAR_SPEED;
    m_vel.y = INITIAL_Y_SPEED;

    m_bombCoreDrawMdl = EGG::egg_new<Render::DrawMdl>();

    auto *resMgr = System::ResourceManager::Instance();
    const void *file = resMgr->getFile("bombCore.brres", nullptr, System::ArchiveId::Core);
    ASSERT(file);
    Abstract::g3d::ResFile resFile = Abstract::g3d::ResFile(file);

    m_bombCoreDrawMdl->linkAnims(0, &resFile, "bombCore", Render::AnmType::Chr);
    auto *anmMgr = m_bombCoreDrawMdl->anmMgr();
    anmMgr->playAnim(0.0f, 1.0f, 0);
    m_animDuration = anmMgr->activeAnim(Render::AnmType::Chr)->frameCount();
    m_explodeTimer = -1;

    resize(RADIUS_AABB, 0.0f);
}

/// @addr{0x80770ADC}
/// @copybrief ObjectBase::calc()
/// @details Calculates collision state-specific behavior, decrements @ref m_cooldownTimer, and
/// updates the X and Z components of @ref m_vel based on the rail interpolator.
void ObjectKoopaBall::calc() {
    switch (m_state) {
    case State::Tangible:
        calcTangible();
        break;
    case State::Intangible:
        calcIntangible();
        break;
    case State::Exploding:
        calcExploding();
        break;
    default:
        break;
    }

    --m_cooldownTimer;

    EGG::Vector3f railVel = m_railInterpolator->curTangentDir() * m_railInterpolator->getCurrVel();
    m_vel.x = railVel.x;
    m_vel.z = railVel.z;
}

/// @addr{0x80771F68}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectKoopaBall::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x80771BF4}
/// @copybrief ObjectCollidable::onCollision()
/// @param reactionOnKart The reaction that should be applied to the kart upon collision
/// @return @ref Kart::Reaction::ExplosionLoseItem if the fireball is exploding, otherwise @ref
/// Kart::Reaction::LaunchSpin.
/// @details Throws the player upwards and removes items if the fireball is exploding, otherwise
/// the player will flip sideways without losing their item.
Kart::Reaction ObjectKoopaBall::onCollision(Kart::KartObject * /*kartObj*/,
        Kart::Reaction reactionOnKart, Kart::Reaction /*reactionOnObj*/,
        EGG::Vector3f & /*hitDepth*/) {
    return m_explodeTimer > 0 ? Kart::Reaction::ExplosionLoseItem : reactionOnKart;
}

/// @addr{0x80771F60}
/// @copybrief ObjectCollidable::getCollisionTranslation()
/// @return Returns the current velocity of the fireball.
/// @details Extends the collision translation to include the velocity of the fireball, so
/// collision detection happens from further away when the ball is moving towards the player.
const EGG::Vector3f &ObjectKoopaBall::getCollisionTranslation() const {
    return m_vel;
}

/// @addr{0x8077079C}
/// @brief Initializes the cooldown timer to `221` frames
void ObjectKoopaBall::initCooldownTimer() {
    constexpr u32 START_COOLDOWN = 221;
    m_cooldownTimer = START_COOLDOWN;
}

/// @addr{0x80770F4C}
/// @brief Runs every frame that the fireball can collide with karts
/// @details Updates the fireball's position based on the rail interpolator. If the fireball reached
/// the end of the rail, the fireball will begin to explode. Checks for floor collision, updates the
/// rotation, then sets the transformation matrix.
void ObjectKoopaBall::calcTangible() {
    constexpr f32 GRAVITY = 2.0f;

    auto railStatus = m_railInterpolator->calc();

    switch (railStatus) {
    case RailInterpolator::Status::SegmentEnd:
        calcSlowdown();
        break;
    case RailInterpolator::Status::ChangingDirection: {
        m_state = State::Exploding;
        setExplosionScale();
        m_explodeTimer = m_animDuration;
    } break;
    default:
        break;
    }

    m_vel.y = m_vel.y - GRAVITY;
    const auto &railPos = m_railInterpolator->curPos();
    setPos(EGG::Vector3f(railPos.x, m_vel.y + pos().y, railPos.z));

    checkSphereFull();
    calcRot();
}

/// @addr{0x80771324}
/// @brief Runs every frame that the fireball is exploding
/// @details While more than `30` frames of the explosion remain, the scale of the fireball will
/// increase. Once there are only `20` frames remaining for the explosion, then collision is
/// disabled. Once the explosion has finished, the scale, position, and collision are reset and @ref
/// m_state is set to @ref State::Intangible.
void ObjectKoopaBall::calcExploding() {
    constexpr s32 EXPLODE_COLLISION_DURATION = 20;
    constexpr s32 EXPLOSION_EXPAND_FRAME = 30;

    if (m_explodeTimer > EXPLOSION_EXPAND_FRAME) {
        m_curScale += SCALE_DELTA;
        setScale(m_curScale);
    }

    if (m_explodeTimer == EXPLODE_COLLISION_DURATION) {
        disableCollision();
    }

    if (--m_explodeTimer == 0) {
        resetScale();
        m_railInterpolator->init(0.0f, 0);
        const auto &railPos = m_railInterpolator->curPos();
        setPos(EGG::Vector3f(railPos.x, m_initPosY, railPos.z));
        enableCollision();
        m_state = State::Intangible;
    }
}

/// @addr{0x80771248}
/// @brief Runs every frame that the fireball is intangible
/// @details Once the cooldown timer has expired, the fireball will become tangible.
/// Reinitializes its rail speed, angular speed, vertical speed, and cooldown timer.
void ObjectKoopaBall::calcIntangible() {
    constexpr s32 COOLDOWN_FRAMES = 210;

    if (m_cooldownTimer >= 0) {
        return;
    }

    m_state = State::Tangible;
    m_railInterpolator->setCurrVel(INITIAL_SPEED);
    m_angSpeed = INITIAL_ANGULAR_SPEED;
    m_vel.y = INITIAL_Y_SPEED;
    m_cooldownTimer = COOLDOWN_FRAMES;
}

/// @addr{0x8077151C}
/// @brief Updates the fireball's angular rotation and transformation matrix
/// @details Applies @ref m_angSpeed to the fireball's rotation around the x-axis. Updates the
/// transformation matrix accordingly.
void ObjectKoopaBall::calcRot() {
    m_angleRad += -m_angSpeed * DEG2RAD;

    EGG::Matrix34f mat = EGG::Matrix34f::ident;
    mat.makeR(EGG::Vector3f(m_angleRad, 0.0f, 0.0f));
    mat = transform().multiplyTo(mat);
    mat.setBase(3, pos());
    setTransform(mat);
}

/// @addr{0x807719A0}
/// @brief Slows down the fireball's rail speed once reaching the end of a rail segment
/// @details This does not occur in the base game, but we implement it in Kinoko for
/// completeness.
void ObjectKoopaBall::calcSlowdown() {
    constexpr f32 VERTICAL_SPEED = 60.0f;

    m_vel.y = VERTICAL_SPEED;
    m_railInterpolator->setCurrVel(INITIAL_SPEED / INITIAL_ANGULAR_SPEED);
}

/// @addr{0x80771B68}
/// @brief Runs once when the fireball starts exploding
/// @details Sets the fireball's scale to @ref INITIAL_EXPLOSION_SCALE.
void ObjectKoopaBall::setExplosionScale() {
    m_curScale = INITIAL_EXPLOSION_SCALE;
    setScale(INITIAL_EXPLOSION_SCALE);
}

/// @addr{0x80771BCC}
/// @brief Resets the scale of the fireball to `1.01f`
void ObjectKoopaBall::resetScale() {
    constexpr f32 INIT_FACTOR = 1.01f;

    m_curScale = INIT_FACTOR;
    setScale(INIT_FACTOR);
}

/// @addr{0x80771624}
/// @brief Checks if the fireball is colliding with the floor and bounces it upwards if so
/// @details If a floor collision occurred, causes an inelastic collision by multiplying the
/// vertical velocity by `-0.4f`. Clamps the fireball's @ref m_angSpeed to minimum of `10.0f`.
/// Finally, updates the fireball's position to push it out of the floor collision.
void ObjectKoopaBall::checkSphereFull() {
    constexpr EGG::Vector3f POS_OFFSET = EGG::Vector3f(0.0f, -900.0f, 0.0f);
    constexpr f32 RADIUS = 100.0f;
    constexpr f32 BOUNCE_FACTOR = -0.4f;
    constexpr f32 MIN_ANG_SPEED = 10.0f;

    CollisionInfo info;
    EGG::Vector3f colPos = pos() + POS_OFFSET;

    if (CollisionDirector::Instance()->checkSphereFull(RADIUS, colPos, EGG::Vector3f::inf,
                KCL_TYPE_FLOOR, &info, nullptr, 0)) {
        m_vel.y *= BOUNCE_FACTOR;
        m_angSpeed = std::max(MIN_ANG_SPEED, m_angSpeed);
        addPos(info.tangentOff);
    }
}

} // namespace Kinoko::Field
