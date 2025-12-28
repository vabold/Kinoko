#pragma once

#include "game/field/obj/ObjectKCL.hh"

#include "game/system/RaceManager.hh"

namespace Field {

/// @brief The oscillating platforms before and after the indoor section on Grumble Volcano.
class ObjectVolcanoRock final : public ObjectKCL {
public:
    ObjectVolcanoRock(const System::MapdataGeoObj &params);
    ~ObjectVolcanoRock() override;

    void calc() override;

    /// @addr{0x8081A688}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x8081A668}
    [[nodiscard]] const char *getKclName() const override {
        return m_variant ? "VolcanoRock2" : "VolcanoRock1";
    }

    /// @addr{0x8081A60C}
    [[nodiscard]] const EGG::Matrix34f &getUpdatedMatrix(u32 timeOffset) override {
        u32 t = System::RaceManager::Instance()->timer() - timeOffset;
        m_rtMat.makeRT(m_initialRot, calcPos(t));
        return m_rtMat;
    }

    /// @addr{0x8081A5D0}
    [[nodiscard]] f32 colRadiusAdditionalLength() const override {
        return 2000.0f + m_zAmplitude;
    }

private:
    EGG::Vector3f calcPos(s32 frame);

    const EGG::Vector3f m_initialPos;
    const EGG::Vector3f m_initialRot;
    const s16 m_phaseShift; ///< Additional framecount applied when calculating z-axis position
    const s16 m_zPeriod;    ///< Framecount of the platform's movement period along z-axis
    const s16 m_yPeriod;    ///< Framecount of the platform's movement period along y-axis
    const f32 m_zAmplitude; ///< Scalar applied to computed z-axis position
    const f32 m_yAmplitude; ///< Scalar applied to computed y-axis position
    const f32 m_zAngVel;    ///< 2pi / m_zPeriod
    const f32 m_yAngVel;    ///< 2pi / m_yPeriod
    const bool m_variant;   ///< Differentiates which KCL is used
    EGG::Matrix34f m_rtMat; ///< Rotation and translation matrix
};

} // namespace Field
