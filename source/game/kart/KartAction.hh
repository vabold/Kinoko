#pragma once

#include "game/kart/KartObjectProxy.hh"

namespace Kart {

enum class Action {
    None = -1,
    UNK_0 = 0,
    UNK_1 = 1,
    UNK_2 = 2,
    UNK_3 = 3,
    UNK_4 = 4,
    UNK_5 = 5,
    UNK_6 = 6,
    UNK_7 = 7,
    UNK_8 = 8,
    UNK_9 = 9,
    UNK_12 = 12,
    UNK_14 = 14,
    UNK_15 = 15,
    UNK_16 = 16,
    Max = 18,
};

class KartAction : KartObjectProxy {
public:
    enum class eFlags {
        Landing = 0,
        Rotating = 3,
        LandingFromFlip = 5,
    };
    typedef EGG::TBitFlag<u32, eFlags> Flags;

    KartAction();
    ~KartAction();

    void init();
    void calc();
    void calcVehicleSpeed();
    bool start(Action action);
    void startRotation(size_t idx);

    void setHitDepth(const EGG::Vector3f &hitDepth) {
        m_hitDepth = hitDepth;
    }

    void setTranslation(const EGG::Vector3f &v) {
        m_translation = v;
    }

    [[nodiscard]] const Flags &flags() const {
        return m_flags;
    }

private:
    /// @brief Parameters specific to an action ID.
    struct ActionParams {
        f32 startSpeedMult;
        f32 calcSpeedMult;
        s16 priority;
    };

    struct RotationParams {
        f32 initialAngleIncrement;
        f32 minAngleIncrement;
        f32 minMultiplier;
        f32 initialMultiplierDecrement;
        f32 slowdownThreshold;
        f32 finalAngle;
    };

    // The player index sent into StartActionFunc is assumed to be cosmetic
    typedef void (KartAction::*StartActionFunc)();
    typedef bool (KartAction::*CalcActionFunc)();
    typedef void (KartAction::*EndActionFunc)(bool arg);

    void calcSideFromHitDepth();
    void calcSideFromHitDepthAndTranslation();

    void end();

    bool calcCurrentAction();
    void calcEndAction(bool endArg);
    bool calcRotation();
    void calcUp();
    void calcLanding();
    void startLaunch(f32 extVelScalar, f32 extVelKart, f32 extVelBike, f32 numRotations,
            u32 param6);
    void activateCrush(u16 timer);

    void applyStartSpeed();
    void setRotation(size_t idx);

    /* ================================ *
     *     START FUNCTIONS
     * ================================ */

    void startStub();
    void startAction1();
    void startAction2();
    void startAction3();
    void startAction5();
    void startLargeFlipAction();
    void startAction9();
    void startLongPressAction();
    void startShortPressAction();
    void startSpinShrinkAction();

    /* ================================ *
     *     CALC FUNCTIONS
     * ================================ */

    bool calcStub();
    bool calcAction1();
    bool calcLaunchAction();
    bool calcLargeFlipAction();
    bool calcPressAction();

    /* ================================ *
     *     END FUNCTIONS
     * ================================ */

    void endStub(bool arg);
    void endAction1(bool arg);
    void endLaunchAction(bool arg);

    EGG::Vector3f m_side;
    Action m_currentAction;
    f32 m_rotationDirection;
    f32 m_targetRot;
    EGG::Vector3f m_hitDepth;
    EGG::Vector3f m_rotAxis;
    EGG::Vector3f m_translation;

    f32 m_velPitch;
    f32 m_pitch;
    f32 m_deltaPitch;
    f32 m_flipPhase;

    StartActionFunc m_onStart;
    CalcActionFunc m_onCalc;
    EndActionFunc m_onEnd;

    EGG::Quatf m_rotation;
    const ActionParams *m_actionParams;
    u32 m_frame;
    u32 m_crushTimer;
    Flags m_flags;
    f32 m_currentAngle;
    f32 m_angleIncrement;
    f32 m_multiplier;
    f32 m_multiplierDecrement;
    f32 m_finalAngle;
    const RotationParams *m_rotationParams;
    EGG::Vector3f m_up;
    u16 m_framesFlipping;
    s16 m_priority;

    static constexpr size_t MAX_ACTION = static_cast<size_t>(Action::Max);

    static const std::array<ActionParams, MAX_ACTION> s_actionParams;
    static const std::array<RotationParams, 5> s_rotationParams;

    static const std::array<StartActionFunc, MAX_ACTION> s_onStart;
    static const std::array<CalcActionFunc, MAX_ACTION> s_onCalc;
    static const std::array<EndActionFunc, MAX_ACTION> s_onEnd;
};

} // namespace Kart
