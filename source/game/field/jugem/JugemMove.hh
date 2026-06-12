#pragma once

#include "game/kart/KartObject.hh"

namespace Kinoko::Field {

/// @brief Manages state information related to Lakitu movement.
class JugemMove {
public:
    JugemMove(const Kart::KartObject *kartObj);
    ~JugemMove();

    void init();
    void calc();

    /// @addr{0x8071EFA8}
    void setPos(const EGG::Vector3f &pos, bool transPos) {
        m_pos = pos;

        if (transPos) {
            m_transPos = pos;
        }
    }

    void setForwardFromKartObjPosDelta(bool set124);
    void setForwardFromKartObjMainRot(bool set124);

    /// @addr{0x8071EFD8}
    void setAnchorPos(const EGG::Vector3f &v) {
        m_anchorPos = v;
    }

    void setRiseVel(const EGG::Vector3f &v) {
        m_riseVel = v;
    }

    void setAwayOrDescending(bool isSet) {
        m_isAwayOrDescending = isSet;
    }

    void setDescending(bool isSet) {
        m_isDescending = isSet;
    }

    void setRising(bool isSet) {
        m_isRising = isSet;
    }

    const EGG::Vector3f &transPos() const {
        return m_transPos;
    }

    const EGG::Matrix34f &transform() const {
        return m_transform;
    }

private:
    EGG::Matrix34f calcOrthonormalBasis();
    EGG::Vector3f calcOscillation(const EGG::Matrix34f &mat);
    [[nodiscard]] EGG::Matrix34f FUN_807202BC();

    [[nodiscard]] static EGG::Vector3f Interpolate(f32 t, const EGG::Vector3f &v0,
            const EGG::Vector3f &v1) {
        return v0 + (v1 - v0) * t;
    }

    const Kart::KartObject *m_kartObj;
    EGG::Vector3f m_pos;
    EGG::Vector3f m_transPos;
    EGG::Vector3f m_anchorPos;
    EGG::Matrix34f m_28;
    EGG::Matrix34f m_58;
    EGG::Matrix34f m_88;
    EGG::Matrix34f m_transform;
    f32 m_phaseX; ///< Left/right oscillation phase
    f32 m_phaseY; ///< Up/down oscillation phase
    EGG::Vector3f m_lastKartObjPos;
    EGG::Vector3f m_velDir;
    EGG::Vector3f m_riseVel; ///< Velocity of Lakitu once he leaves by rising upwards
    EGG::Vector3f m_dir;
    EGG::Vector3f m_currForward;   ///< The current direction that Lakitu is at
    EGG::Vector3f m_targetForward; ///< The direction that Lakitu wants to move to
    f32 m_forwardInterpRate;
    bool m_isAwayOrDescending;
    f32 m_velDirInterpRate;
    bool m_isDescending;
    bool m_isRising;
};

} // namespace Kinoko::Field
