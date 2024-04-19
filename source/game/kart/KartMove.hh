#pragma once

#include "game/kart/KartBoost.hh"
#include "game/kart/KartObjectProxy.hh"

namespace Kart {

/// @brief Responsible for reacting to player inputs and moving the kart.
/// @nosubgrouping
class KartMove : protected KartObjectProxy {
public:
    KartMove();
    virtual ~KartMove();

    virtual void createSubsystems() {}
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
    void calcAirtimeTop();
    void calcSpecialFloor();
    void calcDirs();
    void calcStickyRoad();
    void calcOffroad();
    void calcBoost();
    void calcRampBoost();
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
    void tryStartBoostRamp();
    void tryStartJumpPad();
    void tryEndJumpPad();
    void cancelJumpPad();

    void activateBoost(KartBoost::Type type, s16 frames);
    void applyStartBoost(s16 frames);
    void activateMushroom();
    void setOffroadInvincibility(s16 timer);
    void calcOffroadInvincibility();
    void calcMushroomBoost();
    void landTrick();

    /// @beginSetters
    void setDir(const EGG::Vector3f &v);
    void setVel1Dir(const EGG::Vector3f &v);
    void setFloorCollisionCount(u16 count);
    void setKCLWheelSpeedFactor(f32 val);
    void setKCLWheelRotFactor(f32 val);
    void setPadBoost(bool isSet);
    void setRampBoost(bool isSet);
    void setPadJump(bool isSet);
    /// @endSetters

    /// @beginGetters
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
    const EGG::Vector3f &vel1Dir() const;
    f32 speedRatioCapped() const;
    u16 floorCollisionCount() const;
    s32 hopStickX() const;
    KartJump *jump() const;
    /// @endGetters

protected:
    enum class DriftState {
        NotDrifting = 0,
        ChargingMt = 1,
        ChargedMt = 2,
        ChargingSmt = 2,
        ChargedSmt = 3,
    };

    struct JumpPadProperties {
        f32 minSpeed;
        f32 maxSpeed;
        f32 velY;
    };

    f32 m_baseSpeed;      ///< The speed associated with the current character/vehicle stats.
    f32 m_softSpeedLimit; ///< Base speed + boosts + wheelies, restricted to the hard speed limit.
    f32 m_speed;          ///< Current speed, restricted to the soft speed limit.
    f32 m_lastSpeed;      ///< Last frame's speed, cached to calculate angular velocity.
    f32 m_hardSpeedLimit; ///< Absolute speed cap. It's 120, unless you're in a bullet (140).
    f32 m_acceleration;   ///< Captures the acceleration from player input and boosts.
    f32 m_speedDragMultiplier;  ///< After 5 frames of airtime, this causes speed to slowly decay.
    EGG::Vector3f m_smoothedUp; ///< A smoothed up vector, mostly used after significant airtime.
    EGG::Vector3f m_up;         ///< Vector perpendicular to the floor, pointing upwards.
    EGG::Vector3f m_landingDir;
    EGG::Vector3f m_dir;
    EGG::Vector3f m_vel1Dir;
    EGG::Vector3f m_dirDiff;
    bool m_hasLandingDir;
    f32 m_landingAngle;
    f32 m_speedRatioCapped; ///< The ratio between current speed and the player's base speed stat.
    f32 m_kclSpeedFactor;   ///< Float between 0-1 that scales the player's speed on offroad.
    f32 m_kclRotFactor; ///< Float between 0-1 that scales the player's turning radius on offroad.
    f32 m_kclWheelSpeedFactor; ///< The slowest speed multiplier of each wheel's floor collision.
    f32 m_kclWheelRotFactor;   ///< The slowest rotation multiplier of each wheel's floor collision.
    u16 m_floorCollisionCount; ///< The number of tires colliding with the floor.
    s32 m_hopStickX;        ///< A ternary for the direction of our hop, 0 if still neutral hopping.
    s32 m_hopFrame;         ///< A timer that can prevent subsequent hops until reset.
    EGG::Vector3f m_hopDir; ///< Used for outward drift. Tracks the forward vector of our rotation.
    f32 m_divingRot;        ///< Induces x-axis angular velocity based on up/down stick input.
    f32 m_standStillBoostRot;
    DriftState m_driftState;
    u16 m_mtCharge; ///< A value between 0 and 270 representing current MT charge.
    KartBoost m_boost;
    s16 m_offroadInvincibility; ///< A timer representing how many frames until the player is
                                ///< affected by offroad.
    f32 m_realTurn; ///< The "true" turn magnitude. Equal to @ref m_weighted turn unless drifting.
    f32 m_weightedTurn;    ///< Magnitude+direction of stick input, factoring in the kart's stats.
    EGG::Vector3f m_scale; ///< @unused Always 1.0f
    f32 m_totalScale;      ///< @unused Always 1.0f
    u16 m_mushroomBoostTimer; ///< Number of frames until the mushroom boost runs out.
    u32 m_nonZipperAirtime;
    f32 m_jumpPadMinSpeed; ///< Snaps the player to a minimum speed when first touching a jump pad.
    f32 m_jumpPadMaxSpeed;
    const JumpPadProperties *m_jumpPadProperties;
    u16 m_rampBoost;
    f32 m_hopVelY;    ///< Relative velocity due to a hop. Starts at 10 and decreases with gravity.
    f32 m_hopPosY;    ///< Relative position as the result of a hop. Starts at 0.
    f32 m_hopGravity; ///< Always main gravity (-1.3f).
    bool m_bPadBoost; ///< Caches whether the player is currently interacting with a boost pad.
    bool m_bRampBoost;
    bool m_bPadJump;
    KartJump *m_jump;
    f32 m_rawTurn; ///< Float in range [-1, 1]. Represents stick magnitude + direction.
};

/// @brief Responsible for reacting to player inputs and moving the bike.
/// @details This derived class has specialized behavior for bikes, such as wheelies and leaning.
/// There are also additional member variables to track the bike's unique state.
/// @nosubgrouping
class KartMoveBike : public KartMove {
public:
    KartMoveBike();
    ~KartMoveBike();

    virtual void startWheelie();
    void createSubsystems() override;
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

    /// @beginGetters
    f32 leanRot() const override;
    bool canWheelie() const override;
    /// @endGetters

private:
    f32 m_leanRot;         ///< Z-axis rotation of the bike from leaning.
    f32 m_leanRotCap;      ///< The maximum leaning rotation.
    f32 m_leanRotInc;      ///< The incrementor for leaning rotation.
    f32 m_wheelieRot;      ///< X-axis rotation from wheeling.
    f32 m_maxWheelieRot;   ///< The maximum wheelie rotation.
    u32 m_wheelieFrames;   ///< Tracks wheelie duration and cancels the wheelie after 180 frames.
    u16 m_wheelieCooldown; ///< The number of frames to ignore wheelie inputs for.
    f32 m_wheelieRotDec;   ///< The wheelie rotation decrementor, used after a wheelie has ended.
};

} // namespace Kart
