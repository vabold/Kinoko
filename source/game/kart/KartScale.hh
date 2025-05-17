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

    void startCrush();
    void startUncrush();

    [[nodiscard]] const EGG::Vector3f &currScale() const {
        return m_currScale;
    }

private:
    enum class CrushState {
        None = -1,
        Crush = 0,
        Uncrush = 1,
    };

    void calcCrush();

    [[nodiscard]] EGG::Vector3f getAnmScale(f32 frame) const;

    CrushState m_crushState;   ///< Specifies the current crush/uncrush state
    bool m_calcCrush;          ///< Set while crush scaling is occurring
    f32 m_uncrushAnmFrame;     ///< Current frame of the unsquish animation
    EGG::Vector3f m_currScale; ///< The computed scale for the current frame

    static constexpr f32 CRUSH_SCALE = 0.3f;
};

} // namespace Kart
