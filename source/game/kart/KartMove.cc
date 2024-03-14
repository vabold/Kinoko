#include "KartMove.hh"

#include "game/kart/KartDynamics.hh"
#include "game/kart/KartParam.hh"
#include "game/kart/KartPhysics.hh"
#include "game/kart/KartState.hh"
#include "game/kart/KartSub.hh"
#include "game/kart/KartSuspension.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/KCollisionTypes.hh"

#include <egg/math/Math.hh>
#include <egg/math/Quat.hh>

namespace Kart {

KartMove::KartMove()
    : m_smoothedUp(EGG::Vector3f::ey), m_scale(1.0f, 1.0f, 1.0f), m_totalScale(1.0f) {}

void KartMove::setTurnParams() {
    init(false, false);
}

void KartMove::init(bool b1, bool b2) {
    setKartSpeedLimit();
    m_up = EGG::Vector3f::ey;
    m_smoothedUp = EGG::Vector3f::ey;
    m_dir = EGG::Vector3f::ez;

    if (!b2) {
        m_floorCollisionCount = 0;
    }

    if (!b1) {
        m_scale.set(1.0f);
        m_totalScale = 1.0f;
    }

    m_realTurn = 0.0f;
}

f32 KartMove::leanRot() const {
    return 0.0f;
}

void KartMove::setInitialPhysicsValues(const EGG::Vector3f &position, const EGG::Vector3f &angles) {
    EGG::Quatf quaternion;
    quaternion.setRPY(angles * DEG2RAD);
    EGG::Vector3f newPos = position;
    Field::CourseColMgr::CollisionInfo info;
    Field::KCLTypeMask kcl_flags = KCL_NONE;

    bool bColliding = Field::CollisionDirector::Instance()->checkSphereFullPush(100.0f, newPos,
            EGG::Vector3f::inf, KCL_ANY, &info, &kcl_flags, 0);

    if (bColliding && (kcl_flags & KCL_TYPE_FLOOR)) {
        newPos += info.tangentOff + (info.floorNrm * -100.0f);
        newPos += info.floorNrm * bsp().initialYPos;
    }

    setPos(newPos);
    setRot(quaternion);

    sub()->initPhysicsValues();

    physics()->setPos(pos());
    physics()->setVelocity(dynamics()->velocity());

    m_dir = bodyFront();

    for (u16 tireIdx = 0; tireIdx < suspCount(); ++tireIdx) {
        suspension(tireIdx)->setInitialState();
    }
}

void KartMove::setKartSpeedLimit() {
    constexpr f32 LIMIT = 120.0f;
    m_hardSpeedLimit = LIMIT;
}

void KartMove::calc() {
    dynamics()->resetInternalVelocity();
    calcTop();
    calcDirs();
    calcRotation();
}

void KartMove::calcTop() {
    m_up += state()->top();

    f32 topDotZ = 0.8f - 6.0f * (EGG::Mathf::abs(state()->top().dot(componentZAxis())));
    f32 scalar = std::min(0.8f, std::max(0.3f, topDotZ));

    if (state()->isGround()) {
        m_smoothedUp += (state()->top() - m_smoothedUp) * scalar;
        m_smoothedUp.normalise();
    }
}

void KartMove::calcDirs() {
    EGG::Vector3f right = dynamics()->mainRot().rotateVector(EGG::Vector3f::ex);
    EGG::Vector3f local_88 = right.cross(m_smoothedUp);
    EGG::Matrix34f mat;
    mat.setAxisRotation(0.0f, m_smoothedUp);
    EGG::Vector3f local_b8 = mat.multVector(local_88);
    local_b8 = local_b8.perpInPlane(m_smoothedUp, true);

    EGG::Vector3f dirDiff = local_b8 - m_dir;

    if (dirDiff.dot() <= FLT_EPSILON) {
        m_dir = local_b8;
    }
}

void KartMove::calcRotation() {
    calcVehicleRotation(0.0f);
}

void KartMove::setFloorCollisionCount(u16 count) {
    m_floorCollisionCount = count;
}

const EGG::Vector3f &KartMove::scale() const {
    return m_scale;
}

f32 KartMove::hardSpeedLimit() const {
    return m_hardSpeedLimit;
}

const EGG::Vector3f &KartMove::smoothedUp() const {
    return m_smoothedUp;
}

f32 KartMove::totalScale() const {
    return m_totalScale;
}

const EGG::Vector3f &KartMove::dir() const {
    return m_dir;
}

u16 KartMove::floorCollisionCount() const {
    return m_floorCollisionCount;
}

KartMoveBike::KartMoveBike() : m_leanRot(0.0f) {}

KartMoveBike::~KartMoveBike() = default;

void KartMoveBike::setTurnParams() {
    KartMove::setTurnParams();
}

void KartMoveBike::init(bool b1, bool b2) {
    KartMove::init(b1, b2);
    m_leanRot = 0.0f;
}

f32 KartMoveBike::leanRot() const {
    return m_leanRot;
}

} // namespace Kart
