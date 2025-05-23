#pragma once

#include "game/kart/KartObjectProxy.hh"

namespace Kart {

/// @brief Houses various flags and other variables to preserve the kart's state.
/// @details Most notably, this class is the direct observer of the input state,
/// and sets the appropriate flags for KartMove to act upon the input state.
/// This class also is responsible for managing calculations of the start boost duration.
/// @nosubgrouping
class KartState : KartObjectProxy {
public:
    KartState();

    void init();
    void reset();

    void calcInput();
    void calc();
    void resetFlags();
    void calcCollisions();
    void calcStartBoost();
    void calcHandleStartBoost();
    void handleStartBoost(size_t idx);
    void resetEjection();

    /// @beginSetters
    void clearBitfield0();
    void clearBitfield1();
    void clearBitfield2();
    void clearBitfield3();

    void setAccelerate(bool isSet) {
        m_bAccelerate = isSet;
    }

    void setDriftInput(bool isSet) {
        m_bDriftInput = isSet;
    }

    void setDriftManual(bool isSet) {
        m_bDriftManual = isSet;
    }

    void setBeforeRespawn(bool isSet) {
        m_bBeforeRespawn = isSet;
    }

    void setWallCollision(bool isSet) {
        m_bWallCollision = isSet;
    }

    void setHopStart(bool isSet) {
        m_bHopStart = isSet;
    }

    void setVehicleBodyFloorCollision(bool isSet) {
        m_bVehicleBodyFloorCollision = isSet;
    }

    void setAnyWheelCollision(bool isSet) {
        m_bAnyWheelCollision = isSet;
    }

    void setWallCollisionStart(bool isSet) {
        m_bWallCollisionStart = isSet;
    }

    void setStickyRoad(bool isSet) {
        m_bStickyRoad = isSet;
    }

    void setAllWheelsCollision(bool isSet) {
        m_bAllWheelsCollision = isSet;
    }

    void setTouchingGround(bool isSet) {
        m_bTouchingGround = isSet;
    }

    void setHop(bool isSet) {
        m_bHop = isSet;
    }

    void setBoost(bool isSet) {
        m_bBoost = isSet;
    }

    void setMushroomBoost(bool isSet) {
        m_bMushroomBoost = isSet;
    }

    void setSlipdriftCharge(bool isSet) {
        m_bSlipdriftCharge = isSet;
    }

    void setDriftAuto(bool isSet) {
        m_bDriftAuto = isSet;
    }

    void setWheelie(bool isSet) {
        m_bWheelie = isSet;
    }

    void setJumpPad(bool isSet) {
        m_bJumpPad = isSet;
    }

    void setRampBoost(bool isSet) {
        m_bRampBoost = isSet;
    }

    void setInAction(bool isSet) {
        m_bInAction = isSet;
    }

    void setTriggerRespawn(bool isSet) {
        m_bTriggerRespawn = isSet;
    }

    void setCannonStart(bool isSet) {
        m_bCannonStart = isSet;
    }

    void setInCannon(bool isSet) {
        m_bInCannon = isSet;
    }

    void setTrickStart(bool isSet) {
        m_bTrickStart = isSet;
    }

    void setInATrick(bool isSet) {
        m_bInATrick = isSet;
    }

    void setBoostOffroadInvincibility(bool isSet) {
        m_bBoostOffroadInvincibility = isSet;
    }

    void setHalfPipeRamp(bool isSet) {
        m_bHalfPipeRamp = isSet;
    }

    void setOverZipper(bool isSet) {
        m_bOverZipper = isSet;
    }

    void setJumpPadMushroomCollision(bool isSet) {
        m_bJumpPadMushroomCollision = isSet;
    }

    void setZipperInvisibleWall(bool isSet) {
        m_bZipperInvisibleWall = isSet;
    }

    void setZipperBoost(bool isSet) {
        m_bZipperBoost = isSet;
    }

    void setZipperStick(bool isSet) {
        m_bZipperStick = isSet;
    }

    void setZipperTrick(bool isSet) {
        m_bZipperTrick = isSet;
    }

    void setDisableBackwardsAccel(bool isSet) {
        m_bDisableBackwardsAccel = isSet;
    }

    void setRespawnKillY(bool isSet) {
        m_bRespawnKillY = isSet;
    }

