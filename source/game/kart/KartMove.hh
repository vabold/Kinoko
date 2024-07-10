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

    virtual void createSubsystems();
    virtual void calcTurn();
    virtual void calcWheelie() {}
    virtual void setTurnParams();
    virtual void init(bool b1, bool b2);
    [[nodiscard]] virtual f32 leanRot() const;

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
    void calcDisableBackwardsAccel();
    void calcSsmt();
    bool calcPreDrift();
    void calcManualDrift();
    void startManualDrift();
    void clearDrift();
    void clearJumpPad();
    void clearRampBoost();
    void clearBoost();
    void clearSsmt();
    void clearOffroadInvincibility();
    void releaseMt();
    void controlOutsideDriftAngle();
    void calcRotation();
    void calcVehicleSpeed();
    void calcDeceleration();
    [[nodiscard]] f32 calcVehicleAcceleration() const;
    void calcAcceleration();
    [[nodiscard]] f32 calcWallCollisionSpeedFactor(f32 &f1);
    void calcWallCollisionStart(f32 param_2);
    void calcStandstillBoostRot();
    void calcDive();
    void calcSsmtStart();
    void calcHopPhysics();
    virtual void calcVehicleRotation(f32 turn);
    virtual void hop();
    virtual void onHop() {}
    virtual void onWallCollision() {}
    virtual void calcMtCharge();
    virtual void initOob() {}
    [[nodiscard]] virtual f32 getWheelieSoftSpeedLimitBonus() const;
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

    void enterCannon();
    void calcCannon();
    void calcRotCannon(const EGG::Vector3f &forward);
    void exitCannon();

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
    [[nodiscard]] s32 getAppliedHopStickX() const;
    [[nodiscard]] f32 softSpeedLimit() const;
    [[nodiscard]] f32 speed() const;
    [[nodiscard]] f32 acceleration() const;
    [[nodiscard]] const EGG::Vector3f &scale() const;
    [[nodiscard]] f32 hardSpeedLimit() const;
    [[nodiscard]] const EGG::Vector3f &smoothedUp() const;
    [[nodiscard]] const EGG::Vector3f &up() const;
    [[nodiscard]] f32 totalScale() const;
    [[nodiscard]] const EGG::Vector3f &dir() const;
    [[nodiscard]] const EGG::Vector3f &vel1Dir() const;
    [[nodiscard]] f32 speedRatioCapped() const;
    [[nodiscard]] u16 floorCollisionCount() const;
    [[nodiscard]] s32 hopStickX() const;
    [[nodiscard]] KartJump *jump() const;
    /// @endGetters

