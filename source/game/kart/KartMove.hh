#pragma once

#include "game/kart/KartBoost.hh"
#include "game/kart/KartBurnout.hh"
#include "game/kart/KartHalfPipe.hh"
#include "game/kart/KartObjectProxy.hh"
#include "game/kart/KartReject.hh"
#include "game/kart/KartState.hh"

#include "game/field/CourseColMgr.hh"

#include <egg/core/BitFlag.hh>

namespace Kart {

/// @brief Responsible for reacting to player inputs and moving the kart.
/// @nosubgrouping
class KartMove : protected KartObjectProxy {
public:
    enum class ePadType {
        BoostPanel = 0,
        BoostRamp = 1,
        JumpPad = 2,
    };

    typedef EGG::TBitFlag<u32, ePadType> PadType;

    KartMove();
    virtual ~KartMove();

    virtual void createSubsystems();
    virtual void calcTurn();
    virtual void calcWheelie() {}
    virtual void setTurnParams();
    virtual void init(bool b1, bool b2);
    virtual void clear();

    /// @addr{0x8058974C}
    [[nodiscard]] virtual f32 leanRot() const {
        return 0.0f;
    }

    void setInitialPhysicsValues(const EGG::Vector3f &position, const EGG::Vector3f &angles);
    void resetDriftManual();

    void calc();
    void calcRespawnStart();
    void calcInRespawn();
    void calcRespawnBoost();
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
    void calcAutoDrift();
    void calcManualDrift();
    void startManualDrift();
    void clearDrift();
    void clearJumpPad();
    void clearRampBoost();
    void clearZipperBoost();
    void clearBoost();
    void clearSsmt();
    void clearOffroadInvincibility();
    void clearRejectRoad();
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
    void calcRejectRoad();
    bool calcZipperCollision(f32 radius, f32 scale, EGG::Vector3f &pos, EGG::Vector3f &upLocal,
            const EGG::Vector3f &prevPos, Field::CollisionInfo *colInfo,
            Field::KCLTypeMask *maskOut, Field::KCLTypeMask flags) const;
    f32 calcSlerpRate(f32 scale, const EGG::Quatf &from, const EGG::Quatf &to) const;
    void applyForce(f32 force, const EGG::Vector3f &hitDir, bool stop);
    virtual void calcVehicleRotation(f32 turn);
    virtual void hop();
    virtual void onHop() {}
    virtual void onWallCollision() {}
    virtual void calcMtCharge();
    virtual void initOob();

    /// @stage 2
    /// @brief Returns the % speed boost from wheelies. For karts, this is always 0.
    /// @addr{0x8057C3C8}
    [[nodiscard]] virtual f32 getWheelieSoftSpeedLimitBonus() const {
        return 0.0f;
    }

    /// @addr{0x8058758C}
    virtual bool canWheelie() const {
        return false;
    }

    /// @addr{0x8057DA18}
    virtual bool canHop() const {
        if (!state()->isHopStart() || !state()->isTouchingGround()) {
            return false;
        }

        if (state()->isInAction()) {
            return false;
        }

        return true;
    }

    /// @addr{0x8057EA94}
    /// @details This doesn't bytematch the base game. The base game's implementation uses the >
    /// comparison. In practice, > is only ever used once, whereas >= is used in every other
    /// scenario, so we simplify by writing this function for the more common scenario.
    bool canStartDrift() const {
        return m_speed >= MINIMUM_DRIFT_THRESOLD * m_baseSpeed;
    }

    void tryStartBoostPanel();
    void tryStartBoostRamp();
    void tryStartJumpPad();
    void tryEndJumpPad();
    void cancelJumpPad();

    void activateBoost(KartBoost::Type type, s16 frames);
    void applyStartBoost(s16 frames);
    void activateMushroom();
    void activateZipperBoost();
    void setOffroadInvincibility(s16 timer);
    void calcOffroadInvincibility();
    void calcMushroomBoost();
    void calcZipperBoost();
    void landTrick();
    void activateCrush(u16 timer);
    void calcCrushed();
    void calcScale();
    void applyBumpForce(f32 speed, const EGG::Vector3f &hitDir, bool resetSpeed);

    void enterCannon();
    void calcCannon();
    void calcRotCannon(const EGG::Vector3f &forward);
    void exitCannon();

    void triggerRespawn();

    /// @beginSetters
    void setSpeed(f32 val) {
        m_speed = val;
    }

    void setSmoothedUp(const EGG::Vector3f &v) {
        m_smoothedUp = v;
    }

    void setUp(const EGG::Vector3f &v) {
        m_up = v;
    }

    void setDir(const EGG::Vector3f &v) {
        m_dir = v;
    }

