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

    bool isGround() const;
    bool isChargeStartBoost() const;
    bool isWheelie() const;
    bool isWheelieRot() const;
    f32 stickX() const;
    const EGG::Vector3f &top() const;
    f32 startBoostCharge() const;

    void setWheelie(bool isSet);
    void setWheelieRot(bool isSet);

private:
    bool m_ground;
    bool m_chargeStartBoost;
    bool m_wheelie;
    bool m_wheelieRot;

    EGG::Vector3f m_top;
    f32 m_stickX;
    f32 m_startBoostCharge;
};

} // namespace Kart
