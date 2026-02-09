#include "ObjectRock.hh"

#include "game/field/CollisionDirector.hh"

#include "game/kart/KartCollide.hh"

namespace Field {

/// @addr {0x8076F2E0}
ObjectRock::ObjectRock(const System::MapdataGeoObj &params) : ObjectCollidable(params) {}

/// @addr{0x8076F344}
ObjectRock::~ObjectRock() = default;

/// @addr{0x8076F384}
void ObjectRock::init() {
    m_railInterpolator->init(0.0f, 0);
    m_railInterpolator->setCurrVel(static_cast<f32>(m_mapObj->setting(2)));
    m_railInterpolator->calc();

    m_state = State::Tangible;
    f32 posY = m_startYPos = m_railInterpolator->curPos().y;
    setPos(EGG::Vector3f(pos().x, posY, pos().z));

    EGG::Vector3f curTanDirNorm = m_railInterpolator->curTangentDir();
    curTanDirNorm.normalise();
    setMatrixTangentTo(EGG::Vector3f::ey, curTanDirNorm);
    calcTransform();
    m_angRad = 0.0f;
    m_angSpd = INITIAL_ANGULAR_SPEED;
    m_cooldownTimer = m_mapObj->setting(0);
    m_colTranslate.x = 0.0f;
    m_colTranslate.y = static_cast<f32>(m_mapObj->setting(3));
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

    const auto &railPos = m_railInterpolator->curPos();
    setPos(EGG::Vector3f(railPos.x, pos().y, railPos.z));
    m_colTranslate.y -= 2.0f;

    addPos(m_colTranslate);

    checkSphereFull();
    calcTangibleSub();
}

/// @addr{0x8076F868}
void ObjectRock::calcIntangible() {
    if (m_cooldownTimer < 0) {
        m_state = State::Tangible;
        m_angSpd = INITIAL_ANGULAR_SPEED;
        m_colTranslate.y = static_cast<f32>(m_mapObj->setting(3));
        m_cooldownTimer = m_mapObj->setting(1);
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
    mat = m.multiplyTo(mat);
    mat.setBase(3, pos());
    setTransform(mat);
}

// @addr {0x8076FA60}
void ObjectRock::checkSphereFull() {
    CollisionInfo info;

    EGG::Vector3f offset(0.0f, -(scale().x * 240.0f - 50.0f), 0.0f);
    EGG::Vector3f colPos = pos() + offset;

    if (CollisionDirector::Instance()->checkSphereFull(50.0f, colPos, EGG::Vector3f::inf,
                KCL_TYPE_FLOOR, &info, nullptr, 0)) {
        m_colTranslate.y *= -0.3f;
        m_angSpd = std::max(360.0f * static_cast<f32>(m_mapObj->setting(2)) /
                        (480.0f * scale().x * F_PI),
                m_angSpd + 1.0f);
        addPos(info.tangentOff);
    }
}

/// @addr{0x8076FD90}
void ObjectRock::breakRock() {
    m_state = State::Intangible;
    m_railInterpolator->init(0.0f, 0);
    m_railInterpolator->setCurrVel(static_cast<f32>(m_mapObj->setting(2)));

    setPos(EGG::Vector3f(pos().x, m_startYPos, pos().z));
    disableCollision();
}

// @addr{0x80770068}
Kart::Reaction ObjectRock::onCollision(Kart::KartObject * /*kartObj*/,
        Kart::Reaction reactionOnKart, Kart::Reaction /*reactionOnObj*/,
        EGG::Vector3f & /*hitDepth*/) {
    if (scale().x < 1.0f) {
        breakRock();
        return Kart::Reaction::None;
    }
    return reactionOnKart;
}

} // namespace Field