    void setVel1Dir(const EGG::Vector3f &v) {
        m_vel1Dir = v;
    }

    void setFloorCollisionCount(u16 count) {
        m_floorCollisionCount = count;
    }

    void setKCLWheelSpeedFactor(f32 val) {
        m_kclWheelSpeedFactor = val;
    }

    void setKCLWheelRotFactor(f32 val) {
        m_kclWheelRotFactor = val;
    }

    /// @addr{0x8057B9AC}
    void setKartSpeedLimit() {
        constexpr f32 LIMIT = 120.0f;
        m_hardSpeedLimit = LIMIT;
    }

    /// @addr{0x80581720}
    void setScale(const EGG::Vector3f &v) {
        m_scale = v;
    }

    void setPadType(PadType type) {
        m_padType = type;
    }
    /// @endSetters

    /// @beginGetters

    /// @brief Factors in vehicle speed to retrieve our hop direction and magnitude.
    /// @addr{0x8057EFF8}
    /// @return 0.0f if we are too slow to drift, otherwise the hop direction.
    [[nodiscard]] s32 getAppliedHopStickX() const {
        return canStartDrift() ? m_hopStickX : 0;
    }

    [[nodiscard]] f32 softSpeedLimit() const {
        return m_softSpeedLimit;
    }

    [[nodiscard]] f32 speed() const {
        return m_speed;
    }

    [[nodiscard]] f32 acceleration() const {
        return m_acceleration;
    }

    [[nodiscard]] const EGG::Vector3f &scale() const {
        return m_scale;
    }

    [[nodiscard]] f32 hardSpeedLimit() const {
        return m_hardSpeedLimit;
    }

    [[nodiscard]] const EGG::Vector3f &smoothedUp() const {
        return m_smoothedUp;
    }

    [[nodiscard]] const EGG::Vector3f &up() const {
        return m_up;
    }

    [[nodiscard]] f32 totalScale() const {
        return m_totalScale;
    }

    [[nodiscard]] f32 hitboxScale() const {
        return m_hitboxScale;
    }

    [[nodiscard]] const EGG::Vector3f &dir() const {
        return m_dir;
    }

    [[nodiscard]] const EGG::Vector3f &lastDir() const {
        return m_lastDir;
    }

    [[nodiscard]] const EGG::Vector3f &vel1Dir() const {
        return m_vel1Dir;
    }

    [[nodiscard]] f32 speedRatioCapped() const {
        return m_speedRatioCapped;
    }

    [[nodiscard]] f32 speedRatio() const {
        return m_speedRatio;
    }

    [[nodiscard]] u16 floorCollisionCount() const {
        return m_floorCollisionCount;
    }

    [[nodiscard]] s32 hopStickX() const {
        return m_hopStickX;
    }

    [[nodiscard]] f32 hopPosY() const {
        return m_hopPosY;
    }

    [[nodiscard]] s16 respawnTimer() const {
        return m_respawnTimer;
    }

    [[nodiscard]] s16 respawnPostLandTimer() const {
        return m_respawnPostLandTimer;
    }

    [[nodiscard]] PadType &padType() {
        return m_padType;
    }

    [[nodiscard]] KartJump *jump() const {
        return m_jump;
    }

    [[nodiscard]] KartHalfPipe *halfPipe() const {
        return m_halfPipe;
    }

    [[nodiscard]] KartScale *kartScale() const {
        return m_kartScale;
    }