    void setBurnout(bool isSet) {
        m_bBurnout = isSet;
    }

    void setTrickRot(bool isSet) {
        m_bTrickRot = isSet;
    }

    void setJumpPadMushroomVelYInc(bool isSet) {
        m_bJumpPadMushroomVelYInc = isSet;
    }

    void setChargingSsmt(bool isSet) {
        m_bChargingSsmt = isSet;
    }

    void setRejectRoad(bool isSet) {
        m_bRejectRoad = isSet;
    }

    void setRejectRoadTrigger(bool isSet) {
        m_bRejectRoadTrigger = isSet;
    }

    void setTrickable(bool isSet) {
        m_bTrickable = isSet;
    }

    void setWheelieRot(bool isSet) {
        m_bWheelieRot = isSet;
    }

    void setSkipWheelCalc(bool isSet) {
        m_bSkipWheelCalc = isSet;
    }

    void setJumpPadMushroomTrigger(bool isSet) {
        m_bJumpPadMushroomTrigger = isSet;
    }

    void setNoSparkInvisibleWall(bool isSet) {
        m_bNoSparkInvisibleWall = isSet;
    }

    void setInRespawn(bool isSet) {
        m_bInRespawn = isSet;
    }

    void setAfterRespawn(bool isSet) {
        m_bAfterRespawn = isSet;
    }

    void setJumpPadFixedSpeed(bool isSet) {
        m_bJumpPadFixedSpeed = isSet;
    }

    void setJumpPadDisableYsusForce(bool isSet) {
        m_bJumpPadDisableYsusForce = isSet;
    }

    void setSomethingWallCollision(bool isSet) {
        m_bSomethingWallCollision = isSet;
    }

    void setSoftWallDrift(bool isSet) {
        m_bSoftWallDrift = isSet;
    }

    void setHWG(bool isSet) {
        m_bHWG = isSet;
    }

    void setAfterCannon(bool isSet) {
        m_bAfterCannon = isSet;
    }

    void setActionMidZipper(bool isSet) {
        m_bActionMidZipper = isSet;
    }

    void setEndHalfPipe(bool isSet) {
        m_bEndHalfPipe = isSet;
    }

    void setCannonPointId(u16 val) {
        m_cannonPointId = val;
    }

    void setBoostRampType(s32 val) {
        m_boostRampType = val;
    }

    void setJumpPadVariant(s32 val) {
        m_jumpPadVariant = val;
    }

    void setHalfPipeInvisibilityTimer(s16 val) {
        m_halfPipeInvisibilityTimer = val;
    }

    void setTrickableTimer(s16 val) {
        m_trickableTimer = val;
    }
    /// @endSetters

    /// @beginGetters
    [[nodiscard]] bool isDrifting() const {
        return m_bDriftManual || m_bDriftAuto;
    }

    [[nodiscard]] bool isAccelerate() const {
        return m_bAccelerate;
    }

    [[nodiscard]] bool isBrake() const {
        return m_bBrake;
    }

    [[nodiscard]] bool isDriftInput() const {
        return m_bDriftInput;
    }

    [[nodiscard]] bool isDriftManual() const {
        return m_bDriftManual;
    }

    [[nodiscard]] bool isBeforeRespawn() const {
        return m_bBeforeRespawn;
    }

    [[nodiscard]] bool isWall3Collision() const {
        return m_bWall3Collision;
    }

    [[nodiscard]] bool isWallCollision() const {
        return m_bWallCollision;
    }

    [[nodiscard]] bool isHopStart() const {
        return m_bHopStart;
    }

    [[nodiscard]] bool isAccelerateStart() const {
        return m_bAccelerateStart;
    }

    [[nodiscard]] bool isGroundStart() const {
        return m_bGroundStart;
    }

    [[nodiscard]] bool isVehicleBodyFloorCollision() const {
        return m_bVehicleBodyFloorCollision;
    }

    [[nodiscard]] bool isAnyWheelCollision() const {
        return m_bAnyWheelCollision;
    }

    [[nodiscard]] bool isAllWheelsCollision() const {
        return m_bAllWheelsCollision;
    }

    [[nodiscard]] bool isStickLeft() const {
        return m_bStickLeft;
    }

    [[nodiscard]] bool isWallCollisionStart() const {
        return m_bWallCollisionStart;
    }

