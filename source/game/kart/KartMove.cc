#include "KartMove.hh"

#include "game/kart/KartParam.hh"
#include "game/kart/KartSub.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/KCollisionTypes.hh"

#include <egg/math/Math.hh>
#include <egg/math/Quat.hh>

namespace Kart {

KartMove::KartMove() = default;

void KartMove::setInitialPhysicsValues(const EGG::Vector3f &pos, const EGG::Vector3f &angles) {
    EGG::Quatf quaternion;
    quaternion.setRPY(angles * DEG2RAD);
    EGG::Vector3f newPos = pos;
    Field::CourseColMgr::CollisionInfo info;
    info.astruct_7 = nullptr;
    Field::KCLTypeMask kcl_flags = KCL_NONE;

    bool bColliding = Field::CollisionDirector::Instance()->checkSphereFullPush(100.0f, newPos,
            EGG::Vector3f::inf, KCL_ANY, &info, &kcl_flags, 0);

    if (bColliding && (kcl_flags & KCL_TYPE_FLOOR)) {
        newPos += info.m_minPlusMax + (info.m_floorNrm * -100.0f);
        newPos += info.m_floorNrm * bsp().m_initialYPos;
    }

    setPos(newPos);
    setRot(quaternion);

    sub()->initPhysicsValues();
}

} // namespace Kart
