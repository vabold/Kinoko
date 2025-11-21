#include "ObjectKoopaBall.hh"

#include "game/field/CollisionDirector.hh"

#include "game/kart/KartCollide.hh"

#include <abstract/g3d/ResFile.hh>

namespace Field {

/// @addr{0x80770384}
ObjectKoopaBall::ObjectKoopaBall(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_vel(EGG::Vector3f::zero) {}

/// @addr{0x80771F70}
ObjectKoopaBall::~ObjectKoopaBall() = default;

/// @addr{0x807703D0}
void ObjectKoopaBall::init() {
    constexpr u32 START_COOLDOWN = 221;
    constexpr const char *RES_FILENAME = "bombCore.brres";

    m_state = State::Intangible;
    m_cooldownTimer = START_COOLDOWN;

    m_railInterpolator->init(0.0f, 0);
    m_railInterpolator->setCurrVel(INITIAL_VELOCITY);
    m_railInterpolator->calc();

    m_startYpos = m_railInterpolator->curPos().y;
    m_flags.setBit(eFlags::Position);
    m_pos.y = m_startYpos;

    EGG::Vector3f curTanDirNorm = m_railInterpolator->curTangentDir();
    curTanDirNorm.normalise();
    setMatrixFromOrthonormalBasisAndPos(curTanDirNorm);
    calcTransform();
    m_curTransform = m_transform;

    m_angleRad = 0.0f;
    m_angSpeed = INITIAL_ANGULAR_SPEED;
    m_vel.y = INITIAL_Y_VEL;

    m_bombCoreDrawMdl = new Render::DrawMdl;

    Abstract::g3d::ResFile *resFile = nullptr;
    auto *resMgr = System::ResourceManager::Instance();
    const void *file = resMgr->getFile(RES_FILENAME, nullptr, System::ArchiveId::Core);
    if (file) {
        resFile = new Abstract::g3d::ResFile(file);
    }

    ASSERT(resFile);

    m_bombCoreDrawMdl->linkAnims(0, resFile, "bombCore", Render::AnmType::Chr);
    auto *anmMgr = m_bombCoreDrawMdl->anmMgr();
    anmMgr->playAnim(0.0f, 1.0f, 0);
    m_animFramecount = anmMgr->activeAnim(Render::AnmType::Chr)->frameCount();
    m_explodeTimer = -1;

    EGG::Matrix34f mat;
    mat.makeR(m_rot);
    m_curRot = mat.base(2);
    m_curRot.normalise();

    resize(870.0f * 2.0f, 0.0f);
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

    EGG::Vector3f scaledTangent =
            m_railInterpolator->curTangentDir() * m_railInterpolator->getCurrVel();
    m_vel.x = scaledTangent.x;
    m_vel.z = scaledTangent.z;
}

/// @addr{0x80771BF4}
Kart::Reaction ObjectKoopaBall::onCollision(Kart::KartObject * /*kartObj*/,
        Kart::Reaction reactionOnKart, Kart::Reaction /*reactionOnObj*/,
        EGG::Vector3f & /*hitDepth*/) {
    if (m_explodeTimer > 0) {
        return Kart::Reaction::ExplosionLoseItem;
    }

    return reactionOnKart;
}

/// @addr{0x80770F4C}
void ObjectKoopaBall::calcTangible() {
    auto railStatus = m_railInterpolator->calc();

    switch (railStatus) {
    case RailInterpolator::Status::SegmentEnd:
        m_vel.y = 60.0f;
        m_railInterpolator->setCurrVel(INITIAL_VELOCITY / INITIAL_ANGULAR_SPEED);
        break;
    case RailInterpolator::Status::ChangingDirection: {
        m_state = State::Exploding;
        m_flags.setBit(eFlags::Scale);
        m_curScale = SCALE_INITIAL;
        m_scale = EGG::Vector3f(SCALE_INITIAL, SCALE_INITIAL, SCALE_INITIAL);
        m_explodeTimer = m_animFramecount;
    } break;
    default:
        break;
    }

    m_flags.setBit(eFlags::Position);
    const auto &railPos = m_railInterpolator->curPos();
    m_pos.x = railPos.x;
    m_pos.z = railPos.z;

    m_vel.y = m_vel.y - 2.0f;
    m_pos.y = m_vel.y + m_pos.y;

    checkSphereFull();

    m_angleRad += -m_angSpeed * DEG2RAD;

    EGG::Matrix34f mat = EGG::Matrix34f::ident;
    EGG::Vector3f vRot = EGG::Vector3f(m_angleRad, 0.0f, 0.0f);
    mat.makeR(vRot);
    m_flags.setBit(eFlags::Matrix);
    m_transform = m_transform.multiplyTo(mat);
    m_transform.setBase(3, m_pos);
}

/// @addr{0x80771324}
void ObjectKoopaBall::calcExploding() {
    constexpr u32 EXPLODE_COLLISION_DURATION = 20;
    constexpr f32 INIT_FACTOR = 1.01f;
    constexpr EGG::Vector3f INIT_SCALE = EGG::Vector3f(INIT_FACTOR, INIT_FACTOR, INIT_FACTOR);

    if (m_explodeTimer > 30) {
        m_flags.setBit(eFlags::Scale);
        m_curScale += SCALE_DELTA;
        m_scale = EGG::Vector3f(m_curScale, m_curScale, m_curScale);
    }

    if (m_explodeTimer == EXPLODE_COLLISION_DURATION) {
        disableCollision();
    }

    if (--m_explodeTimer == 0) {
        m_curScale = INIT_FACTOR;
        m_flags.setBit(eFlags::Scale);
        m_scale = INIT_SCALE;
        m_railInterpolator->init(0.0f, 0);
        m_pos = m_railInterpolator->curPos();
        m_pos.y = m_startYpos;
        m_flags.setBit(eFlags::Position);
        enableCollision();
        m_state = State::Intangible;
    }
}

/// @addr{0x80771248}
void ObjectKoopaBall::calcIntangible() {
    if (m_cooldownTimer >= 0) {
        return;
    }

    m_state = State::Tangible;
    m_railInterpolator->setCurrVel(INITIAL_VELOCITY);
    m_angSpeed = INITIAL_ANGULAR_SPEED;
    m_vel.y = INITIAL_Y_VEL;
    m_cooldownTimer = 210;
}

/// @addr{0x80771624}
void ObjectKoopaBall::checkSphereFull() {
    CollisionInfo info;
    info.bbox.setZero();

    EGG::Vector3f pos = m_pos;
    pos.y += -900.0f;

    if (CollisionDirector::Instance()->checkSphereFull(100.0f, pos, EGG::Vector3f::inf,
                KCL_TYPE_FLOOR, &info, nullptr, 0)) {
        m_vel.y *= -0.4f;
        m_angSpeed = std::max(10.0f, m_angSpeed);
        m_pos += info.tangentOff;
        m_flags.setBit(eFlags::Position);
    }
}

} // namespace Field
