#pragma once

#include "game/kart/CollisionGroup.hh"
#include "game/kart/KartObjectProxy.hh"

#include <game/field/CourseColMgr.hh>

namespace Kart {

class KartCollide : KartObjectProxy {
public:
    KartCollide();
    ~KartCollide();

    void init();
    void resetHitboxes();

    void calcHitboxes();

    void findCollision();
    void calcBodyCollision(f32 totalScale, const EGG::Quatf &rot, const EGG::Vector3f &scale);
    void calcFloorEffect();
    void calcTriggers(Field::KCLTypeMask *mask, const EGG::Vector3f &pos, bool twoPoint);
    void calcWheelCollision(u16 wheelIdx, CollisionGroup *hitboxGroup, const EGG::Vector3f &colVel,
            const EGG::Vector3f &center, f32 radius);

    void processWheel(CollisionData &collisionData, Hitbox &hitbox,
            Field::CourseColMgr::CollisionInfo *colInfo, Field::KCLTypeMask *maskOut);
    void processFloor(CollisionData &collisionData, Hitbox &hitbox,
            Field::CourseColMgr::CollisionInfo *colInfo, Field::KCLTypeMask *maskOut, bool wheel);

    void applySomeFloorMoment(f32 down, f32 rate, CollisionGroup *hitboxGroup,
            const EGG::Vector3f &forward, const EGG::Vector3f &nextDir, const EGG::Vector3f &speed,
            bool b1, bool b2, bool b3);

private:
    f32 m_smoothedBack; // 0x50

    bool m_offRoad;
    bool m_groundBoostPanelOrRamp;
    bool m_notTrickable;
};

} // namespace Kart
