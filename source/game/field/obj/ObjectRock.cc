#include "ObjectRock.hh"

#include "game/field/CollisionDirector.hh"

#include "game/kart/KartCollide.hh"

namespace Field {

/// @addr {0x8076F2E0}
ObjectRock::ObjectRock(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_cooldownDuration(params.setting(1)),
      m_vel(static_cast<f32>(params.setting(2))),
      m_intangibleHeight(static_cast<f32>(params.setting(3))) {}

/// @addr{0x8076F344}
ObjectRock::~ObjectRock() = default;

/// @addr{0x8076F384}
void ObjectRock::init() {
    m_railInterpolator->init(0.0f, 0);
    m_railInterpolator->setCurrVel(m_vel);
    m_railInterpolator->calc();

    m_state = State::Tangible;
    m_flags.setBit(eFlags::Position);
    m_pos.y = m_startYPos = m_railInterpolator->curPos().y;

    EGG::Vector3f curTanDirNorm = m_railInterpolator->curTangentDir();
    curTanDirNorm.normalise();
    setMatrixTangentTo(EGG::Vector3f::ey, curTanDirNorm);
    calcTransform();
    m_angRad = 0.0f;
    m_angSpd = INITIAL_ANGULAR_SPEED;
    m_cooldownTimer = m_mapObj->setting(0);
    m_colTranslate.x = 0.0f;
    m_colTranslate.y = m_intangibleHeight;
    m_colTranslate.z = 0.0f;
    calcTransform();
}

/// @addr{0x8076F590}
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
            m_railInterpolator->curTangentDir() * m_railInterpolator->getCurrVel();
    m_colTranslate.x = scaledTang.x;
    m_colTranslate.z = scaledTang.z;
}

// @addr{0x8076F768}
void ObjectRock::calcTangible() {
    auto railStatus = m_railInterpolator->calc();
    if (railStatus == RailInterpolator::Status::ChangingDirection) {
        breakRock();
    }

    m_flags.setBit(eFlags::Position);
    m_pos.x = m_railInterpolator->curPos().x;
    m_flags.setBit(eFlags::Position);
    m_pos.z = m_railInterpolator->curPos().z;
    m_colTranslate.y -= 2.0f;

    m_flags.setBit(eFlags::Position);
    m_pos += m_colTranslate;

    checkSphereFull();
    calcTangibleSub();
}

/// @addr{0x8076F868}
void ObjectRock::calcIntangible() {
    if (m_cooldownTimer < 0) {
        m_state = State::Tangible;
        m_angSpd = INITIAL_ANGULAR_SPEED;
        m_colTranslate.y = m_intangibleHeight;
        m_cooldownTimer = m_cooldownDuration;
        enableCollision();
    }
}

/// @addr{0x8076F91C}
void ObjectRock::calcTangibleSub() {
    EGG::Vector3f tangDir = m_railInterpolator->curTangentDir();
    tangDir.y = 0.0f;
    tangDir.normalise();
    EGG::Matrix34f m = OrthonormalBasis(tangDir);
    m_angRad += m_angSpd * DEG2RAD;

    EGG::Matrix34f mat(EGG::Matrix34f::ident);
    EGG::Vector3f vRot(m_angRad, 0.0f, 0.0f);
    mat.makeR(vRot);
    m_flags.setBit(eFlags::Matrix);
    m_transform = m.multiplyTo(mat);
    m_transform.setBase(3, m_pos);
}

// @addr {0x8076FA60}
void ObjectRock::checkSphereFull() {
    CollisionInfo info;

    EGG::Vector3f offset(0.0f, -(m_scale.x * 240.0f - 50.0f), 0.0f);
    EGG::Vector3f pos = m_pos + offset;

    if (CollisionDirector::Instance()->checkSphereFull(50.0f, pos, EGG::Vector3f::inf,
                KCL_TYPE_FLOOR, &info, nullptr, 0)) {
        m_colTranslate.y *= -0.3f;
        m_angSpd = std::max(360.0f * m_vel / (480.0f * m_scale.x * F_PI), m_angSpd + 1.0f);
        m_flags.setBit(eFlags::Position);
        m_pos += info.tangentOff;
    }
}

/// @addr{0x8076FD90}
void ObjectRock::breakRock() {
    m_state = State::Intangible;
    m_railInterpolator->init(0.0f, 0);
    m_railInterpolator->setCurrVel(m_vel);

    m_flags.setBit(eFlags::Position);
    m_pos.y = m_startYPos;
    disableCollision();
}

// @addr{0x80770068}
Kart::Reaction ObjectRock::onCollision(Kart::KartObject * /*kartObj*/,
        Kart::Reaction reactionOnKart, Kart::Reaction /*reactionOnObj*/,
        EGG::Vector3f & /*hitDepth*/) {
    if (m_scale.x < 1.0f) {
        breakRock();
        return Kart::Reaction::None;
    }
    return reactionOnKart;
}

} // namespace Field
