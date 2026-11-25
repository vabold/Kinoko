#pragma once

#include "game/field/obj/ObjectKCL.hh"

namespace Kinoko::Field {

/// @brief The oscillating platforms before and after the indoor section on Grumble Volcano
/// @details Uses a cosine wave to induce oscillatory motion along the Z and Y axes.
class ObjectVolcanoRock final : public ObjectKCL {
public:
    ObjectVolcanoRock(const System::MapdataGeoObj &params);
    ~ObjectVolcanoRock() override;

    void calc() override;
    [[nodiscard]] LoadFlags loadFlags() const override;
    [[nodiscard]] const char *getKclName() const override;

    [[nodiscard]] const EGG::Matrix34f &getUpdatedMatrix(u32 timeOffset) override;
    [[nodiscard]] f32 colRadiusAdditionalLength() const override;

private:
    [[nodiscard]] EGG::Vector3f calcPos(u32 frame);

    const EGG::Vector3f m_initialPos; ///< Initial position of the volcano rock
    const EGG::Vector3f m_initialRot; ///< Initial rotation of the volcano rock
    const s16 m_phaseShift;           ///< Framecount offset for Z-axis oscillation
    const s16 m_zPeriod;              ///< Framecount of the platform's movement period along Z-axis
    const s16 m_yPeriod;              ///< Framecount of the platform's movement period along Y-axis
    const f32 m_zAmplitude;           ///< Scalar applied to computed Z-axis position
    const f32 m_yAmplitude;           ///< Scalar applied to computed Y-axis position
    const f32 m_zAngVel;              ///< `2pi` / @ref m_zPeriod
    const f32 m_yAngVel;              ///< `2pi` / @ref m_yPeriod
    const bool m_variant;             ///< Differentiates which KCL model is used
    EGG::Matrix34f m_rtMat;           ///< Current frame's rotation and translation matrix
};

} // namespace Kinoko::Field
