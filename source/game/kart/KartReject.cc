#include "KartReject.hh"

#include "game/kart/KartDynamics.hh"
#include "game/kart/KartMove.hh"
#include "game/kart/KartParam.hh"
#include "game/kart/KartState.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/CourseColMgr.hh"

#include <egg/math/Math.hh>
#include <egg/math/Quat.hh>

namespace Kart {

/// @addr{Inlined in 0x80577FC4}
KartReject::KartReject() = default;

/// @addr{0x8057815C}
KartReject::~KartReject() = default;

/// @addr{0x80585AE8}
void KartReject::reset() {
    m_rejectSign = 0.0f;
}

/// @addr{0x80585AF8}
void KartReject::calcRejectRoad() {
    if (state()->isInAction()) {
        return;
    }

    if (state()->isRejectRoadTrigger()) {
        EGG::Vector3f down = -EGG::Vector3f::ey;
        down = down.perpInPlane(move()->up(), true);
        f32 cos = down.dot(move()->lastDir());
        f32 sin = down.cross(move()->lastDir()).length();
        f32 angle = EGG::Mathf::atan2(sin, cos);
        angle = angle > 0.0f ? angle : -angle;

        f32 minAngle = 60.0f;
        angle *= RAD2DEG;
        f32 dVar11 = 1.0f;

        if (move()->up().dot(EGG::Vector3f::ey) < -0.7f) {
            minAngle *= 0.5f;
            dVar11 *= 2.0f;
        }

        if (angle > minAngle) {
            angle = 0.05f * (angle - 60.0f);
            EGG::Quatf rot;
            EGG::Vector3f vRot = EGG::Vector3f(0.0f,
                    (1.0f + angle * move()->speedRatio()) * dVar11 * DEG2RAD * m_rejectSign, 0.0f);
            rot.setRPY(vRot);
            EGG::Quatf local_78 = mainRot().multSwap(rot);
            local_78.normalise();

            dynamics()->setAngVel0(EGG::Vector3f::zero);
            dynamics()->setFullRot(local_78);
            dynamics()->setMainRot(local_78);
        }

        state()->setHop(false);

        bool didReject = calcRejection();

        if (!state()->isNoSparkInvisibleWall() && !didReject) {
            state()->setRejectRoadTrigger(false);
        }

        return;
    }

    if (state()->isRejectRoad() && !state()->isZipperInvisibleWall() && !state()->isOverZipper() &&
            !state()->isHalfPipeRamp()) {
        EGG::Vector3f upXZ = move()->up();
        upXZ.y = 0.0f;

        if (upXZ.length() > 0.0f && speed() > 0.0f) {
            upXZ.normalise();
            EGG::Vector3f local_88 = move()->lastDir().perpInPlane(upXZ, true);

            if (local_88.y > 0.0f) {
                EGG::Vector3f upCross = EGG::Vector3f::ey.cross(local_88);
                m_rejectSign = upCross.dot(move()->up()) > 0.0f ? 1.0f : -1.0f;

                state()->setHop(false);
                state()->setRejectRoadTrigger(true);
            }
        }
    }
}

/// @addr{0x805860BC}
bool KartReject::calcRejection() {
    Field::CollisionInfo colInfo;
    Field::KCLTypeMask mask = KCL_NONE;
    state()->setNoSparkInvisibleWall(false);
    EGG::Vector3f worldUpPos = dynamics()->pos() + bodyUp() * 100.0f;
    f32 posScalar = 100.0f;
    f32 radius = posScalar;

    for (size_t i = 0; i < 2; ++i) {
        EGG::Vector3f local_d0 = dynamics()->mainRot().rotateVector(EGG::Vector3f::ey);
        EGG::Vector3f worldPos = pos() + (-posScalar * move()->scale().y) * local_d0;

        auto *colDir = Field::CollisionDirector::Instance();
        if (!colDir->checkSphereFullPush(radius, worldPos, worldUpPos, KCL_TYPE_B0E82DFF, &colInfo,
                    &mask, 0)) {
            if (i == 0) {
                posScalar = 0.0f;
            }

            continue;
        }

        bool hasFloorCollision = false;
        bool hasRejectCollision = false;
        bool hasInvisibleWallCollision = false;
        EGG::Vector3f tangentOff = EGG::Vector3f::zero;

        if (mask & KCL_TYPE_INVISIBLE_WALL) {
            hasInvisibleWallCollision =
                    colDir->findClosestCollisionEntry(&mask, KCL_TYPE_INVISIBLE_WALL);
        }

        const auto *closestColEntry = colDir->closestCollisionEntry();
        if (hasInvisibleWallCollision && KCL_VARIANT_TYPE(closestColEntry->attribute) == 0) {
            hasRejectCollision = true;
            tangentOff = colInfo.wallNrm;
            state()->setNoSparkInvisibleWall(true);
        } else {
            if (!(mask & KCL_TYPE_DRIVER_FLOOR)) {
                hasFloorCollision = false;
            } else {
                hasFloorCollision = colDir->findClosestCollisionEntry(&mask, KCL_TYPE_DRIVER_FLOOR);
            }

            closestColEntry = colDir->closestCollisionEntry();
            if (hasFloorCollision && closestColEntry->attribute & 0x4000) {
                hasRejectCollision = true;
                tangentOff = colInfo.floorNrm;
            }
        }

        if (!hasRejectCollision) {
            continue;
        }

        EGG::Vector3f tangentUp = (tangentOff - move()->up()) * 1.0f;
        move()->setUp(move()->up() + tangentUp);
        move()->setSmoothedUp(move()->up());

        bool bVar15 = tangentOff.dot(EGG::Vector3f::ey) < -0.17f;
        if (bVar15 || extVel().y < 0.0f || state()->isNoSparkInvisibleWall()) {
            radius = -radius;
            colInfo.tangentOff += worldPos;

            f32 yOffset = bsp().initialYPos * scale().y;
            f32 speedScalar = bVar15 ?
                    1.0f :
                    static_cast<f32>(static_cast<f64>(EGG::Mathf::abs(speed()) * 0.01f) - 0.3d);
            speedScalar = std::min(1.0f, std::max(0.0f, speedScalar));

            EGG::Vector3f posOffset =
                    colInfo.tangentOff + radius * tangentOff + yOffset * tangentOff;
            posOffset.y += move()->hopPosY();
            posOffset -= pos();
            setPos(pos() + posOffset * speedScalar);
        }

        EGG::Vector3f local_13c = move()->lastDir().perpInPlane(move()->smoothedUp(), true);
        move()->setDir(local_13c);
        move()->setVel1Dir(local_13c);

        return true;
    }

    return false;
}

} // namespace Kart