protected:
    enum class DriftState {
        NotDrifting = 0,
        ChargingMt = 1,
        ChargedMt = 2,
        ChargingSmt = 2,
        ChargedSmt = 3,
    };

    /// @brief The direction the player is currently driving in.
    enum class DrivingDirection {
        Forwards = 0,
        Braking = 1,
        WaitingForBackwards = 2, ///< Holding reverse but waiting on a 15 frame delay.
        Backwards = 3,
    };

    struct JumpPadProperties {
        f32 minSpeed;
        f32 maxSpeed;
        f32 velY;
    };

    /// @brief Houses parameters that vary between the drift type (inward bike, outward bike, kart).
    struct DriftingParameters {
        f32 hopVelY;
        f32 stabilizationFactor;
        f32 _8;
        f32 boostRotFactor;
    };

    f32 m_baseSpeed;      ///< The speed associated with the current character/vehicle stats.
    f32 m_softSpeedLimit; ///< Base speed + boosts + wheelies, restricted to the hard speed limit.
    f32 m_speed;          ///< Current speed, restricted to the soft speed limit.
    f32 m_lastSpeed;      ///< Last frame's speed, cached to calculate angular velocity.
    f32 m_processedSpeed; ///< Offset 0x28. It's only ever just a copy of @ref m_speed.
    f32 m_hardSpeedLimit; ///< Absolute speed cap. It's 120, unless you're in a bullet (140).
    f32 m_acceleration;   ///< Captures the acceleration from player input and boosts.
    f32 m_speedDragMultiplier;  ///< After 5 frames of airtime, this causes speed to slowly decay.
    EGG::Vector3f m_smoothedUp; ///< A smoothed up vector, mostly used after significant airtime.
    EGG::Vector3f m_up;         ///< Vector perpendicular to the floor, pointing upwards.
    EGG::Vector3f m_landingDir;
    EGG::Vector3f m_dir;
    EGG::Vector3f m_lastDir; ///< @ref m_speed from the previous frame but with signed magnitude.
    EGG::Vector3f m_vel1Dir;
    EGG::Vector3f m_dirDiff;
    bool m_hasLandingDir;
    f32 m_outsideDriftAngle; ///< The facing angle of an outward-drifting vehicle.
    f32 m_landingAngle;
    EGG::Vector3f m_outsideDriftLastDir; ///< Used to compute the next @ref m_outsideDriftAngle.
    f32 m_speedRatioCapped; ///< The ratio between current speed and the player's base speed stat.
    f32 m_kclSpeedFactor;   ///< Float between 0-1 that scales the player's speed on offroad.
    f32 m_kclRotFactor; ///< Float between 0-1 that scales the player's turning radius on offroad.
    f32 m_kclWheelSpeedFactor; ///< The slowest speed multiplier of each wheel's floor collision.
    f32 m_kclWheelRotFactor;   ///< The slowest rotation multiplier of each wheel's floor collision.
    u16 m_floorCollisionCount; ///< The number of tires colliding with the floor.
    s32 m_hopStickX;        ///< A ternary for the direction of our hop, 0 if still neutral hopping.
    s32 m_hopFrame;         ///< A timer that can prevent subsequent hops until reset.
    EGG::Vector3f m_hopUp;  ///< The up vector when hopping.
    EGG::Vector3f m_hopDir; ///< Used for outward drift. Tracks the forward vector of our rotation.
    f32 m_divingRot;        ///< Induces x-axis angular velocity based on up/down stick input.
    f32 m_standStillBoostRot;
    DriftState m_driftState;
    u16 m_mtCharge;          ///< A value between 0 and 270 representing current MT charge.
    u16 m_smtCharge;         ///< A value between 0 and 300 representing current SMT charge.
    f32 m_outsideDriftBonus; ///< Added to angular velocity when outside drifting.
    KartBoost m_boost;
    s16 m_offroadInvincibility;  ///< How many frames until the player is affected by offroad.
    s16 m_ssmtCharge;            ///< Increments every frame up to 75 when charging stand-still MT.
    s16 m_ssmtLeewayTimer;       ///< Frames to forgive letting go of A before clearing SSMT charge.
    s16 m_ssmtDisableAccelTimer; ///< Counter that tracks delay before starting to reverse.
    f32 m_realTurn; ///< The "true" turn magnitude. Equal to @ref m_weightedTurn unless drifting.
    f32 m_weightedTurn;    ///< Magnitude+direction of stick input, factoring in the kart's stats.
    EGG::Vector3f m_scale; ///< @unused Always 1.0f
    f32 m_totalScale;      ///< @unused Always 1.0f
    u16 m_mushroomBoostTimer; ///< Number of frames until the mushroom boost runs out.
    u32 m_nonZipperAirtime;
    f32 m_jumpPadMinSpeed; ///< Snaps the player to a minimum speed when first touching a jump pad.
    f32 m_jumpPadMaxSpeed;
    const JumpPadProperties *m_jumpPadProperties;
    u16 m_rampBoost;
    f32 m_cannonEntryOfsLength;
    EGG::Vector3f m_cannonEntryPos;
    EGG::Vector3f m_cannonEntryOfs;
    EGG::Vector3f m_cannonOrthog;
    EGG::Vector3f m_cannonProgress;
    f32 m_hopVelY;    ///< Relative velocity due to a hop. Starts at 10 and decreases with gravity.
    f32 m_hopPosY;    ///< Relative position as the result of a hop. Starts at 0.
    f32 m_hopGravity; ///< Always main gravity (-1.3f).
    DrivingDirection m_drivingDirection; ///< Current state of driver's direction.
    s16 m_backwardsAllowCounter;         ///< Tracks the 15f delay before reversing.
    bool m_bLaunchBoost;
    bool m_bPadBoost; ///< Caches whether the player is currently interacting with a boost pad.
    bool m_bRampBoost;
    bool m_bPadJump;
    bool m_bSsmtCharged;      ///< Set after holding a stand-still mini-turbo for 75 frames.
    bool m_bSsmtLeeway;       ///< If set, activates SSMT when not pressing A or B.
    bool m_bTrickableSurface; ///< Set when driving on a trickable surface.
    bool m_bWallBounce;       ///< Set when our speed loss from wall collision is > 30.0f.
    KartJump *m_jump;
    const DriftingParameters *m_driftingParams; ///< Drift-type-specific parameters.
    f32 m_rawTurn; ///< Float in range [-1, 1]. Represents stick magnitude + direction.
};

/// @brief Responsible for reacting to player inputs and moving the bike.
/// @details This derived class has specialized behavior for bikes, such as wheelies and leaning.
/// There are also additional member variables to track the bike's unique state.
/// @nosubgrouping
class KartMoveBike : public KartMove {
public:
    /// @brief Represents turning information which differs only between inside/outside drift.
    struct TurningParameters {
        f32 leanRotShallowFactor;
        f32 leanRotIncRace;
        f32 leanRotCapRace;
        f32 driftStickXFactor;
        f32 leanRotMaxDrift;
        f32 leanRotMinDrift;
        f32 leanRotIncCountdown;
        f32 leanRotCapCountdown;
        f32 leanRotIncSSMT;
        f32 leanRotCapSSMT;
        f32 leanRotDecayFactor;
        u16 maxWheelieFrames;
    };

    KartMoveBike();
    ~KartMoveBike();

    virtual void startWheelie();
    virtual void cancelWheelie();

    void createSubsystems() override;
    void calcVehicleRotation(f32 /*turn*/) override;
    void calcWheelie() override;
    void onHop() override;
    void onWallCollision() override;
    void calcMtCharge() override;
    void initOob() override;
    void setTurnParams() override;
    void init(bool b1, bool b2) override;
    [[nodiscard]] f32 getWheelieSoftSpeedLimitBonus() const override;
    [[nodiscard]] f32 wheelieRotFactor() const;

    void tryStartWheelie();

    /// @beginGetters
    [[nodiscard]] f32 leanRot() const override;
    [[nodiscard]] bool canWheelie() const override;
    /// @endGetters

private:
    f32 m_leanRot;         ///< Z-axis rotation of the bike from leaning.
    f32 m_leanRotCap;      ///< The maximum leaning rotation.
    f32 m_leanRotInc;      ///< The incrementor for leaning rotation.
    f32 m_wheelieRot;      ///< X-axis rotation from wheeling.
    f32 m_maxWheelieRot;   ///< The maximum wheelie rotation.
    u32 m_wheelieFrames;   ///< Tracks wheelie duration and cancels the wheelie after 180 frames.
    s16 m_wheelieCooldown; ///< The number of frames before another wheelie can start.
    f32 m_wheelieRotDec;   ///< The wheelie rotation decrementor, used after a wheelie has ended.
    const TurningParameters *m_turningParams; ///< Inside/outside drifting bike turn info.
};

} // namespace Kart
