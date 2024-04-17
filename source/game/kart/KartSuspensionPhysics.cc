#include "KartSuspensionPhysics.hh"

#include "game/kart/KartCollide.hh"
#include "game/kart/KartDynamics.hh"
#include "game/kart/KartState.hh"
#include "game/kart/KartSub.hh"
#include "game/kart/KartTire.hh"

#include <egg/math/Math.hh>

namespace Kart {

WheelPhysics::WheelPhysics(u16 wheelIdx, u16 bspWheelIdx)
    : m_wheelIdx(wheelIdx), m_bspWheelIdx(bspWheelIdx), m_bspWheel(nullptr) {}

WheelPhysics::~WheelPhysics() = default;

void WheelPhysics::init() {
    m_hitboxGroup = new CollisionGroup;
    m_hitboxGroup->createSingleHitbox(10.0f, EGG::Vector3f::zero);
}

void WheelPhysics::initBsp() {
    m_bspWheel = &bsp().wheels[m_bspWheelIdx];
}

void WheelPhysics::reset() {
    m_pos.setZero();
    m_lastPos.setZero();
    m_lastPosDiff.setZero();
    m_suspTravel = 0.0f;
    m_colVel.setZero();
    m_speed.setZero();
    m_wheelEdgePos.setZero();
    m_effectiveRadius = 0.0f;
    m_targetEffectiveRadius = 0.0f;
    m_74 = 0.0f;
    m_topmostPos.setZero();

    if (m_bspWheel) {
        m_suspTravel = m_bspWheel->maxTravel;
        m_effectiveRadius = m_bspWheel->wheelRadius;
    }
}

void WheelPhysics::realign(const EGG::Vector3f &bottom, const EGG::Vector3f &vehicleMovement) {
    const EGG::Vector3f topmostPos = m_topmostPos + vehicleMovement;
    f32 scaledMaxTravel = m_bspWheel->maxTravel * sub()->someScale();
    f32 suspTravel = bottom.dot(m_pos - topmostPos);
    m_suspTravel = std::max(0.0f, std::min(scaledMaxTravel, suspTravel));
    m_pos = topmostPos + m_suspTravel * bottom;
    m_speed = m_pos - m_lastPos;
    m_speed -= dynamics()->intVel();
    m_speed -= collisionData().movement;
    m_hitboxGroup->collisionData().vel += m_speed;
    m_lastPos = m_pos;
    m_lastPosDiff = m_pos - topmostPos;
}

void WheelPhysics::updateCollision(const EGG::Vector3f &bottom, const EGG::Vector3f &topmostPos) {
    m_targetEffectiveRadius = m_bspWheel->wheelRadius;
    f32 nextRadius = m_bspWheel->sphereRadius;
    f32 scalar = m_effectiveRadius * scale().y - nextRadius * move()->totalScale();

    EGG::Vector3f center = m_pos + scalar * bottom;
    scalar = 0.3f * (nextRadius * move()->leanRot()) * move()->totalScale();
    center += scalar * bodyForward();
    m_hitboxGroup->setHitboxScale(move()->totalScale());
    collide()->calcWheelCollision(m_wheelIdx, m_hitboxGroup, m_colVel, center, nextRadius);
    CollisionData &colData = m_hitboxGroup->collisionData();

    if (colData.floor) {
        m_pos += colData.tangentOff;
        if (colData.intensity > -1) {
            m_targetEffectiveRadius = m_bspWheel->wheelRadius - static_cast<f32>(colData.intensity);
        }
    }

    m_hitboxGroup->hitbox(0).setLastPos(center);

    m_topmostPos = topmostPos;
    m_wheelEdgePos = m_pos + m_effectiveRadius * move()->totalScale() * bottom;
    m_effectiveRadius += (m_targetEffectiveRadius - m_effectiveRadius) * 0.1f;
    m_suspTravel = bottom.dot(m_pos - topmostPos);

    if (m_suspTravel < 0.0f) {
        m_74 = 1.0f;
        EGG::Vector3f suspBottom = m_suspTravel * bottom;
        sub()->updateSuspOvertravel(suspBottom);
    } else {
        m_74 = 0.0f;
    }
}

const EGG::Vector3f &WheelPhysics::pos() const {
    return m_pos;
}

const EGG::Vector3f &WheelPhysics::lastPosDiff() const {
    return m_lastPosDiff;
}

f32 WheelPhysics::suspTravel() {
    return m_suspTravel;
}

const EGG::Vector3f &WheelPhysics::topmostPos() const {
    return m_topmostPos;
}

CollisionGroup *WheelPhysics::hitboxGroup() {
    return m_hitboxGroup;
}

const CollisionGroup *WheelPhysics::hitboxGroup() const {
    return m_hitboxGroup;
}

const EGG::Vector3f &WheelPhysics::speed() const {
    return m_speed;
}

f32 WheelPhysics::effectiveRadius() const {
    return m_effectiveRadius;
}

f32 WheelPhysics::_74() const {
    return m_74;
}

void WheelPhysics::setSuspTravel(f32 suspTravel) {
    m_suspTravel = suspTravel;
}

void WheelPhysics::setPos(const EGG::Vector3f &pos) {
    m_pos = pos;
}

void WheelPhysics::setLastPos(const EGG::Vector3f &pos) {
    m_lastPos = pos;
}

void WheelPhysics::setLastPosDiff(const EGG::Vector3f &pos) {
    m_lastPosDiff = pos;
}

void WheelPhysics::setWheelEdgePos(const EGG::Vector3f &pos) {
    m_wheelEdgePos = pos;
}

void WheelPhysics::setColVel(const EGG::Vector3f &vec) {
    m_colVel = vec;
}

KartSuspensionPhysics::KartSuspensionPhysics(u16 wheelIdx, u16 bspWheelIdx)
    : m_tirePhysics(nullptr), m_bspWheelIdx(bspWheelIdx), m_wheelIdx(wheelIdx) {}

KartSuspensionPhysics::~KartSuspensionPhysics() = default;

void KartSuspensionPhysics::init() {
    m_tirePhysics = tire(m_wheelIdx)->wheelPhysics();
    m_bspWheel = &bsp().wheels[m_bspWheelIdx];
}

void KartSuspensionPhysics::reset() {
    m_topmostPos.setZero();
    m_maxTravelScaled = 0.0f;
    m_bottomDir.setZero();
}

void KartSuspensionPhysics::setInitialState() {
    EGG::Vector3f rotatedRelPos = dynamics()->fullRot().rotateVector(m_bspWheel->relPosition);
    rotatedRelPos += pos();

    EGG::Vector3f unitRotated = dynamics()->fullRot().rotateVector(-EGG::Vector3f::ey);

    m_tirePhysics->setPos(rotatedRelPos + m_bspWheel->maxTravel * unitRotated);
    m_tirePhysics->setLastPos(rotatedRelPos + m_bspWheel->maxTravel * unitRotated);
    m_tirePhysics->setLastPosDiff(m_tirePhysics->pos() - rotatedRelPos);
    m_tirePhysics->setWheelEdgePos(m_tirePhysics->pos() +
            (m_tirePhysics->effectiveRadius() * move()->totalScale() * unitRotated));
    m_tirePhysics->hitboxGroup()->hitbox(0).setWorldPos(m_tirePhysics->pos());
    m_tirePhysics->hitboxGroup()->hitbox(0).setLastPos(pos() + 100 * EGG::Vector3f::ey);
    m_topmostPos = rotatedRelPos;
}

void KartSuspensionPhysics::calcCollision(f32 dt, const EGG::Vector3f &gravity,
        const EGG::Matrix34f &mat) {
    m_maxTravelScaled = m_bspWheel->maxTravel * sub()->someScale();
    EGG::Vector3f topmostPos = mat.ps_multVector(m_bspWheel->relPosition * scale());
    EGG::Matrix34f mStack_60;
    EGG::Vector3f euler_angles(m_bspWheel->xRot * DEG2RAD, 0.0f, 0.0f);
    mStack_60.makeR(euler_angles);
    EGG::Vector3f local_ac = mStack_60.multVector33(EGG::Vector3f(0.0f, -1.0f, 0.0f));
    m_bottomDir = mat.multVector33(local_ac);

    f32 y_down = m_tirePhysics->suspTravel() + sub()->someScale() * 5.0f;
    m_tirePhysics->setSuspTravel(std::max(0.0f, std::min(m_maxTravelScaled, y_down)));
    m_tirePhysics->setColVel(dt * 10.0f * gravity);
    m_tirePhysics->setPos(topmostPos + m_tirePhysics->suspTravel() * m_bottomDir);
    m_tirePhysics->updateCollision(m_bottomDir, topmostPos);

    m_topmostPos = topmostPos;
}

void KartSuspensionPhysics::calcSuspension(const EGG::Vector3f &forward,
        const EGG::Vector3f &vehicleMovement) {
    EGG::Vector3f lastPosDiff = m_tirePhysics->lastPosDiff();

    m_tirePhysics->realign(m_bottomDir, vehicleMovement);

    CollisionGroup *hitboxGroup = m_tirePhysics->hitboxGroup();
    CollisionData &collisionData = hitboxGroup->collisionData();
    if (!collisionData.floor) {
        return;
    }

    EGG::Vector3f topDiff = m_tirePhysics->pos() - m_topmostPos;
    f32 yDown = std::max(0.0f, m_bottomDir.dot(topDiff));
    EGG::Vector3f speed = lastPosDiff - topDiff;
    f32 travel = m_maxTravelScaled - yDown;
    f32 speedScalar = m_bottomDir.dot(speed);

    f32 springDamp =
            -(m_bspWheel->springStiffness * travel + m_bspWheel->dampingFactor * speedScalar);

    EGG::Vector3f fRot = m_bottomDir * springDamp;
    EGG::Vector3f fLinear = fRot;
    EGG::Vector3f rotProj = fRot;
    rotProj.y = 0.0f;

    rotProj = rotProj.proj(collisionData.floorNrm);
    fLinear.y += rotProj.y;
    fLinear.y = std::min(fLinear.y, param()->stats().maxNormalAcceleration);

    if (dynamics()->extVel().y > 5.0f) {
        fLinear.y = 0.0f;
    }

    dynamics()->applySuspensionWrench(m_topmostPos, fLinear, fRot, state()->isWheelieRot());

    collide()->applySomeFloorMoment(0.1f, 0.8f, hitboxGroup, forward, move()->dir(),
            m_tirePhysics->speed(), true, true, !state()->isWheelieRot());
}

} // namespace Kart
