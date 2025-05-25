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
    UNK_16 = 16,
    Max = 18,
};

class KartAction : KartObjectProxy {
public:
    enum class eFlags {
        Rotating = 3,
    };
    typedef EGG::TBitFlag<u32, eFlags> Flags;

    KartAction();
    ~KartAction();

    void init();
    void calc();
    void calcVehicleSpeed();
    bool start(Action action);
    void startRotation(size_t idx);

    void setHitDepth(const EGG::Vector3f &hitDepth);

    const Flags &flags() const;

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

    void end();

    bool calcCurrentAction();
    void calcEndAction(bool endArg);
    bool calcRotation();
    void calcUp();

    void applyStartSpeed();
    void setRotation(size_t idx);

    /* ================================ *
     *     START FUNCTIONS
     * ================================ */

    void startStub();
    void startAction1();
    void startAction9();

    /* ================================ *
     *     CALC FUNCTIONS
     * ================================ */

    bool calcStub();
    bool calcAction1();

    /* ================================ *
     *     END FUNCTIONS
     * ================================ */

    void endStub(bool arg);
    void endAction1(bool arg);

    Action m_currentAction;
    f32 m_rotationDirection;
    EGG::Vector3f m_hitDepth;

    StartActionFunc m_onStart;
    CalcActionFunc m_onCalc;
    EndActionFunc m_onEnd;

    EGG::Quatf m_rotation;
    const ActionParams *m_actionParams;
    u32 m_frame;
    Flags m_flags;
    f32 m_currentAngle;
    f32 m_angleIncrement;
    f32 m_multiplier;
    f32 m_multiplierDecrement;
    f32 m_finalAngle;
    const RotationParams *m_rotationParams;
    EGG::Vector3f m_up;
    s16 m_priority;

    static constexpr size_t MAX_ACTION = static_cast<size_t>(Action::Max);

    static const std::array<ActionParams, MAX_ACTION> s_actionParams;
    static const std::array<RotationParams, 5> s_rotationParams;

    static const std::array<StartActionFunc, MAX_ACTION> s_onStart;
    static const std::array<CalcActionFunc, MAX_ACTION> s_onCalc;
    static const std::array<EndActionFunc, MAX_ACTION> s_onEnd;
};

} // namespace Kart
