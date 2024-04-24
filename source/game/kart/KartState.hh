#pragma once

#include "game/kart/KartObjectProxy.hh"

namespace Kart {

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

    bool isDrifting() const;

    bool isAccelerate() const;
    bool isDriftInput() const;
    bool isDriftManual() const;
    bool isHopStart() const;
    bool isGroundStart() const;
    bool isVehicleBodyFloorCollision() const;
    bool isAnyWheelCollision() const;
    bool isAllWheelsCollision() const;
    bool isStickLeft() const;
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
    bool isTrickStart() const;
    bool isInATrick() const;
    bool isBoostOffroadInvincibility() const;
    bool isTrickRot() const;
    bool isTrickable() const;
    bool isWheelieRot() const;
    bool isUNK2() const;
    bool isSomethingWallCollision() const;
    bool isAutoDrift() const;

    s32 boostRampType() const;
    f32 stickX() const;
    f32 stickY() const;
    u32 airtime() const;
    const EGG::Vector3f &top() const;
    const EGG::Vector3f &softWallSpeed() const;
    f32 startBoostCharge() const;

    void clearBitfield0();
    void clearBitfield1();
    void clearBitfield3();

    void setAccelerate(bool isSet);
    void setDriftManual(bool isSet);
    void setVehicleBodyFloorCollision(bool isSet);
    void setAllWheelsCollision(bool isSet);
    void setAnyWheelCollision(bool isSet);
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
    void setBoostOffroadInvincibility(bool isSet);
    void setTrickRot(bool isSet);
    void setTrickable(bool isSet);
    void setWheelieRot(bool isSet);
    void setSomethingWallCollision(bool isSet);
    void setSoftWallDrift(bool isSet);

    void setBoostRampType(s32 val);

private:
    // Bits from the base game's bitfields are marked with prefix 'b'
    bool m_bAccelerate;
    bool m_bBrake;
    bool m_bDriftInput;
    bool m_bDriftManual;
    bool m_bHopStart;
    bool m_bAccelerateStart;
    bool m_bGroundStart;
    bool m_bVehicleBodyFloorCollision;
    bool m_bAnyWheelCollision;
    bool m_bAllWheelsCollision;
    bool m_bStickLeft;
    /// @brief Set when we have been in the air for more than 20 frames
    bool m_bAirtimeOver20;
    /// @brief Like the rBC stairs
    bool m_bStickyRoad;
    bool m_bTouchingGround;
    bool m_bHop;
    bool m_bBoost;
    bool m_bStickRight;
    bool m_bMushroomBoost;
    bool m_bDriftAuto;
    bool m_bSlipdriftCharge;
    bool m_bWheelie;
    bool m_bRampBoost;

    bool m_bTrickStart;
    bool m_bInATrick;
    bool m_bBoostOffroadInvincibility;
    bool m_bTrickRot;
    bool m_bTrickable;

    bool m_bWheelieRot;

    bool m_bUNK2;
    bool m_bSomethingWallCollision;
    bool m_bSoftWallDrift;
    bool m_bChargeStartBoost;

    bool m_bAutoDrift;

    u32 m_airtime;
    EGG::Vector3f m_top;
    EGG::Vector3f m_softWallSpeed;
    s32 m_hwgTimer;
    s32 m_boostRampType;
    f32 m_stickX;
    f32 m_stickY;
    f32 m_startBoostCharge;
    size_t m_startBoostIdx;
};

} // namespace Kart
