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

    void setAccelerate(bool isSet);
    void setDriftInput(bool isSet);
    void setDriftManual(bool isSet);
    void setBeforeRespawn(bool isSet);
    void setWallCollision(bool isSet);
    void setHopStart(bool isSet);
    void setVehicleBodyFloorCollision(bool isSet);
    void setAllWheelsCollision(bool isSet);
    void setAnyWheelCollision(bool isSet);
    void setWallCollisionStart(bool isSet);
    void setStickyRoad(bool isSet);
    void setTouchingGround(bool isSet);
    void setHop(bool isSet);
    void setBoost(bool isSet);
    void setMushroomBoost(bool isSet);
    void setSlipdriftCharge(bool isSet);
    void setDriftAuto(bool isSet);
    void setWheelie(bool isSet);
    void setRampBoost(bool isSet);
    void setTriggerRespawn(bool isSet);
    void setCannonStart(bool isSet);
    void setInCannon(bool isSet);
    void setTrickStart(bool isSet);
    void setInATrick(bool isSet);
    void setJumpPad(bool isSet);
    void setBoostOffroadInvincibility(bool isSet);
    void setHalfPipeRamp(bool isSet);
    void setOverZipper(bool isSet);
    void setZipperBoost(bool isSet);
    void setZipperStick(bool isSet);
    void setZipperTrick(bool isSet);
    void setDisableBackwardsAccel(bool isSet);
    void setRespawnKillY(bool isSet);
    void setBurnout(bool isSet);
    void setInRespawn(bool isSet);
    void setAfterRespawn(bool isSet);
    void setTrickRot(bool isSet);
    void setChargingSsmt(bool isSet);
    void setRejectRoad(bool isSet);
    void setRejectRoadTrigger(bool isSet);
    void setTrickable(bool isSet);
    void setWheelieRot(bool isSet);
    void setSkipWheelCalc(bool isSet);
    void setNoSparkInvisibleWall(bool isSet);
    void setJumpPadDisableYsusForce(bool isSet);
    void setSomethingWallCollision(bool isSet);
    void setSoftWallDrift(bool isSet);
    void setHWG(bool isSet);
    void setCannonPointId(u16 val);
    void setBoostRampType(s32 val);
    void setJumpPadVariant(s32 val);
    void setHalfPipeInvisibilityTimer(s16 val);
    void setTrickableTimer(s16 val);
    /// @endSetters

    /// @beginGetters
    [[nodiscard]] bool isDrifting() const;
    [[nodiscard]] bool isAccelerate() const;
    [[nodiscard]] bool isBrake() const;
    [[nodiscard]] bool isDriftInput() const;
    [[nodiscard]] bool isDriftManual() const;
    [[nodiscard]] bool isBeforeRespawn() const;
    [[nodiscard]] bool isWall3Collision() const;
    [[nodiscard]] bool isWallCollision() const;
    [[nodiscard]] bool isHopStart() const;
    [[nodiscard]] bool isAccelerateStart() const;
    [[nodiscard]] bool isGroundStart() const;
    [[nodiscard]] bool isVehicleBodyFloorCollision() const;
    [[nodiscard]] bool isAnyWheelCollision() const;
    [[nodiscard]] bool isAllWheelsCollision() const;
    [[nodiscard]] bool isStickLeft() const;
    [[nodiscard]] bool isWallCollisionStart() const;
    [[nodiscard]] bool isAirtimeOver20() const;
    [[nodiscard]] bool isStickyRoad() const;
    [[nodiscard]] bool isTouchingGround() const;
    [[nodiscard]] bool isHop() const;
    [[nodiscard]] bool isSoftWallDrift() const;
    [[nodiscard]] bool isHWG() const;
    [[nodiscard]] bool isChargeStartBoost() const;
    [[nodiscard]] bool isBoost() const;
    [[nodiscard]] bool isAirStart() const;
    [[nodiscard]] bool isStickRight() const;
    [[nodiscard]] bool isMushroomBoost() const;
    [[nodiscard]] bool isDriftAuto() const;
    [[nodiscard]] bool isSlipdriftCharge() const;
    [[nodiscard]] bool isWheelie() const;
    [[nodiscard]] bool isRampBoost() const;
    [[nodiscard]] bool isJumpPad() const;
    [[nodiscard]] bool isTriggerRespawn() const;
    [[nodiscard]] bool isCannonStart() const;
    [[nodiscard]] bool isInCannon() const;
    [[nodiscard]] bool isTrickStart() const;
    [[nodiscard]] bool isInATrick() const;
    [[nodiscard]] bool isBoostOffroadInvincibility() const;
    [[nodiscard]] bool isHalfPipeRamp() const;
    [[nodiscard]] bool isOverZipper() const;
    [[nodiscard]] bool isZipperBoost() const;
    [[nodiscard]] bool isZipperTrick() const;
    [[nodiscard]] bool isDisableBackwardsAccel() const;
    [[nodiscard]] bool isRespawnKillY() const;
    [[nodiscard]] bool isBurnout() const;
    [[nodiscard]] bool isZipperStick() const;
    [[nodiscard]] bool isTrickRot() const;
    [[nodiscard]] bool isChargingSsmt() const;
    [[nodiscard]] bool isRejectRoad() const;
    [[nodiscard]] bool isRejectRoadTrigger() const;
    [[nodiscard]] bool isTrickable() const;
    [[nodiscard]] bool isWheelieRot() const;
    [[nodiscard]] bool isSkipWheelCalc() const;
    [[nodiscard]] bool isNoSparkInvisibleWall() const;
    [[nodiscard]] bool isInRespawn() const;
    [[nodiscard]] bool isAfterRespawn() const;
    [[nodiscard]] bool isJumpPadDisableYsusForce() const;
    [[nodiscard]] bool isUNK2() const;
    [[nodiscard]] bool isSomethingWallCollision() const;
    [[nodiscard]] bool isAutoDrift() const;
    [[nodiscard]] u16 cannonPointId() const;
    [[nodiscard]] s32 boostRampType() const;
    [[nodiscard]] s32 jumpPadVariant() const;
    [[nodiscard]] f32 stickX() const;
    [[nodiscard]] f32 stickY() const;
    [[nodiscard]] u32 airtime() const;
    [[nodiscard]] const EGG::Vector3f &top() const;
    [[nodiscard]] const EGG::Vector3f &softWallSpeed() const;
    [[nodiscard]] f32 startBoostCharge() const;
    [[nodiscard]] s16 wallBonkTimer() const;
    [[nodiscard]] s16 trickableTimer() const;
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
    bool m_bTriggerRespawn;
    bool m_bCannonStart;
    bool m_bInCannon;
    bool m_bTrickStart;
    bool m_bInATrick;
    bool m_bBoostOffroadInvincibility; ///< Set if we should ignore offroad slowdown this frame.
    bool m_bHalfPipeRamp;              ///< Set while colliding with zipper KCL.
    bool m_bOverZipper;                ///< Set while mid-air from a zipper.
    bool m_bZipperBoost;               ///< Set when boosting after landing from a zipper.
    bool m_bZipperStick;               ///< Set while mid-air and still influenced by the zipper.
    bool m_bZipperTrick;               ///< Set while tricking mid-air from a zipper.
    bool m_bDisableBackwardsAccel;     ///< Enforces a 20f delay when reversing after charging SSMT.
    bool m_bRespawnKillY;              ///< Set while respawning to cap external velocity at 0.
    bool m_bBurnout;                   ///< Set during a burnout on race start.
    bool m_bTrickRot;
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
    bool m_bNoSparkInvisibleWall;
    bool m_bInRespawn;
    bool m_bAfterRespawn;
    bool m_bJumpPadDisableYsusForce;
    /// @}

    /// @name bitfield3
    /// The bitfield at offset 0x10.
    /// @{
    bool m_bUNK2;
    bool m_bSomethingWallCollision;
    bool m_bSoftWallDrift;
    bool m_bHWG;              ///< Set when "Horizontal Wall Glitch" is active.
    bool m_bChargeStartBoost; ///< Like @ref m_bAccelerate but during countdown.
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
