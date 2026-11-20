#pragma once

#include "game/field/obj/ObjectKCL.hh"

namespace Kinoko::Field {

/// @brief Distinguishes between light and dark variants of a mushroom object
enum class KinokoType : u16 {
    Light = 0, ///< Light mushrooms outside of the cave
    Dark = 1,  ///< Dark mushrooms inside of the cave
};

/// @brief The base class for a mushroom object with jump pad properties
/// @details Implements a "pulse" scaling effect. Also defines a virtual oscillation function so
/// that derived classes can have custom cyclic behavior.
class ObjectKinoko : public ObjectKCL {
public:
    ObjectKinoko(const System::MapdataGeoObj &params);
    ~ObjectKinoko() override;

    void calc() override;
    [[nodiscard]] LoadFlags loadFlags() const override;
    [[nodiscard]] const char *getKclName() const override;

    /// @brief Updates the mushroom's position or rotation for its oscillation
    virtual void calcOscillation() = 0;

protected:
    const KinokoType m_type;       ///< The variant of mushroom object (light or dark)
    const EGG::Vector3f m_initPos; ///< Initial position of the mushroom
    const EGG::Vector3f m_initRot; ///< Initial rotation of the mushroom
    s16 m_pulseFrame;              ///< Framecount used for the pulse sine wave
    s16 m_restFrame;               ///< Number of frames the mushroom has been idling for
    f32 m_pulseAmplitude;          ///< Amplitude of the pulse sine wave, decaying over the pulse
};

/// @brief Mushrooms which oscillate up and down. The stem does not move.
/// @details This represents the first two mushrooms on Mushroom Gorge, even though they don't
/// oscillate up or down.
class ObjectKinokoUd final : public ObjectKinoko {
public:
    ObjectKinokoUd(const System::MapdataGeoObj &params);
    ~ObjectKinokoUd() override;

    [[nodiscard]] const char *getKclName() const override;

    void calcScale(u32 timeOffset) override;

    void calcOscillation() override;

private:
    u16 m_oscFrame;     ///< Current frame of the oscillation
    u16 m_waitFrame;    ///< How long the mushroom has paused in the middle of its oscillation cycle
    const s16 m_period; ///< Total oscillation period
    const s16 m_waitDuration; ///< How long to pause at its lowest point in the cycle
    const s16 m_amplitude;    ///< Maximum vertical displacement above the initial Y position
    const f32 m_angFreq;      ///< Angular frequency of the oscillation cosine wave
};

/// @brief Mushrooms which bend in a certain direction
/// @details Several red mushrooms throughout Mushroom Gorge are instances of this class, but the
/// bending functionality is not used in the base game.
class ObjectKinokoBend final : public ObjectKinoko {
public:
    ObjectKinokoBend(const System::MapdataGeoObj &params);
    ~ObjectKinokoBend() override;

    void calcScale(u32 timeOffset) override;

    void calcOscillation() override;

private:
    s16 m_currentFrame;    ///< Current frame within the bend animation cycle
    const s16 m_period;    ///< Total framecount of the animation cycle
    const f32 m_amplitude; ///< Max bend angle in radians
    const f32 m_angFreq;   ///< Angular frequency of the bend sine wave
};

/// @brief Green mushrooms with normal road properties, for the most part
class ObjectKinokoNm final : public ObjectKCL {
public:
    ObjectKinokoNm(const System::MapdataGeoObj &params);
    ~ObjectKinokoNm() override;

    [[nodiscard]] const char *getKclName() const override;

private:
    const KinokoType m_type; ///< The variant of mushroom object (light or dark)
};

} // namespace Kinoko::Field
