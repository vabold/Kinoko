#pragma once

#include "game/field/obj/ObjectKCL.hh"

namespace Kinoko::Field {

/// @brief Represents the bulldozers at the end of Toad's Factory.
/// @details Their oscillation is represented as a cosine wave, with two resting periods.
class ObjectBulldozer final : public ObjectKCL {
public:
    ObjectBulldozer(const System::MapdataGeoObj &params);
    ~ObjectBulldozer() override;

    void calc() override;
    void initCollision() override;
    [[nodiscard]] LoadFlags loadFlags() const override;
    [[nodiscard]] f32 colRadiusAdditionalLength() const override;
    [[nodiscard]] const EGG::Matrix34f &getUpdatedMatrix(u32 timeOffset) override;

    [[nodiscard]] f32 calcPosOffset(u32 t) const;

    const EGG::Vector3f m_initialPos; ///< Position of the bulldozer when the object is created
    const EGG::Vector3f m_initialRot; ///< Rotation of the bulldozer when the object is created
    const u16 m_timeOffset;           ///< Time offset of the bulldozer's cosine wave, in frames
    const u16 m_period;               ///< Period of oscillation in frames, excluding rest frames
    const u16 m_restFrames;           ///< How long bulldozers stop moving for
    const u16 m_fullPeriod;           ///< m_period + 2 * m_restFrames
    const u16 m_amplitude;  ///< Max displacement of the bulldozer from its initial position
    const bool m_left;      ///< Flips the cosine wave direction
    const f32 m_angFreq;    ///< Angular frequency, in radians per frame
    const u16 m_halfPeriod; ///< Half of @ref m_fullPeriod
    EGG::Matrix34f m_rtMat; ///< Exists solely so getUpdatedMatrix can return a reference
};

} // namespace Kinoko::Field
