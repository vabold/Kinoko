#pragma once

#include "game/kart/KartObjectProxy.hh"
#include "game/system/KPadController.hh"

namespace Kart {

/// @brief Determined by the KCL, this represents the variation of the trick that will be performed.
/// @details This is also directly used to determine the duration of the trick boost.
enum class SurfaceVariant {
    DoubleFlipTrick = 0, ///< RR ramps before figure-8 and last turn.
    SingleFlipTrick = 1, ///< BC starting ramp.
    StuntTrick = 2,      ///< Think rMR ramp and pipe, MH after first turn, mushrooms, etc.
};

/// @brief Represents the type of trick that will be performed based on kart and player input.
/// @details Some of these types are only available for bikes.
enum class TrickType {
    StuntTrickBasic = 0,    ///< An up/down @ref StuntTrick with kart/bike.
    BikeFlipTrickNose = 1,  ///< An up trick with bike.
    BikeFlipTrickTail = 2,  ///< A down trick with bike.
    FlipTrickYLeft = 3,     ///< A left trick with kart/bike.
    FlipTrickYRight = 4,    ///< A right trick with kart/bike.
    KartFlipTrickZ = 5,     ///< An up/down trick with a kart.
    BikeSideStuntTrick = 6, ///< A side @ref StuntTrick with a bike.
};

/// @brief Manages trick inputs and state.
/// @nosubgrouping
class KartJump : protected KartObjectProxy {
public:
    struct TrickProperties {
        f32 initialAngleDiff;
        f32 angleDeltaMin;
        f32 angleDeltaFactorMin;
        f32 angleDiffMulDec;
    };

    struct AngleProperties {
        f32 targetAngle;
        f32 rotAngle;
    };

    KartJump(KartMove *move);
    virtual ~KartJump();

    virtual void calcRot();

    void setupProperties();
    void reset();
    void tryStart(const EGG::Vector3f &left);
    void calc();
    bool someFlagCheck();
    void calcInput();
    void end();

    void setAngle(const EGG::Vector3f &left);

    /// @beginSetters
    void setBoostRampEnabled(bool isSet) {
        m_boostRampEnabled = isSet;
    }
    /// @endSetters

    /// @beginGetters
    [[nodiscard]] bool isBoostRampEnabled() const {
        return m_boostRampEnabled;
    }

    [[nodiscard]] TrickType type() const {
        return m_type;
    }
    [[nodiscard]] SurfaceVariant variant() const {
        return m_variant;
    }
    [[nodiscard]] s16 cooldown() const {
        return m_cooldown;
    }
    /// @endGetters

protected:
    virtual void start(const EGG::Vector3f &left);
    virtual void init();

    TrickType m_type;
    SurfaceVariant m_variant;
    System::Trick m_nextTrick;
    f32 m_rotSign;
    TrickProperties m_properties;
    f32 m_angle;
    f32 m_angleDelta;
    f32 m_angleDeltaFactor;
    f32 m_angleDeltaFactorDec;
    f32 m_finalAngle;
    s16 m_cooldown;
    EGG::Quatf m_rot;
    KartMove *m_move;

private:
    s16 m_nextAllowTimer;
    bool m_boostRampEnabled;
};

class KartJumpBike : public KartJump {
public:
    KartJumpBike(KartMove *move);
    ~KartJumpBike();

    void calcRot() override;

private:
    void start(const EGG::Vector3f &left) override;
    void init() override;
};

} // namespace Kart
