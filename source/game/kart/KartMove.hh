#pragma once

#include "game/kart/KartBoost.hh"
#include "game/kart/KartObjectProxy.hh"

namespace Kart {

class KartMove : public KartObjectProxy {
public:
    KartMove();
    virtual ~KartMove();

    virtual void calcAirtimeTop() {}
    virtual void calcTurn();
    virtual void calcWheelie() {}
    virtual void setTurnParams();
    virtual void init(bool b1, bool b2);
    virtual f32 leanRot() const;

    void setInitialPhysicsValues(const EGG::Vector3f &position, const EGG::Vector3f &angles);
    void setKartSpeedLimit();
    void resetDriftManual();

    void calc();
    void calcTop();
    void calcSpecialFloor();
    void calcDirs();
    void calcOffroad();
    bool calcPreDrift();
    void calcManualDrift();
    void startManualDrift();
    void releaseMt();
    void controlOutsideDriftAngle();
    void calcRotation();
    void calcVehicleSpeed();
    f32 calcVehicleAcceleration() const;
    void calcAcceleration();
    void calcStandstillBoostRot();
    void calcDive();
    void calcHopPhysics();
    virtual void calcVehicleRotation(f32 /*turn*/) {}
    virtual void hop();
    virtual void onHop() {}
    virtual void calcMtCharge() {}
    virtual f32 getWheelieSoftSpeedLimitBonus() const;
    virtual bool canWheelie() const;
    virtual bool canHop() const;

    void tryStartBoostPanel();

    void activateBoost(KartBoost::Type type, s16 frames);
    void applyStartBoost(s16 frames);
    void activateMushroom();
    void setOffroadInvincibility(s16 timer);
    void calcOffroadInvincibility();
    void calcMushroomBoost();

    void setFloorCollisionCount(u16 count);
    void setKCLWheelSpeedFactor(f32 val);
    void setKCLWheelRotFactor(f32 val);
    void setPadBoost(bool isSet);

    s32 getAppliedHopStickX() const;
    f32 softSpeedLimit() const;
    f32 speed() const;
    f32 acceleration() const;
    const EGG::Vector3f &scale() const;
    f32 hardSpeedLimit() const;
    const EGG::Vector3f &smoothedUp() const;
    const EGG::Vector3f &up() const;
    f32 totalScale() const;
    const EGG::Vector3f &dir() const;
    u16 floorCollisionCount() const;
    s32 hopStickX() const;

protected:
    enum class DriftState {
        NotDrifting = 0,
        ChargingMt = 1,
        ChargedMt = 2,
        ChargingSmt = 2,
        ChargedSmt = 3,
    };

    f32 m_baseSpeed;
    f32 m_softSpeedLimit;
    f32 m_speed;
    f32 m_lastSpeed;
    f32 m_hardSpeedLimit;
    f32 m_acceleration;
    f32 m_speedDragMultiplier;
    EGG::Vector3f m_smoothedUp;
    EGG::Vector3f m_up;
    EGG::Vector3f m_landingDir;
    EGG::Vector3f m_dir;
    EGG::Vector3f m_vel1Dir;
    EGG::Vector3f m_dirDiff;
    bool m_hasLandingDir;
    f32 m_landingAngle;
    f32 m_speedRatioCapped;
    f32 m_kclSpeedFactor;
    f32 m_kclRotFactor;
    f32 m_kclWheelSpeedFactor;
    f32 m_kclWheelRotFactor;
    u16 m_floorCollisionCount;
    s32 m_hopStickX;
    s32 m_hopFrame;
    EGG::Vector3f m_hopDir;
    f32 m_divingRot;
    f32 m_standStillBoostRot;
    DriftState m_driftState;
    u16 m_mtCharge;
    KartBoost m_boost;
    s16 m_offroadInvincibility;
    f32 m_realTurn;
    f32 m_weightedTurn;
    EGG::Vector3f m_scale;
    f32 m_totalScale;
    u16 m_mushroomBoostTimer;
    u32 m_nonZipperAirtime;
    f32 m_hopVelY;
    f32 m_hopPosY;
    f32 m_hopGravity;
    bool m_bPadBoost;
    f32 m_rawTurn;
};

class KartMoveBike : public KartMove {
public:
    KartMoveBike();
    ~KartMoveBike();

    virtual void startWheelie();

    void calcAirtimeTop() override;
    void calcVehicleRotation(f32 /*turn*/) override;
    void calcWheelie() override;
    void onHop() override;
    void calcMtCharge() override;
    void setTurnParams() override;
    void init(bool b1, bool b2) override;
    f32 getWheelieSoftSpeedLimitBonus() const override;
    f32 wheelieRotFactor() const;

    void tryStartWheelie();
    void cancelWheelie();

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
