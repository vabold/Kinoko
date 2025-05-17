#include "KartHalfPipe.hh"

#include "game/kart/KartCollide.hh"
#include "game/kart/KartDynamics.hh"
#include "game/kart/KartMove.hh"
#include "game/kart/KartParam.hh"
#include "game/kart/KartPhysics.hh"
#include "game/kart/KartState.hh"

#include "game/field/CourseColMgr.hh"

#include <egg/math/Math.hh>

namespace Kart {

/// @addr{0x80574114}
KartHalfPipe::KartHalfPipe() = default;

/// @addr{0x80574170}
KartHalfPipe::~KartHalfPipe() = default;

/// @addr{0x805741B0}
void KartHalfPipe::reset() {
    m_stunt = StuntType::None;
    m_touchingZipper = false;
    m_timer = 0;
}

/// @addr{0x80574340}
void KartHalfPipe::calc() {
    constexpr s16 LANDING_BOOST_DELAY = 3;

    if (state()->airtime() > 15 && state()->isOverZipper()) {
        m_timer = LANDING_BOOST_DELAY;
    }

    bool isLanding = state()->isHalfPipeRamp() && m_timer <= 0;

    calcTrick();

    if (!state()->isInAction() &&
            collide()->surfaceFlags().offBit(KartCollide::eSurfaceFlags::StopHalfPipeState) &&
            m_touchingZipper && state()->isAirStart()) {
        dynamics()->setExtVel(EGG::Vector3f::zero);
        state()->setOverZipper(true);

        EGG::Vector3f upXZ = move()->up();
        upXZ.y = 0.0f;
        upXZ.normalise();
        EGG::Vector3f up = move()->dir().perpInPlane(upXZ, true);

        EGG::Vector3f local_64 = up.cross(bodyUp().perpInPlane(up, true));
        m_nextSign = local_64.dot(EGG::Vector3f::ey) > 0.0f ? 1.0f : -1.0f;

        EGG::Vector3f velNorm = velocity();
        velNorm.normalise();
        EGG::Vector3f rot = dynamics()->mainRot().rotateVectorInv(velNorm);

        m_rot.makeVectorRotation(rot, EGG::Vector3f::ez);
        m_prevPos = prevPos();

        calcLanding(false);

        f32 scaledDir = std::min(65.0f, move()->dir().y * move()->speed());
        m_attemptedTrickTimer = std::max<s32>(0, scaledDir * 2.0f / 1.3f - 1.0f);
    } else {
        if (state()->isOverZipper()) {
            dynamics()->setGravity(-1.3f);

            EGG::Vector3f side = mainRot().rotateVector(EGG::Vector3f::ez);
            EGG::Vector3f velNorm = velocity();
            velNorm.normalise();

            EGG::Quatf sideRot;
            sideRot.makeVectorRotation(side, velNorm);
            sideRot = sideRot.multSwap(mainRot()).multSwap(m_rot);

            f32 t = move()->calcSlerpRate(DEG2RAD360, mainRot(), sideRot);
            EGG::Quatf slerp = mainRot().slerpTo(sideRot, t);
            dynamics()->setFullRot(slerp);
            dynamics()->setMainRot(slerp);

            --m_attemptedTrickTimer;

            calcRot();
            calcLanding(false);
        } else {
            if (state()->isHalfPipeRamp()) {
                calcLanding(true);
            }
        }
    }

    m_timer = std::max(0, m_timer - 1);
    m_touchingZipper = isLanding;
}

/// @addr{0x80574C90}
void KartHalfPipe::calcTrick() {
    constexpr s16 TRICK_COOLDOWN = 10;

    auto &trick = inputs()->currentState().trick;

    if (trick != System::Trick::None) {
        m_nextTimer = TRICK_COOLDOWN;
        m_trick = trick;
    }

    if (state()->isOverZipper()) {
        if (!state()->isZipperTrick() && m_nextTimer > 0 && state()->airtime() > 3 &&
                state()->airtime() < 10) {
            activateTrick(m_attemptedTrickTimer, m_trick);
        }
    }

    m_nextTimer = std::max(0, m_nextTimer - 1);
}

/// @addr{0x805750CC}
void KartHalfPipe::calcRot() {
    if (m_stunt == StuntType::None) {
        return;
    }

    m_stuntManager.calcAngle();

    f32 angle = m_rotSign * (DEG2RAD * m_stuntManager.angle);

    switch (m_stunt) {
    case StuntType::Side360:
    case StuntType::Side720:
        m_stuntRot.setRPY(EGG::Vector3f(0.0f, angle, 0.0f));
        break;
    case StuntType::Backside: {
        EGG::Quatf rpy;
        rpy.setRPY(
                EGG::Vector3f(0.0f, DEG2RAD * (0.25f * -m_rotSign * m_stuntManager.angle), 0.0f));
        EGG::Vector3f rot = rpy.rotateVector(EGG::Vector3f::ez);
        m_stuntRot.setAxisRotation(angle, rot);
    } break;
    case StuntType::Frontside: {
        EGG::Quatf rpy;
        rpy.setRPY(EGG::Vector3f(0.0f, 0.0f, DEG2RAD * (0.2f * -m_rotSign * m_stuntManager.angle)));
        EGG::Vector3f rot = rpy.rotateVector(EGG::Vector3f::ey);
        m_stuntRot.setAxisRotation(angle, rot);
    } break;
    case StuntType::Frontflip:
        m_stuntRot.setRPY(EGG::Vector3f(m_rotSign * angle, 0.0f, 0.0f));
        break;
    case StuntType::Backflip:
        m_stuntRot.setRPY(EGG::Vector3f(-m_rotSign * angle, 0.0f, 0.0f));
        break;
    default:
        break;
    }

    physics()->composeStuntRot(m_stuntRot);
}

/// @addr{0x805752E8}
void KartHalfPipe::calcLanding(bool) {
    constexpr f32 LANDING_RADIUS = 150.0f;
    constexpr f32 PREVIOUS_RADIUS = 200.0f;
    constexpr f32 MIDAIR_RADIUS = 50.0f;
    constexpr f32 WALL_RADIUS = 100.0f;

    constexpr f32 COS_PI_OVER_4 = 0.707f;

    Field::CollisionInfo colInfo;
    Field::CollisionInfo colInfo2;
    Field::KCLTypeMask maskOut;
    EGG::Vector3f pos;
    EGG::Vector3f upLocal;

    Field::KCLTypeMask mask = state()->isOverZipper() ?
            KCL_TYPE_ANY_INVISIBLE_WALL :
            KCL_TYPE_BIT(COL_TYPE_HALFPIPE_INVISIBLE_WALL);
    EGG::Vector3f prevPos = m_prevPos + EGG::Vector3f::ey * PREVIOUS_RADIUS;

    bool hasDriverFloorCollision = move()->calcZipperCollision(LANDING_RADIUS, bsp().initialYPos,
            pos, upLocal, prevPos, &colInfo, &maskOut, KCL_TYPE_DRIVER_FLOOR);

    prevPos = hasDriverFloorCollision ? EGG::Vector3f::inf : prevPos;

    if (state()->isOverZipper()) {
        if (!move()->calcZipperCollision(MIDAIR_RADIUS, bsp().initialYPos, pos, upLocal, prevPos,
                    &colInfo2, &maskOut, mask)) {
            mask |= KCL_TYPE_DRIVER_WALL;
        }
    }

    if (move()->calcZipperCollision(WALL_RADIUS, bsp().initialYPos, pos, upLocal, prevPos,
                &colInfo2, &maskOut, mask)) {
        EGG::Vector3f up = move()->up();
        move()->setUp(up + (colInfo2.wallNrm - up) * 0.2f);
        move()->setSmoothedUp(move()->up());

        f32 yScale = bsp().initialYPos * scale().y;
        EGG::Vector3f newPos =
                pos + colInfo2.tangentOff + -WALL_RADIUS * colInfo2.wallNrm + yScale * upLocal;
        newPos.y += move()->hopPosY();

        dynamics()->setPos(newPos);
        move()->setDir(move()->dir().perpInPlane(move()->up(), true));
        move()->setVel1Dir(move()->dir());

        if (state()->isOverZipper()) {
            state()->setZipperStick(true);
        }

        m_prevPos = newPos;
    } else {
        if (state()->isOverZipper()) {
            state()->setZipperStick(false);
        }
    }

    if (!hasDriverFloorCollision || state()->airtime() <= 5) {
        return;
    }

    if (colInfo.floorNrm.dot(EGG::Vector3f::ey) <= COS_PI_OVER_4) {
        return;
    }

    if (state()->isOverZipper()) {
        state()->setZipperStick(false);
    }
}

/// @addr{0x80574E60}
void KartHalfPipe::activateTrick(s32 duration, System::Trick trick) {
    if (duration < 51 || trick == System::Trick::None) {
        m_stunt = StuntType::None;
    } else {
        m_rotSign = m_nextSign;
        bool timerThreshold = duration > 70;

        switch (trick) {
        case System::Trick::Up:
            m_stunt = timerThreshold ? StuntType::Backside : StuntType::Backflip;
            break;
        case System::Trick::Down:
            m_stunt = timerThreshold ? StuntType::Frontside : StuntType::Frontflip;
            break;
        case System::Trick::Left:
        case System::Trick::Right:
            m_stunt = timerThreshold ? StuntType::Side720 : StuntType::Side360;
            m_rotSign = trick == System::Trick::Left ? 1.0f : -1.0f;
            break;
        default:
            break;
        }

        m_stuntManager.setProperties(static_cast<size_t>(m_stunt));

        state()->setZipperTrick(true);
    }

    m_stuntRot = EGG::Quatf::ident;
}

/// @addr{0x805758E4}
void KartHalfPipe::end(bool boost) {
    if (state()->isOverZipper() && state()->airtime() > 5 && boost) {
        move()->activateZipperBoost();
    }

    if (state()->isZipperTrick()) {
        physics()->composeDecayingStuntRot(m_stuntRot);
    }

    if (state()->isOverZipper()) {
        move()->setDir(mainRot().rotateVector(EGG::Vector3f::ez));
        move()->setVel1Dir(move()->dir());
    }

    state()->setOverZipper(false);
    state()->setZipperTrick(false);
    state()->setZipperStick(false);

    m_stunt = StuntType::None;
}

void KartHalfPipe::StuntManager::calcAngle() {
    if (finalAngle * properties.finalAngleScalar < angle) {
        angleDelta = std::max(properties.angleDeltaMin, angleDelta * angleDeltaFactor);
        angleDeltaFactor = std::max(properties.angleDeltaFactorMin,
                angleDeltaFactor - properties.angleDeltaFactorDecr);
    }

    angle = std::min(finalAngle, angle + angleDelta);
}

void KartHalfPipe::StuntManager::setProperties(size_t idx) {
    static constexpr std::array<StuntProperties, 6> STUNT_PROPERTIES = {{
            {6.0f, 2.5f, 0.955f, 0.01f, 0.7f, 360.0f},
            {7.0f, 3.0f, 0.955f, 0.01f, 0.7f, 360.0f},
            {7.0f, 3.0f, 0.95f, 0.01f, 0.7f, 360.0f},
            {12.0f, 2.5f, 0.955f, 0.01f, 0.0f, 360.0f},
            {4.0f, 4.0f, 0.98f, 0.01f, 0.0f, 360.0f},
            {9.0f, 3.0f, 0.92f, 0.01f, 0.8f, 720.0f},
    }};

    ASSERT(idx < STUNT_PROPERTIES.size());

    properties = STUNT_PROPERTIES[idx];
    finalAngle = properties.finalAngle;
    angleDelta = properties.angleDelta;
    angleDeltaFactorDecr = properties.angleDeltaFactorDecr;
    angle = 0.0f;
    angleDeltaFactor = 1.0f;
}

} // namespace Kart
