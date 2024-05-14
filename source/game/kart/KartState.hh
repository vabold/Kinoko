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
    void calcCollisions();
    void calcStartBoost();
    void calcHandleStartBoost();
    void handleStartBoost(size_t idx);

    /// @beginSetters
    void clearBitfield0();
    void clearBitfield1();
    void clearBitfield3();

    void setAccelerate(bool isSet);
    void setDriftInput(bool isSet);
    void setDriftManual(bool isSet);
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
    void setWheelie(bool isSet);
    void setRampBoost(bool isSet);
    void setTrickStart(bool isSet);
    void setInATrick(bool isSet);
    void setJumpPad(bool isSet);
    void setBoostOffroadInvincibility(bool isSet);
    void setDisableBackwardsAccel(bool isSet);
    void setTrickRot(bool isSet);
    void setChargingSsmt(bool isSet);
    void setTrickable(bool isSet);
    void setWheelieRot(bool isSet);
    void setJumpPadDisableYsusForce(bool isSet);
    void setSomethingWallCollision(bool isSet);
    void setSoftWallDrift(bool isSet);
    void setBoostRampType(s32 val);
    void setJumpPadVariant(s32 val);
    void setTrickableTimer(s16 val);
    /// @endSetters

    /// @beginGetters
    bool isDrifting() const;
    bool isAccelerate() const;
    bool isBrake() const;
    bool isDriftInput() const;
    bool isDriftManual() const;
    bool isWallCollision() const;
    bool isHopStart() const;
    bool isGroundStart() const;
    bool isVehicleBodyFloorCollision() const;
    bool isAnyWheelCollision() const;
    bool isAllWheelsCollision() const;
    bool isStickLeft() const;
    bool isWallCollisionStart() const;
    bool isAirtimeOver20() const;
    bool isStickyRoad() const;
    bool isTouchingGround() const;
    bool isHop() const;
    bool isSoftWallDrift() const;
    bool isChargeStartBoost() const;
    bool isBoost() const;
    bool isStickRight() const;
    bool isMushroomBoost() const;
    bool isDriftAuto() const;
    bool isSlipdriftCharge() const;
    bool isWheelie() const;
    bool isRampBoost() const;
    bool isJumpPad() const;
    bool isTrickStart() const;
    bool isInATrick() const;
    bool isBoostOffroadInvincibility() const;
    bool isDisableBackwardsAccel() const;
    bool isTrickRot() const;
    bool isChargingSsmt() const;
    bool isTrickable() const;
    bool isWheelieRot() const;
    bool isJumpPadDisableYsusForce() const;
    bool isUNK2() const;
    bool isSomethingWallCollision() const;
    bool isAutoDrift() const;
    s32 boostRampType() const;
    s32 jumpPadVariant() const;
    f32 stickX() const;
    f32 stickY() const;
    u32 airtime() const;
    const EGG::Vector3f &top() const;
    const EGG::Vector3f &softWallSpeed() const;
    f32 startBoostCharge() const;
    s16 wallBonkTimer() const;
    s16 trickableTimer() const;
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
    bool m_bTrickStart;
    bool m_bInATrick;
    bool m_bBoostOffroadInvincibility; ///< Set if we should ignore offroad slowdown this frame.
    bool m_bDisableBackwardsAccel;     ///< Enforces a 20f delay when reversing after charging SSMT.
    bool m_bTrickRot;
    bool m_bChargingSsmt; ///< Tracks whether we are charging a stand-still mini-turbo.
    bool m_bTrickable;
    /// @}

    /// @name bitfield2
    /// The bitfield at offset 0xC.
    /// @{
    bool m_bWheelieRot;
    bool m_bJumpPadDisableYsusForce;
    /// @}

    /// @name bitfield3
    /// The bitfield at offset 0x10.
    /// @{
    bool m_bUNK2;
    bool m_bSomethingWallCollision;
    bool m_bSoftWallDrift;
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
    s32 m_boostRampType;
    s32 m_jumpPadVariant;
    f32 m_stickX;           ///< One of 15 discrete stick values from [-1.0, 1.0].
    f32 m_stickY;           ///< One of 15 discrete stick values from [-1.0, 1.0].
    f32 m_startBoostCharge; ///< 0-1 representation of start boost charge. Burnout if >0.95f.
    size_t m_startBoostIdx; ///< Used to map @ref m_startBoostCharge to a start boost duration.
    s16 m_wallBonkTimer;    ///< 2f counter that stunts your speed after hitting a wall. @rename
    s16 m_trickableTimer;
};

} // namespace Kart
