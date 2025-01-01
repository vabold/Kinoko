#pragma once

#include "game/kart/KartObjectProxy.hh"

#include "game/system/KPadController.hh"

namespace Kart {

/// @brief Handles the physics and boosts associated with zippers.
/// @nosubgrouping
class KartHalfPipe : public KartObjectProxy {
public:
    KartHalfPipe();
    ~KartHalfPipe();

    void reset();
    void calc();
    void calcTrick();
    void calcRot();
    void calcLanding(bool);
    void activateTrick(s32 duration, System::Trick trick);
    void end(bool boost);

    /// @addr{0x80574108}
    [[nodiscard]] static consteval f32 TerminalVelocity() {
        return 65.0f;
    }

private:
    enum class StuntType {
        None = -1,
        Backflip = 0,
        Frontflip = 1,
        Side360 = 2,
        Backside = 3,
        Frontside = 4,
        Side720 = 5,
    };

    /// @brief Angle properties corresponding with the stunts
    struct StuntProperties {
        f32 angleDelta;
        f32 angleDeltaMin;
        f32 angleDeltaFactorMin;
        f32 angleDeltaFactorDecr;
        f32 finalAngleScalar;
        f32 finalAngle;
    };

    struct StuntManager {
        void calcAngle();
        void setProperties(size_t idx);

        f32 angle;
        f32 angleDelta;
        f32 angleDeltaFactor;
        f32 angleDeltaFactorDecr;
        f32 finalAngle;
        StuntProperties properties;
    };

    bool m_touchingZipper;
    s16 m_timer;
    f32 m_nextSign;
    s32 m_attemptedTrickTimer; ///< When attempting a trick, tracks how long the animation would be.
    EGG::Quatf m_rot;
    EGG::Vector3f m_prevPos;
    StuntType m_stunt;
    f32 m_rotSign;
    s16 m_nextTimer;
    System::Trick m_trick;
    EGG::Quatf m_stuntRot;
    StuntManager m_stuntManager;
};

} // namespace Kart
