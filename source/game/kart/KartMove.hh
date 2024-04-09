#pragma once

#include "game/kart/KartBoost.hh"
#include "game/kart/KartObjectProxy.hh"

namespace Kart {

class KartMove : public KartObjectProxy {
public:
    KartMove();

    virtual void calcTurn();
    virtual void calcWheelie() {}
    virtual void setTurnParams();
    virtual void init(bool b1, bool b2);
    virtual f32 leanRot() const;

    void setInitialPhysicsValues(const EGG::Vector3f &position, const EGG::Vector3f &angles);
    void setKartSpeedLimit();

    void calc();
    void calcTop();
    void calcDirs();
    void calcOffroad();
    void calcRotation();
    void calcVehicleSpeed();
    f32 calcVehicleAcceleration() const;
    void calcAcceleration();
    void calcStandstillBoostRot();
    virtual void calcVehicleRotation(f32 /*turn*/) {}
    virtual f32 getWheelieSoftSpeedLimitBonus() const;
    virtual bool canWheelie() const;

    void applyStartBoost(s16 frames);

    void setFloorCollisionCount(u16 count);
    void setKCLWheelRotFactor(f32 val);

    f32 softSpeedLimit() const;
    f32 speed() const;
    f32 acceleration() const;
    const EGG::Vector3f &scale() const;
    f32 hardSpeedLimit() const;
    const EGG::Vector3f &smoothedUp() const;
    f32 totalScale() const;
    const EGG::Vector3f &dir() const;
    u16 floorCollisionCount() const;

protected:
    f32 m_baseSpeed;
    f32 m_softSpeedLimit;
    f32 m_speed;
    f32 m_lastSpeed;
    f32 m_hardSpeedLimit;
    f32 m_acceleration;
    EGG::Vector3f m_smoothedUp;
    EGG::Vector3f m_up;
    EGG::Vector3f m_dir;
    EGG::Vector3f m_vel1Dir;
    EGG::Vector3f m_dirDiff;
    f32 m_speedRatioCapped;
    f32 m_kclRotFactor;
    f32 m_kclWheelRotFactor;
    u16 m_floorCollisionCount;
    f32 m_standStillBoostRot;
    KartBoost m_boost;
    f32 m_realTurn;
    f32 m_weightedTurn;
    f32 m_rawTurn;
    EGG::Vector3f m_scale;
    f32 m_totalScale;
};

class KartMoveBike : public KartMove {
public:
    KartMoveBike();
    ~KartMoveBike();

    virtual void startWheelie();
    virtual void cancelWheelie();

    void calcVehicleRotation(f32 /*turn*/) override;
    void calcWheelie() override;
    void setTurnParams() override;
    void init(bool b1, bool b2) override;
    f32 getWheelieSoftSpeedLimitBonus() const override;
    f32 wheelieRotFactor() const;

    void tryStartWheelie();

    f32 leanRot() const override;
    bool canWheelie() const override;

private:
    f32 m_leanRot;
    f32 m_leanRotCap;
    f32 m_leanRotInc;
    f32 m_wheelieRot;
    f32 m_maxWheelieRot;
    u32 m_wheelieFrames;
    u16 m_wheelieCooldown;
    f32 m_wheelieRotDec;
};

} // namespace Kart
