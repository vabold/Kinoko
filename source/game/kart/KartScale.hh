#pragma once

#include "game/kart/KartObjectProxy.hh"
#include "game/kart/KartParam.hh"

namespace Kart {

/// @brief Mainly responsible for calculating scaling for the squish/unsquish animation.
class KartScale : protected KartObjectProxy {
public:
    KartScale(const KartParam::Stats &stats);
    ~KartScale();

    void reset();
    void calc();

    void startCrush();
    void endCrush();
    void startShrink(s32 unk);
    void endShrink(s32 unk);

    [[nodiscard]] const EGG::Vector3f &sizeScale() const {
        return m_sizeScale;
    }

    [[nodiscard]] const EGG::Vector3f &pressScale() const {
        return m_pressScale;
    }

private:
    enum class CrushState {
        None = -1,
        Crush = 0,
        Uncrush = 1,
    };

    void calcCrush();

    [[nodiscard]] EGG::Vector3f getAnmScale(f32 frame) const;

    s32 m_type;
    EGG::Vector3f m_scaleTransformOffset;
    EGG::Vector3f m_scaleTransformSlope;
    EGG::Vector3f m_sizeScale;
    bool m_scaleAnmActive;
    f32 m_anmFrame;
    std::array<f32, 4> m_scaleTarget;
    CrushState m_crushState; ///< Specifies the current crush/uncrush state
    bool m_calcCrush;        ///< Set while crush scaling is occurring
    f32 m_uncrushAnmFrame;   ///< Current frame of the unsquish animation
    EGG::Vector3f m_pressScale;

    static constexpr f32 CRUSH_SCALE = 0.3f;

    static constexpr std::array<f32, 4> s_baseScaleStart = {{
            0.5f,
            1.0f,
            1.0f,
            2.0f,
    }};

    static constexpr std::array<f32, 4> s_baseScaleTarget = {{
            1.0f,
            0.5f,
            2.0f,
            1.0f,
    }};
};

} // namespace Kart
