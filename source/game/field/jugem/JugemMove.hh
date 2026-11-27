#pragma once

#include <egg/math/Matrix.hh>

namespace Kinoko {

namespace Kart {

class KartObject;

} // namespace Kart

namespace Field {

/// @brief Manages state information related to Lakitu movement
class JugemMove {
public:
    JugemMove(const Kart::KartObject *kartObj);
    ~JugemMove();

    void init();
    void calc();
    void calcForwardFromKartObjPosDelta(bool setCurr);
    void calcForwardFromKartObjMainRot(bool setCurr);

    /// @beginSetters

    /// @addr{0x8071EFA8}
    /// @brief Sets Lakitu's position and optionally sets the transformed position
    /// @param pos The new position to set
    /// @param transPos If `true`, also sets the transformed position to the same value
    void setPos(const EGG::Vector3f &pos, bool transPos) {
        m_pos = pos;

        if (transPos) {
            m_transPos = pos;
        }
    }

    /// @addr{0x8071EFD8}
    /// @brief Sets the anchor position that Lakitu is trying to move towards
    /// @param v The new anchor position to set
    void setAnchorPos(const EGG::Vector3f &v) {
        m_anchorPos = v;
    }

    /// @brief Sets Lakitu's velocity when rising back up
    /// @param v The rising velocity to set
    void setRiseVel(const EGG::Vector3f &v) {
        m_riseVel = v;
    }

    /// @brief Sets whether Lakitu is either away or descending
    /// @param isSet `true` if Lakitu is away or descending, `false` otherwise
    void setAwayOrDescending(bool isSet) {
        m_isAwayOrDescending = isSet;
    }

    /// @brief Sets whether Lakitu is rising up into the sky
    /// @param isSet `true` if Lakitu is rising, `false` otherwise
    void setRising(bool isSet) {
        m_isRising = isSet;
    }

    /// @endSetters

    /// @beginGetters

    /// @brief Returns Lakitu's transformed position
    /// @return The transformed position of Lakitu in world space, @ref m_transPos
    [[nodiscard]] const EGG::Vector3f &transPos() const {
        return m_transPos;
    }

    /// @brief Returns Lakitu's transformation matrix
    /// @return The combined lean and rotation matrices representing Lakitu's orientation, @ref
    /// m_transform
    [[nodiscard]] const EGG::Matrix34f &transform() const {
        return m_transform;
    }

    /// @endGetters

private:
    EGG::Matrix34f calcOrthonormalBasis();
    EGG::Vector3f calcOscillation(const EGG::Matrix34f &mat);
    EGG::Matrix34f calcLeanBasis();

    const Kart::KartObject *m_kartObj; ///< Pointer to the kart object that Lakitu is following
    EGG::Vector3f m_pos;               ///< Lakitu's position in world space
    EGG::Vector3f m_transPos;   ///< Lakitu's world position after applying the oscillation offset
    EGG::Vector3f m_anchorPos;  ///< The position that Lakitu is trying to move towards
    EGG::Matrix34f m_leanMat;   ///< Tilts the up vector based on movement direction
    EGG::Matrix34f m_rtMat;     ///< Describes the Lakitu's orientation
    EGG::Matrix34f m_transform; ///< Combines the lean and rotation matrices
    f32 m_phaseX;               ///< Left/right oscillation phase
    f32 m_phaseY;               ///< Up/down oscillation phase
    EGG::Vector3f m_lastKartObjPos; ///< Position of the kart on the previous frame
    EGG::Vector3f m_vel;            ///< Velocity of Lakitu's movement
    EGG::Vector3f m_riseVel;        ///< Velocity of Lakitu once he leaves by rising upwards
    EGG::Vector3f m_smoothedVel;    ///< Smoothed @ref m_vel, used to compute Lakitu's lean
    EGG::Vector3f m_currForward;    ///< Lakitu's current facing direction
    EGG::Vector3f m_targetForward;  ///< Lakitu's target facing direction
    f32 m_forwardInterpRate;        ///< Interpolation rate for Lakitu's smoothed facing direction
    bool m_isAwayOrDescending;      ///< `true` if Lakitu is inactive or is descending from the sky
    f32 m_velInterpRate;            ///< Interpolation rate for Lakitu's smoothed velocity
    bool m_isRising;                ///< `true` if Lakitu is rising up into the sky (despawning)
};

} // namespace Field

} // namespace Kinoko