    [[nodiscard]] bool isAirtimeOver20() const {
        return m_bAirtimeOver20;
    }

    [[nodiscard]] bool isStickyRoad() const {
        return m_bStickyRoad;
    }

    [[nodiscard]] bool isTouchingGround() const {
        return m_bTouchingGround;
    }

    [[nodiscard]] bool isHop() const {
        return m_bHop;
    }

    [[nodiscard]] bool isSoftWallDrift() const {
        return m_bSoftWallDrift;
    }

    [[nodiscard]] bool isHWG() const {
        return m_bHWG;
    }

    [[nodiscard]] bool isAfterCannon() const {
        return m_bAfterCannon;
    }

    [[nodiscard]] bool isActionMidZipper() const {
        return m_bActionMidZipper;
    }

    [[nodiscard]] bool isChargeStartBoost() const {
        return m_bChargeStartBoost;
    }

    [[nodiscard]] bool isBoost() const {
        return m_bBoost;
    }

    [[nodiscard]] bool isAirStart() const {
        return m_bAirStart;
    }

    [[nodiscard]] bool isStickRight() const {
        return m_bStickRight;
    }

    [[nodiscard]] bool isMushroomBoost() const {
        return m_bMushroomBoost;
    }

    [[nodiscard]] bool isDriftAuto() const {
        return m_bDriftAuto;
    }

    [[nodiscard]] bool isSlipdriftCharge() const {
        return m_bSlipdriftCharge;
    }

    [[nodiscard]] bool isWheelie() const {
        return m_bWheelie;
    }

    [[nodiscard]] bool isJumpPad() const {
        return m_bJumpPad;
    }

    [[nodiscard]] bool isRampBoost() const {
        return m_bRampBoost;
    }

    [[nodiscard]] bool isInAction() const {
        return m_bInAction;
    }

    [[nodiscard]] bool isTriggerRespawn() const {
        return m_bTriggerRespawn;
    }

    [[nodiscard]] bool isCannonStart() const {
        return m_bCannonStart;
    }

    [[nodiscard]] bool isInCannon() const {
        return m_bInCannon;
    }

    [[nodiscard]] bool isTrickStart() const {
        return m_bTrickStart;
    }

    [[nodiscard]] bool isInATrick() const {
        return m_bInATrick;
    }

    [[nodiscard]] bool isBoostOffroadInvincibility() const {
        return m_bBoostOffroadInvincibility;
    }

    [[nodiscard]] bool isHalfPipeRamp() const {
        return m_bHalfPipeRamp;
    }

    [[nodiscard]] bool isOverZipper() const {
        return m_bOverZipper;
    }

    [[nodiscard]] bool isJumpPadMushroomCollision() const {
        return m_bJumpPadMushroomCollision;
    }

    [[nodiscard]] bool isZipperInvisibleWall() const {
        return m_bZipperInvisibleWall;
    }

    [[nodiscard]] bool isZipperBoost() const {
        return m_bZipperBoost;
    }

    [[nodiscard]] bool isZipperTrick() const {
        return m_bZipperTrick;
    }

    [[nodiscard]] bool isDisableBackwardsAccel() const {
        return m_bDisableBackwardsAccel;
    }

    [[nodiscard]] bool isRespawnKillY() const {
        return m_bRespawnKillY;
    }

    [[nodiscard]] bool isBurnout() const {
        return m_bBurnout;
    }

    [[nodiscard]] bool isZipperStick() const {
        return m_bZipperStick;
    }

    [[nodiscard]] bool isTrickRot() const {
        return m_bTrickRot;
    }

    [[nodiscard]] bool isJumpPadMushroomVelYInc() const {
        return m_bJumpPadMushroomVelYInc;
    }

    [[nodiscard]] bool isChargingSsmt() const {
        return m_bChargingSsmt;
    }

    [[nodiscard]] bool isRejectRoad() const {
        return m_bRejectRoad;
    }

    [[nodiscard]] bool isRejectRoadTrigger() const {
        return m_bRejectRoadTrigger;
    }

    [[nodiscard]] bool isTrickable() const {
        return m_bTrickable;
    }

    [[nodiscard]] bool isWheelieRot() const {
        return m_bWheelieRot;
    }

    [[nodiscard]] bool isJumpPadDisableYsusForce() const {
        return m_bJumpPadDisableYsusForce;
    }

