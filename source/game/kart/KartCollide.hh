#pragma once

#include "game/kart/CollisionGroup.hh"
#include "game/kart/KartObjectProxy.hh"

#include <game/field/CourseColMgr.hh>

namespace Kart {

/// @brief Manages body+wheel collision and its influence on position/velocity/etc.
/// @nosubgrouping
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
    void calcBodyCollision(f32 totalScale, f32 sinkDepth, const EGG::Quatf &rot,
            const EGG::Vector3f &scale);
    void calcFloorEffect();
    void calcTriggers(Field::KCLTypeMask *mask, const EGG::Vector3f &pos, bool twoPoint);
    void handleTriggers(Field::KCLTypeMask *mask);
    void calcFallBoundary(Field::KCLTypeMask *mask, bool shortBoundary);
    void activateOob(bool detachCamera, Field::KCLTypeMask *mask, bool somethingCPU,
            bool somethingBullet);
    void calcWheelCollision(u16 wheelIdx, CollisionGroup *hitboxGroup, const EGG::Vector3f &colVel,
            const EGG::Vector3f &center, f32 radius);
    void calcSideCollision(CollisionData &collisionData, Hitbox &hitbox,
            Field::CourseColMgr::CollisionInfo *colInfo);

    void processWheel(CollisionData &collisionData, Hitbox &hitbox,
            Field::CourseColMgr::CollisionInfo *colInfo, Field::KCLTypeMask *maskOut);
    void processBody(CollisionData &collisionData, Hitbox &hitbox,
            Field::CourseColMgr::CollisionInfo *colInfo, Field::KCLTypeMask *maskOut);
    [[nodiscard]] bool processWall(CollisionData &collisionData, Field::KCLTypeMask *maskOut);
    void processFloor(CollisionData &collisionData, Hitbox &hitbox,
            Field::CourseColMgr::CollisionInfo *colInfo, Field::KCLTypeMask *maskOut, bool wheel);

    void applySomeFloorMoment(f32 down, f32 rate, CollisionGroup *hitboxGroup,
            const EGG::Vector3f &forward, const EGG::Vector3f &nextDir, const EGG::Vector3f &speed,
            bool b1, bool b2, bool b3);

    [[nodiscard]] bool FUN_805B6A9C(CollisionData &collisionData, const Hitbox &hitbox,
            EGG::BoundBox3f &minMax, EGG::Vector3f &relPos, s32 &count,
            const Field::KCLTypeMask &maskOut, const Field::CourseColMgr::CollisionInfo &colInfo);
    void applyBodyCollision(CollisionData &collisionData, const EGG::Vector3f &movement,
            const EGG::Vector3f &posRel, s32 count);

    /// @beginSetters
    void setFloorColInfo(CollisionData &collisionData, const EGG::Vector3f &relPos,
            const EGG::Vector3f &vel, const EGG::Vector3f &floorNrm);
    void setMovement(const EGG::Vector3f &v);
    /// @endSetters

    /// @beginGetters
    [[nodiscard]] const EGG::Vector3f &movement() const;
    [[nodiscard]] f32 suspBottomHeightSoftWall() const;
    [[nodiscard]] u16 someSoftWallTimer() const;
    [[nodiscard]] f32 suspBottomHeightNonSoftWall() const;
    [[nodiscard]] u16 someNonSoftWallTimer() const;
    [[nodiscard]] f32 colPerpendicularity() const;

    [[nodiscard]] bool isRampBoost() const;
    [[nodiscard]] bool isTrickable() const;
    [[nodiscard]] bool isNotTrickable() const;
    /// @endGetters

private:
    EGG::Vector3f m_movement;
    s16 m_respawnTimer;
    f32 m_smoothedBack; // 0x50
    f32 m_suspBottomHeightSoftWall;
    u16 m_someSoftWallTimer;
    f32 m_suspBottomHeightNonSoftWall;
    u16 m_someNonSoftWallTimer;
    f32 m_colPerpendicularity;

    bool m_rampBoost;
    bool m_offRoad; ///< @unused
    bool m_groundBoostPanelOrRamp;
    bool m_trickable;
    bool m_notTrickable;
};

} // namespace Kart