    [[nodiscard]] KartBurnout &burnout() {
        return m_burnout;
    }
    /// @endGetters

protected:
    enum class eFlags {
        Respawned = 0,   ///< Set when Lakitu lets go of the player, cleared when landing.
        DriftReset = 1,  ///< Set when a wall bonk should cancel your drift.
        SsmtCharged = 2, ///< Set after holding a stand-still mini-turbo for 75 frames.
        LaunchBoost = 4,
        SsmtLeeway = 5,       ///< If set, activates SSMT when not pressing A or B.
        TrickableSurface = 6, ///< Set when driving on a trickable surface.
        WallBounce = 8,       ///< Set when our speed loss from wall collision is > 30.0f.
    };
    typedef EGG::TBitFlag<u16, eFlags> Flags;

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
    f32 m_speedRatioCapped;              ///< @ref m_speedRatio but capped at 1.0f.
    f32 m_speedRatio;     ///< The ratio between current speed and the player's base speed stat.
    f32 m_kclSpeedFactor; ///< Float between 0-1 that scales the player's speed on offroad.
    f32 m_kclRotFactor;   ///< Float between 0-1 that scales the player's turning radius on offroad.
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
    s16 m_zipperBoostTimer;
    s16 m_zipperBoostMax;
    KartReject m_reject;
    s16 m_offroadInvincibility;  ///< How many frames until the player is affected by offroad.
    s16 m_ssmtCharge;            ///< Increments every frame up to 75 when charging stand-still MT.
    s16 m_ssmtLeewayTimer;       ///< Frames to forgive letting go of A before clearing SSMT charge.
    s16 m_ssmtDisableAccelTimer; ///< Counter that tracks delay before starting to reverse.
    f32 m_realTurn; ///< The "true" turn magnitude. Equal to @ref m_weightedTurn unless drifting.
    f32 m_weightedTurn;    ///< Magnitude+direction of stick input, factoring in the kart's stats.
    EGG::Vector3f m_scale; ///< Normally the unit vector, but may vary due to crush animations.
    f32 m_totalScale;      ///< @unused Always 1.0f
    f32 m_hitboxScale;
    u16 m_mushroomBoostTimer; ///< Number of frames until the mushroom boost runs out.
    u16 m_crushTimer;         ///< Number of frames until player will be uncrushed.
    u32 m_nonZipperAirtime;
    f32 m_jumpPadMinSpeed; ///< Snaps the player to a minimum speed when first touching a jump pad.
    f32 m_jumpPadMaxSpeed;
    f32 m_jumpPadBoostMultiplier;
    f32 m_jumpPadSoftSpeedLimit;
    const JumpPadProperties *m_jumpPadProperties;
    u16 m_rampBoost;
    f32 m_autoDriftAngle;
    s16 m_autoDriftStartFrameCounter;
    f32 m_cannonEntryOfsLength;
    EGG::Vector3f m_cannonEntryPos;
    EGG::Vector3f m_cannonEntryOfs;
    EGG::Vector3f m_cannonOrthog;
    EGG::Vector3f m_cannonProgress;
    f32 m_hopVelY;    ///< Relative velocity due to a hop. Starts at 10 and decreases with gravity.
    f32 m_hopPosY;    ///< Relative position as the result of a hop. Starts at 0.
    f32 m_hopGravity; ///< Always main gravity (-1.3f).
    s16 m_timeInRespawn;        ///< The number of frames elapsed after position snap from respawn.
    s16 m_respawnPreLandTimer;  ///< Counts down from 4 when pressing A before landing from respawn.
    s16 m_respawnPostLandTimer; ///< Counts up to 4 if not accelerating after respawn landing.
    s16 m_respawnTimer;
    s16 m_bumpTimer;                     ///< Set when a @ref Reaction::SmallBump collision occurs.
    DrivingDirection m_drivingDirection; ///< Current state of driver's direction.
    s16 m_backwardsAllowCounter;         ///< Tracks the 15f delay before reversing.
    PadType m_padType;
    Flags m_flags;
    KartJump *m_jump;
    KartHalfPipe *m_halfPipe;                   ///< Pertains to zipper physics.
    KartScale *m_kartScale;                     ///< Manages scaling due to TF stompers and MH cars.
    KartBurnout m_burnout;                      ///< Manages the state of start boost burnout.
    const DriftingParameters *m_driftingParams; ///< Drift-type-specific parameters.
    f32 m_rawTurn; ///< Float in range [-1, 1]. Represents stick magnitude + direction.

    static constexpr f32 MINIMUM_DRIFT_THRESOLD = 0.55f;
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
    void clear() override;

    /// @stage 2
    /// @brief Returns what % to raise the speed cap when wheeling.
    /// @addr{0x80588324}
    [[nodiscard]] f32 getWheelieSoftSpeedLimitBonus() const override {
        constexpr f32 WHEELIE_SPEED_BONUS = 0.15f;
        return state()->isWheelie() ? WHEELIE_SPEED_BONUS : 0.0f;
    }

    /// @addr{0x80588860}
    [[nodiscard]] f32 wheelieRotFactor() const {
        constexpr f32 WHEELIE_ROTATION_FACTOR = 0.2f;

        return state()->isWheelie() ? WHEELIE_ROTATION_FACTOR : 1.0f;
    }

    void tryStartWheelie();

    /// @beginGetters
    /// @addr{0x805896BC}
    [[nodiscard]] f32 leanRot() const override {
        return m_leanRot;
    }

    /// @brief Checks if the kart is going fast enough to wheelie.
    /// @addr{0x80588FE0}
    [[nodiscard]] bool canWheelie() const override {
        constexpr f32 WHEELIE_THRESHOLD = 0.3f;

        return m_speedRatioCapped >= WHEELIE_THRESHOLD && m_speed >= 0.0f;
    }

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
    s16 m_autoHardStickXFrames;
    const TurningParameters *m_turningParams; ///< Inside/outside drifting bike turn info.
};

} // namespace Kart
