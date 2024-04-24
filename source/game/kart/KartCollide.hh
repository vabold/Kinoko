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
    void FUN_80572F4C();
    void FUN_805B72B8(f32 param_1, f32 param_2, bool lockXZ, bool addExtVelY);
    void calcBodyCollision(f32 totalScale, const EGG::Quatf &rot, const EGG::Vector3f &scale);
    void calcFloorEffect();
    void calcTriggers(Field::KCLTypeMask *mask, const EGG::Vector3f &pos, bool twoPoint);
    void calcWheelCollision(u16 wheelIdx, CollisionGroup *hitboxGroup, const EGG::Vector3f &colVel,
            const EGG::Vector3f &center, f32 radius);

    void processWheel(CollisionData &collisionData, Hitbox &hitbox,
            Field::CourseColMgr::CollisionInfo *colInfo, Field::KCLTypeMask *maskOut);
    void processBody(CollisionData &collisionData, Hitbox &hitbox,
            Field::CourseColMgr::CollisionInfo *colInfo, Field::KCLTypeMask *maskOut);
    void processFloor(CollisionData &collisionData, Hitbox &hitbox,
            Field::CourseColMgr::CollisionInfo *colInfo, Field::KCLTypeMask *maskOut, bool wheel);

    void applySomeFloorMoment(f32 down, f32 rate, CollisionGroup *hitboxGroup,
            const EGG::Vector3f &forward, const EGG::Vector3f &nextDir, const EGG::Vector3f &speed,
            bool b1, bool b2, bool b3);

    bool FUN_805B6A9C(CollisionData &collisionData, const Hitbox &hitbox, EGG::BoundBox3f &minMax,
            EGG::Vector3f &relPos, s32 &count, Field::KCLTypeMask &maskOut,
            const Field::CourseColMgr::CollisionInfo &colInfo);
    void applyBodyCollision(CollisionData &collisionData, const EGG::Vector3f &movement,
            const EGG::Vector3f &posRel, s32 count);

    void setFloorColInfo(CollisionData &collisionData, const EGG::Vector3f &relPos,
            const EGG::Vector3f &vel, const EGG::Vector3f &floorNrm);

    const EGG::Vector3f &movement() const;
    f32 suspBottomHeightSoftWall() const;
    u16 someSoftWallTimer() const;
    f32 suspBottomHeightNonSoftWall() const;
    u16 someNonSoftWallTimer() const;

    bool isRampBoost() const;

    void setMovement(const EGG::Vector3f &v);

private:
    EGG::Vector3f m_movement;
    f32 m_smoothedBack; // 0x50
    f32 m_suspBottomHeightSoftWall;
    u16 m_someSoftWallTimer;
    f32 m_suspBottomHeightNonSoftWall;
    u16 m_someNonSoftWallTimer;

    bool m_rampBoost;
    bool m_offRoad;
    bool m_groundBoostPanelOrRamp;
    bool m_trickable;
    bool m_notTrickable;
};

} // namespace Kart
