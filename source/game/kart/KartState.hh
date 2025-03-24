#pragma once

#include "game/kart/KartObjectProxy.hh"

namespace Kart {

/// @brief Houses various flags and other variables to preserve the kart's state.
/// @details Most notably, this class is the direct observer of the input state,
/// and sets the appropriate flags for KartMove to act upon the input state.
/// This class also is responsible for managing calculations of the start boost duration.
/// @nosubgrouping
class KartState : KartObjectProxy {
public:
    KartState();

    void init();
    void reset();

    void calcInput();
    void calc();
    void resetFlags();
    void calcCollisions();
    void calcStartBoost();
    void calcHandleStartBoost();
    void handleStartBoost(size_t idx);
    void resetEjection();

    /// @beginSetters
    void setCannonPointId(u16 val) {
        m_cannonPointId = val;
    }

    void setBoostRampType(s32 val) {
        m_boostRampType = val;
    }

    void setJumpPadVariant(s32 val) {
        m_jumpPadVariant = val;
    }

    void setHalfPipeInvisibilityTimer(s16 val) {
        m_halfPipeInvisibilityTimer = val;
    }

    void setTrickableTimer(s16 val) {
        m_trickableTimer = val;
    }
    /// @endSetters

    /// @beginGetters
    [[nodiscard]] bool isDrifting() const {
        return m_status.onBit(eStatus::DriftManual, eStatus::DriftAuto);
    }

    [[nodiscard]] u16 cannonPointId() const {
        return m_cannonPointId;
    }

    [[nodiscard]] s32 boostRampType() const {
        return m_boostRampType;
    }

    [[nodiscard]] s32 jumpPadVariant() const {
        return m_jumpPadVariant;
    }

    [[nodiscard]] f32 stickX() const {
        return m_stickX;
    }

    [[nodiscard]] f32 stickY() const {
        return m_stickY;
    }

    [[nodiscard]] u32 airtime() const {
        return m_airtime;
    }

    [[nodiscard]] const EGG::Vector3f &top() const {
        return m_top;
    }

    [[nodiscard]] const EGG::Vector3f &softWallSpeed() const {
        return m_softWallSpeed;
    }

    [[nodiscard]] f32 startBoostCharge() const {
        return m_startBoostCharge;
    }

    [[nodiscard]] s16 wallBonkTimer() const {
        return m_wallBonkTimer;
    }

    [[nodiscard]] s16 trickableTimer() const {
        return m_trickableTimer;
    }

    [[nodiscard]] Status &status() {
        return m_status;
    }

    [[nodiscard]] const Status &status() const {
        return m_status;
    }

    /// @endGetters

private:
    Status m_status;
    u32 m_airtime;
    EGG::Vector3f m_top;
    EGG::Vector3f m_softWallSpeed;
    s32 m_hwgTimer;
    u16 m_cannonPointId;
    s32 m_boostRampType;
    s32 m_jumpPadVariant;
    s16 m_halfPipeInvisibilityTimer;
    f32 m_stickX;           ///< One of 15 discrete stick values from [-1.0, 1.0].
    f32 m_stickY;           ///< One of 15 discrete stick values from [-1.0, 1.0].
    f32 m_startBoostCharge; ///< 0-1 representation of start boost charge. Burnout if >0.95f.
    size_t m_startBoostIdx; ///< Used to map @ref m_startBoostCharge to a start boost duration.
    s16 m_wallBonkTimer;    ///< 2f counter that stunts your speed after hitting a wall. @rename
    s16 m_trickableTimer;
};

} // namespace Kart
