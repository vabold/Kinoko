#pragma once

#include "game/system/KPadController.hh"

#include <egg/math/Matrix.hh>

#include <list>
#include <vector>

namespace Field {

class BoxColUnit;
class ObjectCollisionKart;

} // namespace Field

namespace Render {

class KartModel;

} // namespace Render

namespace Kart {

class CollisionGroup;
struct CollisionData;
class KartAction;
class KartBody;
class KartCollide;
class KartDynamics;
class KartHalfPipe;
class KartJump;
class KartMove;
class KartParam;
struct BSP;
class KartPhysics;
class KartState;
class KartSub;
class KartSuspension;
class KartSuspensionPhysics;
class KartTire;
class WheelPhysics;

/// @brief Shared between classes who inherit KartObjectProxy so they can access one another.
struct KartAccessor {
    KartParam *param;
    KartBody *body;
    Render::KartModel *model;
    KartSub *sub;
    KartMove *move;
    KartAction *action;
    KartCollide *collide;
    Field::ObjectCollisionKart *objectCollisionKart;
    KartState *state;

    std::vector<KartSuspension *> suspensions;
    std::vector<KartTire *> tires;

    Field::BoxColUnit *boxColUnit;
};

/// @brief This is a union of the 5 bitfields contained within KartState
enum class eStatus {
    Accelerate = 0, ///< Accel button is pressed.
    Brake = 1,      ///< Brake button is pressed.
    /// @brief A "fake" button, normally set if you meet the speed requirement to hop.
    /// @warning When playing back a ghost, the game will register a hop regardless of whether
    /// or not the acceleration button is pressed. This can lead to "successful" synchronization
    /// of ghosts which could not have been created legitimately in the first place.
    DriftInput = 2,
    DriftManual = 3,                ///< Currently in a drift w/ manual.
    BeforeRespawn = 4,              ///< Set on respawn collision, cleared on position snap.
    Wall3Collision = 5,             ///< Set when colliding with wall KCL #COL_TYPE_WALL_2
    WallCollision = 6,              ///< Set if we are colliding with a wall.
    HopStart = 7,                   ///< Set if @ref m_bDriftInput was toggled on this frame.
    AccelerateStart = 8,            ///< Set if @ref m_bAccelerate was toggled on this frame.
    GroundStart = 9,                ///< Set first frame landing from airtime.
    VehicleBodyFloorCollision = 10, ///< Set if the vehicle body is colliding with the floor.
    AnyWheelCollision = 11,         ///< Set when any wheel is touching floor collision.
    AllWheelsCollision = 12,        ///< Set when all wheels are touching floor collision.
    StickLeft = 13,          ///< Set on left stick input. Mutually exclusive to @ref m_bStickRight.
    WallCollisionStart = 14, ///< Set if we have just started colliding with a wall.
    AirtimeOver20 = 15,      ///< Set after 20 frames of airtime, resets on landing.
    StickyRoad = 16,         ///< Like the rBC stairs
    TouchingGround = 18,     ///< Set when any part of the vehicle is colliding with floor KCL.
    Hop = 19,                ///< Set while we are in a drift hop. Clears when we land.
    Boost = 20,              ///< Set while in a boost.
    AirStart = 23,
    StickRight = 24,      ///< Set on right stick input. Mutually exclusive to @ref m_bStickLeft.
    MushroomBoost = 26,   ///< Set while we are in a mushroom boost.
    SlipdriftCharge = 27, ///< Currently in a drift w/ automatic.
    DriftAuto = 28,
    Wheelie = 29, ///< Set while we are in a wheelie (even during the countdown).
    JumpPad = 30,
    RampBoost = 31,

    InAction = 32,
    TriggerRespawn = 33,
    CannonStart = 35,
    InCannon = 36,
    TrickStart = 37,
    InATrick = 38,
    BoostOffroadInvincibility = 39, ///< Set if we should ignore offroad slowdown this frame.
    HalfPipeRamp = 41,              ///< Set while colliding with zipper KCL.
    OverZipper = 42,                ///< Set while mid-air from a zipper.
    ZipperInvisibleWall = 44,       ///< Set when colliding with invisible wall above a zipper.
    ZipperBoost = 45,               ///< Set when boosting after landing from a zipper.
    ZipperStick = 46,               ///< Set while mid-air and still influenced by the zipper.
    ZipperTrick = 47,               ///< Set while tricking mid-air from a zipper.
    DisableBackwardsAccel = 48,     ///< Enforces a 20f delay when reversing after charging SSMT.
    RespawnKillY = 49,              ///< Set while respawning to cap external velocity at 0.
    Burnout = 50,                   ///< Set during a burnout on race start.
    TrickRot = 54,
    ChargingSSMT = 57,      ///< Tracks whether we are charging a stand-still mini-turbo.
    RejectRoad = 59,        ///< Collision which causes a change in the player's pos and rot.
    RejectRoadTrigger = 60, ///< e.g. DK Summit ending, and Maple Treeway side walls.
    Trickable = 62,

    WheelieRot = 68,
    SkipWheelCalc = 69,
    NoSparkInvisibleWall = 75,
    InRespawn = 77,
    AfterRespawn = 78,
    JumpPadDisableYsusForce = 86,

    UNK2 = 97,
    SomethingWallCollision = 99,
    SoftWallDrift = 100,
    HWG = 101, ///< Set when "Horizontal Wall Glitch" is active.
    AfterCannon = 102,
    ChargeStartBoost = 104, ///< Like @ref m_bAccelerate but during countdown.
    EndHalfPipe = 107,

    AutoDrift = 132, ///< True if auto transmission, false if manual.

