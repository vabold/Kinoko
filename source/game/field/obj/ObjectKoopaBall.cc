#include "ObjectKoopaBall.hh"

#include "game/field/CollisionDirector.hh"

#include "game/kart/KartCollide.hh"

namespace Field {

/// @addr{0x80770384}
ObjectKoopaBall::ObjectKoopaBall(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_vel(EGG::Vector3f::zero) {}

/// @addr{0x80771F70}
ObjectKoopaBall::~ObjectKoopaBall() {
    delete m_bombCoreDrawMdl;
}

/// @addr{0x807703D0}
void ObjectKoopaBall::init() {
    constexpr u32 START_COOLDOWN = 221;

    m_state = State::Intangible;
    m_cooldownTimer = START_COOLDOWN;

    m_railInterpolator->init(0.0f, 0);
    m_railInterpolator->setCurrVel(INITIAL_VELOCITY);
    m_railInterpolator->calc();

    m_initPosY = m_railInterpolator->curPos().y;
    setPos(EGG::Vector3f(pos().x, m_initPosY, pos().z));

    EGG::Vector3f curTanDirNorm = m_railInterpolator->curTangentDir();
    curTanDirNorm.normalise();
    setMatrixFromOrthonormalBasisAndPos(curTanDirNorm);
    calcTransform();

    m_angleRad = 0.0f;
    m_angSpeed = INITIAL_ANGULAR_SPEED;
    m_vel.y = INITIAL_Y_VEL;

    m_bombCoreDrawMdl = new Render::DrawMdl;

    auto *resMgr = System::ResourceManager::Instance();
    const void *file = resMgr->getFile("bombCore.brres", nullptr, System::ArchiveId::Core);
    ASSERT(file);
    Abstract::g3d::ResFile resFile = Abstract::g3d::ResFile(file);

    m_bombCoreDrawMdl->linkAnims(0, &resFile, "bombCore", Render::AnmType::Chr);
    auto *anmMgr = m_bombCoreDrawMdl->anmMgr();
    anmMgr->playAnim(0.0f, 1.0f, 0);
    m_animFramecount = anmMgr->activeAnim(Render::AnmType::Chr)->frameCount();
    m_explodeTimer = -1;

    EGG::Matrix34f mat;
    mat.makeR(rot());

    resize(RADIUS_AABB, 0.0f);
}

/// @addr{0x80770ADC}
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

/// @addr{0x80771BF4}
Kart::Reaction ObjectKoopaBall::onCollision(Kart::KartObject * /*kartObj*/,
        Kart::Reaction reactionOnKart, Kart::Reaction /*reactionOnObj*/,
        EGG::Vector3f & /*hitDepth*/) {
    return m_explodeTimer > 0 ? Kart::Reaction::ExplosionLoseItem : reactionOnKart;
}

/// @addr{0x80770F4C}
void ObjectKoopaBall::calcTangible() {
    constexpr f32 END_VELOCITY = 60.0f;
    constexpr f32 GRAVITY = 2.0f;

    auto railStatus = m_railInterpolator->calc();

    switch (railStatus) {
    case RailInterpolator::Status::SegmentEnd:
        m_vel.y = END_VELOCITY;
        m_railInterpolator->setCurrVel(INITIAL_VELOCITY / INITIAL_ANGULAR_SPEED);
        break;
    case RailInterpolator::Status::ChangingDirection: {
        m_state = State::Exploding;
        m_curScale = SCALE_INITIAL;
        setScale(SCALE_INITIAL);
        m_explodeTimer = m_animFramecount;
    } break;
    default:
        break;
    }

    m_vel.y = m_vel.y - GRAVITY;
    const auto &railPos = m_railInterpolator->curPos();
    setPos(EGG::Vector3f(railPos.x, m_vel.y + pos().y, railPos.z));

    checkSphereFull();

    m_angleRad += -m_angSpeed * DEG2RAD;

    EGG::Matrix34f mat = EGG::Matrix34f::ident;
    mat.makeR(EGG::Vector3f(m_angleRad, 0.0f, 0.0f));
    mat = transform().multiplyTo(mat);
    mat.setBase(3, pos());
    setTransform(mat);
}

/// @addr{0x80771324}
void ObjectKoopaBall::calcExploding() {
    constexpr s32 EXPLODE_COLLISION_DURATION = 20;
    constexpr s32 EXPLOSION_EXPAND_FRAME = 30;
    constexpr f32 INIT_FACTOR = 1.01f;
    constexpr EGG::Vector3f INIT_SCALE = EGG::Vector3f(INIT_FACTOR, INIT_FACTOR, INIT_FACTOR);

    if (m_explodeTimer > EXPLOSION_EXPAND_FRAME) {
        m_curScale += SCALE_DELTA;
        setScale(m_curScale);
    }

    if (m_explodeTimer == EXPLODE_COLLISION_DURATION) {
        disableCollision();
    }

    if (--m_explodeTimer == 0) {
        m_curScale = INIT_FACTOR;
        setScale(INIT_SCALE);
        m_railInterpolator->init(0.0f, 0);
        const auto &railPos = m_railInterpolator->curPos();
        setPos(EGG::Vector3f(railPos.x, m_initPosY, railPos.z));
        enableCollision();
        m_state = State::Intangible;
    }
}

/// @addr{0x80771248}
void ObjectKoopaBall::calcIntangible() {
    constexpr s32 COOLDOWN_FRAMES = 210;

    if (m_cooldownTimer >= 0) {
        return;
    }

    m_state = State::Tangible;
    m_railInterpolator->setCurrVel(INITIAL_VELOCITY);
    m_angSpeed = INITIAL_ANGULAR_SPEED;
    m_vel.y = INITIAL_Y_VEL;
    m_cooldownTimer = COOLDOWN_FRAMES;
}

/// @addr{0x80771624}
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

} // namespace Field
