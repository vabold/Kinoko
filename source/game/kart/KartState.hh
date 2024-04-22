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
    bool isTouchingGround() const;
    bool isHop() const;
    bool isChargeStartBoost() const;
    bool isBoost() const;
    bool isStickRight() const;
    bool isMushroomBoost() const;
    bool isDriftAuto() const;
    bool isWheelie() const;
    bool isBoostOffroadInvincibility() const;
    bool isWheelieRot() const;
    bool isAutoDrift() const;
    f32 stickX() const;
    f32 stickY() const;
    u32 airtime() const;
    const EGG::Vector3f &top() const;
    f32 startBoostCharge() const;

    void clearBitfield0();

    void setAccelerate(bool isSet);
    void setDriftManual(bool isSet);
    void setVehicleBodyFloorCollision(bool isSet);
    void setAllWheelsCollision(bool isSet);
    void setAnyWheelCollision(bool isSet);
    void setTouchingGround(bool isSet);
    void setHop(bool isSet);
    void setBoost(bool isSet);
    void setMushroomBoost(bool isSet);
    void setWheelie(bool isSet);
    void setBoostOffroadInvincibility(bool isSet);
    void setWheelieRot(bool isSet);

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
    bool m_bTouchingGround;
    bool m_bHop;
    bool m_bBoost;
    bool m_bStickRight;
    bool m_bMushroomBoost;
    bool m_bDriftAuto;
    bool m_bWheelie;

    bool m_bBoostOffroadInvincibility;

    bool m_bWheelieRot;

    bool m_bChargeStartBoost;

    bool m_bAutoDrift;

    u32 m_airtime;
    EGG::Vector3f m_top;
    f32 m_stickX;
    f32 m_stickY;
    f32 m_startBoostCharge;
    size_t m_startBoostIdx;
};

} // namespace Kart