    FlagMax, ///< Internal. Total number of bits in the status.
};
typedef EGG::TBitFlagExt<static_cast<size_t>(eStatus::FlagMax), eStatus> Status;

/// @brief Base class for most kart-related objects.
/// @nosubgrouping
class KartObjectProxy {
    friend class KartObject;

public:
    KartObjectProxy();
    ~KartObjectProxy();

    /// @beginSetters
    void setPos(const EGG::Vector3f &pos);
    void setRot(const EGG::Quatf &q);
    void setInertiaScale(const EGG::Vector3f &scale);
    /// @endSetters

    /// @beginGetters
    [[nodiscard]] KartAction *action();
    [[nodiscard]] const KartAction *action() const;
    [[nodiscard]] KartBody *body();
    [[nodiscard]] const KartBody *body() const;
    [[nodiscard]] KartCollide *collide();
    [[nodiscard]] const KartCollide *collide() const;
    [[nodiscard]] CollisionGroup *collisionGroup();
    [[nodiscard]] const CollisionGroup *collisionGroup() const;
    [[nodiscard]] KartMove *move();
    [[nodiscard]] const KartMove *move() const;
    [[nodiscard]] KartHalfPipe *halfPipe();
    [[nodiscard]] const KartHalfPipe *halfPipe() const;
    [[nodiscard]] KartJump *jump();
    [[nodiscard]] const KartJump *jump() const;
    [[nodiscard]] KartParam *param();
    [[nodiscard]] const KartParam *param() const;
    [[nodiscard]] const BSP &bsp() const;
    [[nodiscard]] KartPhysics *physics();
    [[nodiscard]] const KartPhysics *physics() const;
    [[nodiscard]] KartDynamics *dynamics();
    [[nodiscard]] const KartDynamics *dynamics() const;
    [[nodiscard]] KartState *state();
    [[nodiscard]] const KartState *state() const;
    [[nodiscard]] KartSub *sub();
    [[nodiscard]] const KartSub *sub() const;
    [[nodiscard]] KartSuspension *suspension(u16 suspIdx);
    [[nodiscard]] const KartSuspension *suspension(u16 suspIdx) const;
    [[nodiscard]] KartSuspensionPhysics *suspensionPhysics(u16 suspIdx);
    [[nodiscard]] const KartSuspensionPhysics *suspensionPhysics(u16 suspIdx) const;
    [[nodiscard]] KartTire *tire(u16 tireIdx);
    [[nodiscard]] const KartTire *tire(u16 tireIdx) const;
    [[nodiscard]] WheelPhysics *tirePhysics(u16 tireIdx);
    [[nodiscard]] const WheelPhysics *tirePhysics(u16 tireIdx) const;
    [[nodiscard]] CollisionData &collisionData();
    [[nodiscard]] const CollisionData &collisionData() const;
    [[nodiscard]] CollisionData &collisionData(u16 tireIdx);
    [[nodiscard]] const CollisionData &collisionData(u16 tireIdx) const;
    [[nodiscard]] const System::KPad *inputs() const;
    [[nodiscard]] Render::KartModel *model();
    [[nodiscard]] const Render::KartModel *model() const;
    [[nodiscard]] Field::ObjectCollisionKart *objectCollisionKart();
    [[nodiscard]] const Field::ObjectCollisionKart *objectCollisionKart() const;
    [[nodiscard]] Field::BoxColUnit *boxColUnit();
    [[nodiscard]] const Field::BoxColUnit *boxColUnit() const;

    [[nodiscard]] const EGG::Vector3f &scale() const;
    [[nodiscard]] const EGG::Matrix34f &pose() const;
    [[nodiscard]] EGG::Vector3f bodyFront() const;
    [[nodiscard]] EGG::Vector3f bodyForward() const;
    [[nodiscard]] EGG::Vector3f bodyUp() const;

    [[nodiscard]] const EGG::Vector3f &componentXAxis() const;
    [[nodiscard]] const EGG::Vector3f &componentYAxis() const;
    [[nodiscard]] const EGG::Vector3f &componentZAxis() const;

    [[nodiscard]] const EGG::Vector3f &pos() const;
    [[nodiscard]] const EGG::Vector3f &prevPos() const;
    [[nodiscard]] const EGG::Quatf &fullRot() const;
    [[nodiscard]] const EGG::Vector3f &extVel() const;
    [[nodiscard]] const EGG::Vector3f &intVel() const;
    [[nodiscard]] const EGG::Vector3f &velocity() const;
    [[nodiscard]] f32 speed() const;
    [[nodiscard]] f32 acceleration() const;
    [[nodiscard]] f32 softSpeedLimit() const;
    [[nodiscard]] const EGG::Quatf &mainRot() const;
    [[nodiscard]] const EGG::Vector3f &angVel2() const;
    [[nodiscard]] bool isBike() const;
    [[nodiscard]] u16 suspCount() const;
    [[nodiscard]] u16 tireCount() const;
    [[nodiscard]] bool hasFloorCollision(const WheelPhysics *wheelPhysics) const;
    [[nodiscard]] std::pair<EGG::Vector3f, EGG::Vector3f> getCannonPosRot();
    [[nodiscard]] f32 speedRatio() const;
    [[nodiscard]] f32 speedRatioCapped() const;
    [[nodiscard]] bool isInRespawn() const;
    [[nodiscard]] Status &status();
    [[nodiscard]] const Status &status() const;

    [[nodiscard]] static std::list<KartObjectProxy *> &proxyList() {
        return s_proxyList;
    }
    /// @endGetters

protected:
    void apply(size_t idx);

private:
    static void ApplyAll(const KartAccessor *pointers);

    const KartAccessor *m_accessor;

    static std::list<KartObjectProxy *> s_proxyList; ///< List of all KartObjectProxy children.
};

} // namespace Kart