    [[nodiscard]] bool isSkipWheelCalc() const {
        return m_bSkipWheelCalc;
    }

    [[nodiscard]] bool isJumpPadMushroomTrigger() const {
        return m_bJumpPadMushroomTrigger;
    }

    [[nodiscard]] bool isNoSparkInvisibleWall() const {
        return m_bNoSparkInvisibleWall;
    }

    [[nodiscard]] bool isInRespawn() const {
        return m_bInRespawn;
    }

    [[nodiscard]] bool isAfterRespawn() const {
        return m_bAfterRespawn;
    }

    [[nodiscard]] bool isJumpPadFixedSpeed() const {
        return m_bJumpPadFixedSpeed;
    }

    [[nodiscard]] bool isUNK2() const {
        return m_bUNK2;
    }

    [[nodiscard]] bool isSomethingWallCollision() const {
        return m_bSomethingWallCollision;
    }

    [[nodiscard]] bool isEndHalfPipe() const {
        return m_bEndHalfPipe;
    }

    [[nodiscard]] bool isAutoDrift() const {
        return m_bAutoDrift;
    }

    [[nodiscard]] u16 cannonPointId() const {
        return m_cannonPointId;
    }

    [[nodiscard]] s32 boostRampType() const {
        return m_boostRampType;
    }

    [[nodiscard]] s32 jumpPadVariant() const {
        return m_jumpPadVariant;
    }

    [[nodiscard]] f32 stickX() const {
        return m_stickX;
    }

    [[nodiscard]] f32 stickY() const {
        return m_stickY;
    }

    [[nodiscard]] u32 airtime() const {
        return m_airtime;
    }

    [[nodiscard]] const EGG::Vector3f &top() const {
        return m_top;
    }

    [[nodiscard]] const EGG::Vector3f &softWallSpeed() const {
        return m_softWallSpeed;
    }

    [[nodiscard]] f32 startBoostCharge() const {
        return m_startBoostCharge;
    }

    [[nodiscard]] s16 wallBonkTimer() const {
        return m_wallBonkTimer;
    }

    [[nodiscard]] s16 trickableTimer() const {
        return m_trickableTimer;
    }
    /// @endGetters

private:
    // Bits from the base game's bitfields are marked with prefix 'b'

    /// @name bitfield0
    /// The bitfield at offset 0x4. It mostly pertains to the input state and some collision info.
    /// @{
    bool m_bAccelerate; ///< Accel button is pressed.
    bool m_bBrake;      ///< Brake button is pressed.
    /// @brief A "fake" button, normally set if you meet the speed requirement to hop.
    /// @warning When playing back a ghost, the game will register a hop regardless of whether or
    /// not the acceleration button is pressed. This can lead to "successful" synchronization of
    /// ghosts which could not have been created legitimately in the first place.
    bool m_bDriftInput;
    bool m_bDriftManual;               ///< Currently in a drift w/ manual.
    bool m_bBeforeRespawn;             ///< Set on respawn collision, cleared on position snap.
    bool m_bWall3Collision;            ///< Set when colliding with wall KCL #COL_TYPE_WALL_2
    bool m_bWallCollision;             ///< Set if we are colliding with a wall.
    bool m_bHopStart;                  ///< Set if @ref m_bDriftInput was toggled on this frame.
    bool m_bAccelerateStart;           ///< Set if @ref m_bAccelerate was toggled on this frame.
    bool m_bGroundStart;               ///< Set first frame landing from airtime.
    bool m_bVehicleBodyFloorCollision; ///< Set if the vehicle body is colliding with the floor.
    bool m_bAnyWheelCollision;         ///< Set when any wheel is touching floor collision.
    bool m_bAllWheelsCollision;        ///< Set when all wheels are touching floor collision.
    bool m_bStickLeft; ///< Set on left stick input. Mutually exclusive to @ref m_bStickRight.
    bool m_bWallCollisionStart; ///< Set if we have just started colliding with a wall.
    bool m_bAirtimeOver20;      ///< Set after 20 frames of airtime, resets on landing.
    bool m_bStickyRoad;         ///< Like the rBC stairs
    bool m_bTouchingGround;     ///< Set when any part of the vehicle is colliding with floor KCL.
    bool m_bHop;                ///< Set while we are in a drift hop. Clears when we land.
    bool m_bBoost;              ///< Set while in a boost.
    bool m_bAirStart;
    bool m_bStickRight;    ///< Set on right stick input. Mutually exclusive to @ref m_bStickLeft.
    bool m_bMushroomBoost; ///< Set while we are in a mushroom boost.
    bool m_bDriftAuto;     ///< Currently in a drift w/ automatic.
    bool m_bSlipdriftCharge;
    bool m_bWheelie; ///< Set while we are in a wheelie (even during the countdown).
    bool m_bJumpPad;
    bool m_bRampBoost;
    /// @}

