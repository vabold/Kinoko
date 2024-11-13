#pragma once

#include "game/kart/CollisionGroup.hh"
#include "game/kart/KartAction.hh"
#include "game/kart/KartObjectProxy.hh"

#include "game/field/CourseColMgr.hh"

#include <egg/core/BitFlag.hh>

namespace Kart {

enum class Reaction {
    None = 0,
    UNK_3 = 3,
    UNK_4 = 4,
    UNK_5 = 5,
    UNK_7 = 7,
    WallAllSpeed = 8,
    SpinAllSpeed = 9,
    SpinSomeSpeed = 10,
    FireSpin = 11,
    ClipThroughSomeSpeed = 12,
    SmallLaunch = 13,
    KnockbackSomeSpeedLoseItem = 14,
    LaunchSpinLoseItem = 15,
    KnockbackBumpLoseItem = 16,
    LongCrushLoseItem = 17,
    SmallBump = 18,
    BigBump = 19,
    SpinShrink = 20,
    HighLaunchLoseItem = 21,
    SpinHitSomeSpeed = 22,
    WeakWall = 23,
    Wall = 24,
    LaunchSpin = 25,
    WallSpark = 26,
    RubberWall = 27,
    Wall2 = 28,
    UntrickableJumpPad = 29,
    ShortCrushLoseItem = 30,
    CrushRespawn = 31,
    ExplosionLoseItem = 32,
};

/// @brief Manages body+wheel collision and its influence on position/velocity/etc.
/// @nosubgrouping
class KartCollide : KartObjectProxy {
public:
    enum class eSurfaceFlags {
        ObjectWall = 2,
        ObjectWall3 = 3,
        BoostRamp = 4,
        Offroad = 6, ///< @unused
        GroundBoostPanelOrRamp = 7,
        Trickable = 11,
        NotTrickable = 12,
    };
    typedef EGG::TBitFlag<u32, eSurfaceFlags> SurfaceFlags;

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
    void calcBoundingRadius();
    void calcObjectCollision();

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

    /// Object collision functions
    Action handleReactNone(size_t idx);
    Action handleReactWallAllSpeed(size_t idx);
    Action handleReactSpinAllSpeed(size_t idx);
    Action handleReactSpinSomeSpeed(size_t idx);
    Action handleReactFireSpin(size_t idx);
    Action handleReactSmallLaunch(size_t idx);
    Action handleReactKnockbackSomeSpeedLoseItem(size_t idx);
    Action handleReactLaunchSpinLoseItem(size_t idx);
    Action handleReactKnockbackBumpLoseItem(size_t idx);
    Action handleReactLongCrushLoseItem(size_t idx);
    Action handleReactHighLaunchLoseItem(size_t idx);
    Action handleReactWeakWall(size_t idx);
    Action handleReactLaunchSpin(size_t idx);
    Action handleReactWallSpark(size_t idx);
    Action handleReactShortCrushLoseItem(size_t idx);
    Action handleReactCrushRespawn(size_t idx);
    Action handleReactExplosionLoseItem(size_t idx);

    /// @beginSetters
    void setFloorColInfo(CollisionData &collisionData, const EGG::Vector3f &relPos,
            const EGG::Vector3f &vel, const EGG::Vector3f &floorNrm);
    void setTangentOff(const EGG::Vector3f &v);
    void setMovement(const EGG::Vector3f &v);
    /// @endSetters

    /// @beginGetters
    [[nodiscard]] f32 boundingRadius() const;
    [[nodiscard]] const SurfaceFlags &surfaceFlags() const;
    [[nodiscard]] const EGG::Vector3f &tangentOff() const;
    [[nodiscard]] const EGG::Vector3f &movement() const;
    [[nodiscard]] f32 suspBottomHeightSoftWall() const;
    [[nodiscard]] u16 someSoftWallTimer() const;
    [[nodiscard]] f32 suspBottomHeightNonSoftWall() const;
    [[nodiscard]] u16 someNonSoftWallTimer() const;
    [[nodiscard]] f32 colPerpendicularity() const;
    /// @endGetters

private:
    typedef Action (KartCollide::*ObjectCollisionHandler)(size_t idx);

    f32 m_boundingRadius;
    EGG::Vector3f m_totalReactionWallNrm;
    SurfaceFlags m_surfaceFlags;
    EGG::Vector3f m_tangentOff;
    EGG::Vector3f m_movement;
    s16 m_respawnTimer;
    f32 m_smoothedBack; // 0x50
    f32 m_suspBottomHeightSoftWall;
    u16 m_someSoftWallTimer;
    f32 m_suspBottomHeightNonSoftWall;
    u16 m_someNonSoftWallTimer;
    f32 m_colPerpendicularity;

    static std::array<ObjectCollisionHandler, 33> s_objectCollisionHandlers;
};

} // namespace Kart
