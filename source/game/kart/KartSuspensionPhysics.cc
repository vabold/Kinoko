#include "KartSuspensionPhysics.hh"

#include "game/kart/KartBody.hh"
#include "game/kart/KartCollide.hh"
#include "game/kart/KartDynamics.hh"
#include "game/kart/KartState.hh"
#include "game/kart/KartSub.hh"
#include "game/kart/KartTire.hh"

#include <egg/math/Math.hh>

namespace Kart {

/// @addr{0x8059940C}
WheelPhysics::WheelPhysics(u16 wheelIdx, u16 bspWheelIdx)
    : m_wheelIdx(wheelIdx), m_bspWheelIdx(bspWheelIdx), m_bspWheel(nullptr) {}

/// @addr{0x8059A9C4}
WheelPhysics::~WheelPhysics() {
    delete m_hitboxGroup;
}

/// @addr{0x80599470}
void WheelPhysics::init() {
    m_hitboxGroup = new CollisionGroup;
    m_hitboxGroup->createSingleHitbox(10.0f, EGG::Vector3f::zero);
}

/// @addr{0x805994D4}
void WheelPhysics::initBsp() {
    m_bspWheel = &bsp().wheels[m_bspWheelIdx];
}

/// @addr{0x80599508}
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

/// @addr{0x80599AD0}
void WheelPhysics::realign(const EGG::Vector3f &bottom, const EGG::Vector3f &vehicleMovement) {
    const EGG::Vector3f topmostPos = m_topmostPos + vehicleMovement;
    f32 scaledMaxTravel = m_bspWheel->maxTravel * sub()->someScale();
    f32 suspTravel = bottom.dot(m_pos - topmostPos);
    m_suspTravel = std::max(0.0f, std::min(scaledMaxTravel, suspTravel));
    m_pos = topmostPos + m_suspTravel * bottom;
    m_speed = m_pos - m_lastPos;
    m_speed -= dynamics()->intVel();
    m_speed -= dynamics()->movingObjVel();
    m_speed -= collisionData().movement;
    m_speed -= collide()->movement();
    m_hitboxGroup->collisionData().vel += m_speed;
    m_lastPos = m_pos;
    m_lastPosDiff = m_pos - topmostPos;
}

/// @addr{0x80599690}
void WheelPhysics::updateCollision(const EGG::Vector3f &bottom, const EGG::Vector3f &topmostPos) {
    m_targetEffectiveRadius = m_bspWheel->wheelRadius;
    if (!state()->isSkipWheelCalc()) {
        f32 nextRadius = m_bspWheel->sphereRadius;
        f32 scalar = m_effectiveRadius * scale().y - nextRadius * move()->totalScale();

        EGG::Vector3f center = m_pos + scalar * bottom;
        scalar = 0.3f * (nextRadius * move()->leanRot()) * move()->totalScale();
        center += scalar * bodyForward();

        if (state()->isInCannon()) {
            collisionData().reset();
        } else {
            m_hitboxGroup->setHitboxScale(move()->totalScale());
            if (state()->isUNK2()) {
                m_hitboxGroup->hitbox(0).setLastPos(dynamics()->pos());
            }

            collide()->calcWheelCollision(m_wheelIdx, m_hitboxGroup, m_colVel, center, nextRadius);
            CollisionData &colData = m_hitboxGroup->collisionData();

            if (colData.bFloor || colData.bWall || colData.bWall3) {
                m_pos += colData.tangentOff;
                if (colData.intensity > -1) {
                    f32 sinkDepth = 3.0f * static_cast<f32>(colData.intensity);
                    m_targetEffectiveRadius = m_bspWheel->wheelRadius - sinkDepth;
                    body()->trySetTargetSinkDepth(sinkDepth);
                }
            }
        }
        m_hitboxGroup->hitbox(0).setLastPos(center);
    }

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

/// @addr{0x80599ED4}
KartSuspensionPhysics::KartSuspensionPhysics(u16 wheelIdx, TireType tireType, u16 bspWheelIdx)
    : m_tirePhysics(nullptr), m_tireType(tireType), m_bspWheelIdx(bspWheelIdx),
      m_wheelIdx(wheelIdx) {}

/// @addr{0x8059AA04}
KartSuspensionPhysics::~KartSuspensionPhysics() = default;

/// @addr{0x80599FA0}
void KartSuspensionPhysics::init() {
    m_tirePhysics = tire(m_wheelIdx)->wheelPhysics();
    m_bspWheel = &bsp().wheels[m_bspWheelIdx];
}

/// @addr{0x80599F54}
void KartSuspensionPhysics::reset() {
    m_topmostPos.setZero();
    m_maxTravelScaled = 0.0f;
    m_bottomDir.setZero();
}

/// @addr{0x8059A02C}
void KartSuspensionPhysics::setInitialState() {
    EGG::Vector3f relPos = m_bspWheel->relPosition;
    if (m_tireType == TireType::KartReflected) {
        relPos.x = -relPos.x;
    }

    const EGG::Vector3f rotatedRelPos = dynamics()->fullRot().rotateVector(relPos) + pos();
    const EGG::Vector3f unitRotated = dynamics()->fullRot().rotateVector(-EGG::Vector3f::ey);

    m_tirePhysics->setPos(rotatedRelPos + m_bspWheel->maxTravel * unitRotated);
    m_tirePhysics->setLastPos(rotatedRelPos + m_bspWheel->maxTravel * unitRotated);
    m_tirePhysics->setLastPosDiff(m_tirePhysics->pos() - rotatedRelPos);
    m_tirePhysics->setWheelEdgePos(m_tirePhysics->pos() +
            (m_tirePhysics->effectiveRadius() * move()->totalScale() * unitRotated));
    m_tirePhysics->hitboxGroup()->hitbox(0).setWorldPos(m_tirePhysics->pos());
    m_tirePhysics->hitboxGroup()->hitbox(0).setLastPos(pos() + 100 * EGG::Vector3f::ey);
    m_topmostPos = rotatedRelPos;
}

/// @addr{0x8059A278}
void KartSuspensionPhysics::calcCollision(f32 dt, const EGG::Vector3f &gravity,
        const EGG::Matrix34f &mat) {
    m_maxTravelScaled = m_bspWheel->maxTravel * sub()->someScale();

    EGG::Vector3f scaledRelPos = m_bspWheel->relPosition * scale();
    if (m_tireType == TireType::KartReflected) {
        scaledRelPos.x = -scaledRelPos.x;
    }

    const EGG::Vector3f topmostPos = mat.ps_multVector(scaledRelPos);
    EGG::Matrix34f mStack_60;
    EGG::Vector3f euler_angles(m_bspWheel->xRot * DEG2RAD, 0.0f, 0.0f);
    mStack_60.makeR(euler_angles);
    EGG::Vector3f local_ac = mStack_60.multVector33(EGG::Vector3f(0.0f, -1.0f, 0.0f));
    m_bottomDir = mat.multVector33(local_ac);

    f32 y_down = m_tirePhysics->suspTravel() + sub()->someScale() * 5.0f;
    m_tirePhysics->setSuspTravel(std::max(0.0f, std::min(m_maxTravelScaled, y_down)));
    m_tirePhysics->setColVel(dt * 10.0f * gravity);
    m_tirePhysics->setPos(topmostPos + m_tirePhysics->suspTravel() * m_bottomDir);

    if (!state()->isSkipWheelCalc()) {
        m_tirePhysics->updateCollision(m_bottomDir, topmostPos);
        m_topmostPos = topmostPos;
    }
}

/// @stage All
/// @brief Calculates linear force and rotation from the kart's suspension.
/// @addr{0x8059A574}
void KartSuspensionPhysics::calcSuspension(const EGG::Vector3f &forward,
        const EGG::Vector3f &vehicleMovement) {
    EGG::Vector3f lastPosDiff = m_tirePhysics->lastPosDiff();

    m_tirePhysics->realign(m_bottomDir, vehicleMovement);

    CollisionGroup *hitboxGroup = m_tirePhysics->hitboxGroup();
    CollisionData &collisionData = hitboxGroup->collisionData();
    if (!collisionData.bFloor) {
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

    if (isInRespawn()) {
        fRot.y = std::max(-1.0f, std::min(1.0f, fRot.y));
    }

    EGG::Vector3f fLinear = fRot;
    EGG::Vector3f rotProj = fRot;
    rotProj.y = 0.0f;

    rotProj = rotProj.proj(collisionData.floorNrm);
    fLinear.y += rotProj.y;
    fLinear.y = std::min(fLinear.y, param()->stats().maxNormalAcceleration);

    if (dynamics()->extVel().y > 5.0f || state()->isJumpPadDisableYsusForce()) {
        fLinear.y = 0.0f;
    }

    dynamics()->applySuspensionWrench(m_topmostPos, fLinear, fRot, state()->isWheelieRot());

    f32 rate = state()->isSomethingWallCollision() ? 0.01f : collide()->floorMomentRate();

    collide()->applySomeFloorMoment(0.1f, rate, hitboxGroup, forward, move()->dir(),
            m_tirePhysics->speed(), true, true, !state()->isWheelieRot());
}

} // namespace Kart