    /// @name bitfield1
    /// The bitfield at offset 0x8.
    /// @{
    bool m_bInAction;
    bool m_bTriggerRespawn;
    bool m_bCannonStart;
    bool m_bInCannon;
    bool m_bTrickStart;
    bool m_bInATrick;
    bool m_bBoostOffroadInvincibility; ///< Set if we should ignore offroad slowdown this frame.
    bool m_bHalfPipeRamp;              ///< Set while colliding with zipper KCL.
    bool m_bOverZipper;                ///< Set while mid-air from a zipper.
    bool m_bJumpPadMushroomCollision;
    bool m_bZipperInvisibleWall;   ///< Set when colliding with invisible wall above a zipper.
    bool m_bZipperBoost;           ///< Set when boosting after landing from a zipper.
    bool m_bZipperStick;           ///< Set while mid-air and still influenced by the zipper.
    bool m_bZipperTrick;           ///< Set while tricking mid-air from a zipper.
    bool m_bDisableBackwardsAccel; ///< Enforces a 20f delay when reversing after charging SSMT.
    bool m_bRespawnKillY;          ///< Set while respawning to cap external velocity at 0.
    bool m_bBurnout;               ///< Set during a burnout on race start.
    bool m_bTrickRot;
    bool m_bJumpPadMushroomVelYInc;
    bool m_bChargingSsmt;      ///< Tracks whether we are charging a stand-still mini-turbo.
    bool m_bRejectRoad;        ///< Collision which causes a change in the player's pos and rot.
    bool m_bRejectRoadTrigger; ///< e.g. DK Summit ending, and Maple Treeway side walls.
    bool m_bTrickable;
    /// @}

    /// @name bitfield2
    /// The bitfield at offset 0xC.
    /// @{
    bool m_bWheelieRot;
    bool m_bSkipWheelCalc;
    bool m_bJumpPadMushroomTrigger;
    bool m_bNoSparkInvisibleWall;
    bool m_bInRespawn;
    bool m_bAfterRespawn;
    bool m_bJumpPadFixedSpeed;
    bool m_bJumpPadDisableYsusForce;
    /// @}

    /// @name bitfield3
    /// The bitfield at offset 0x10.
    /// @{
    bool m_bUNK2;
    bool m_bSomethingWallCollision;
    bool m_bSoftWallDrift;
    bool m_bHWG; ///< Set when "Horizontal Wall Glitch" is active.
    bool m_bAfterCannon;
    bool m_bActionMidZipper;
    bool m_bChargeStartBoost; ///< Like @ref m_bAccelerate but during countdown.
    bool m_bEndHalfPipe;
    /// @}

    /// @name bitfield4
    /// The bitfield at offset 0x14.
    /// @{
    bool m_bAutoDrift; ///< True if auto transmission, false if manual.
    /// @}

    u32 m_airtime;
    EGG::Vector3f m_top;
    EGG::Vector3f m_softWallSpeed;
    s32 m_hwgTimer;
    u16 m_cannonPointId;
    s32 m_boostRampType;
    s32 m_jumpPadVariant;
    s16 m_halfPipeInvisibilityTimer;
    f32 m_stickX;           ///< One of 15 discrete stick values from [-1.0, 1.0].
    f32 m_stickY;           ///< One of 15 discrete stick values from [-1.0, 1.0].
    f32 m_startBoostCharge; ///< 0-1 representation of start boost charge. Burnout if >0.95f.
    size_t m_startBoostIdx; ///< Used to map @ref m_startBoostCharge to a start boost duration.
    s16 m_wallBonkTimer;    ///< 2f counter that stunts your speed after hitting a wall. @rename
    s16 m_trickableTimer;
};

} // namespace Kart
