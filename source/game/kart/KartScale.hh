#pragma once

#include "game/kart/KartObjectProxy.hh"

namespace Kart {

/// @brief Mainly responsible for calculating scaling for the squish/unsquish animation.
class KartScale : protected KartObjectProxy {
public:
    KartScale();
    ~KartScale();

    void reset();
    void calc();

    void startPressDown();
    void startPressUp();

    [[nodiscard]] const EGG::Vector3f &currScale() const {
        return m_currScale;
    }

private:
    enum class PressState {
        None = -1,
        Down = 0,
        Up = 1,
    };

    void calcPress();

    [[nodiscard]] EGG::Vector3f getAnmScale(f32 frame) const;

    PressState m_pressState;   ///< Specifies squish/unsquish state
    bool m_calcPress;          ///< Set while crush scaling is occurring
    f32 m_pressUpAnmFrame;     ///< Current frame of the unsquish animation
    EGG::Vector3f m_currScale; ///< The computed scale for the current frame

    static constexpr f32 CRUSH_SCALE = 0.3f;
};

} // namespace Kart
