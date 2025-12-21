#pragma once

#include "game/field/obj/ObjectKCL.hh"

namespace Field {

/// @brief The moving crane platforms after the second turn of Toad's Factory.
/// @details Moves in a simple sine wave. There is both an x-axis and y-axis wave, though the
/// platforms in the base game have a y amplitude of 0.
class ObjectCrane final : public ObjectKCL {
public:
    ObjectCrane(const System::MapdataGeoObj &params);
    ~ObjectCrane() override;

    void calc() override;

    /// @addr{0x807FEB20}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x807FEAF0}
    [[nodiscard]] f32 colRadiusAdditionalLength() const override {
        return m_xAmplitude;
    }

private:
    const EGG::Vector3f m_startPos; ///< Initial starting position
    s16 m_xt;                       ///< Current time along the x-axis period
    s16 m_yt;                       ///< Current time along the y-axis period
    const s16 m_xPeriod;            ///< Framecount of a full oscillation on x-axis
    const s16 m_yPeriod;            ///< Framecount of a full oscillation on y-axis
    const s16 m_xAmplitude;         ///< Max x-position delta from starting position
    const s16 m_yAmplitude;         ///< Max y-position delta from starting position
    const f32 m_xFreq;              ///< 2pi / m_xPeriod
    const f32 m_yFreq;              ///< 2pi / m_yPeriod
};

} // namespace Field
