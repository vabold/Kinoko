#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

/// @brief Lava eruptions without any hump (@ref ObjectFlamePoleFoot) like on N64 Bowser's Castle
/// @details Flamepoles have an initial delay framecount defined by @ref m_initDelay, an overall
/// cycle duration of @ref m_cycleDuration, lie dormant for @ref m_dormantFrames, erupt for
/// @ref ERUPT_FRAMES, and fall for @ref FALL_FRAMES. Their maximum height varies depending on
/// whether this object's name is `FlamePole_v` or `FlamePole_v_big`.
class ObjectFlamePoleV final : public ObjectCollidable, private StateManager {
public:
    ObjectFlamePoleV(const System::MapdataGeoObj &params);
    ~ObjectFlamePoleV() override;

    void init() override;
    void calc() override;
    [[nodiscard]] LoadFlags loadFlags() const override;
    [[nodiscard]] const char *getResources() const override;
    [[nodiscard]] const char *getKclName() const override;

private:
    void initEruptionKinematics();

    void enterBeforeErupting();
    void enterErupting();
    void enterLowering();
    void enterDormant();

    void calcBeforeErupting();
    void calcErupting();
    void calcErupted();
    void calcLowering();
    void calcDormant();

    const u32 m_initDelay;     ///< Frames before state lifecycle begins
    const s32 m_cycleDuration; ///< Duration of a full loop of the geyser's state lifecycle
    const s32 m_dormantFrames; ///< Number of frames within a cycle the geyser remains dormant for
    const f32 m_scaleFactor;   ///< Scale multiplier
    const f32 m_initPosY;      ///< Initial y-axis position
    f32 m_maxOffsetY;          ///< Max height offset of the pole
    f32 m_eruptionDecel;       ///< Effectively the gravity coefficient when erupting
    f32 m_initEruptingVel;     ///< Velocity of the geyser when it begins erupting upwards
    f32 m_fallSpeed;           ///< Rate at which the geyser lowers
    const bool m_isBig; ///< Discerns between FlamePole_v and FlamePole_v_big to alter size handling
    f32 m_currOffsetY;  ///< Height offset based off eruption velocity
    f32 m_loweringStartOffsetY; ///< Height offset when the geyser begins lowering to dormancy

    /// @brief Duration of the erupted state, cached in Kinoko to avoid recomputation in
    /// @ref calcErupted()
    /// @details Rather than populating this duration directly from a param setting, the base game
    /// instead derives the erupted duration by filling in the gap between all other state durations
    /// and the total @ref m_cycleDuration.
    /// @note This is kept as an `f32` because it can be negative if the param settings are
    /// misconfigured, matching the base game's float comparison in @ref calcErupted().
    const f32 m_eruptedDuration;

    static constexpr f32 ERUPT_FRAMES = 60.0f;        ///< Frames it takes to raise to max height
    static constexpr f32 BEFORE_ERUPT_FRAMES = 50.0f; ///< Delay before pole starts erupting
    static constexpr f32 FALL_FRAMES = 180.0f;        ///< Frames it takes to lower into the ground

    /// @brief The enter and calc functions for each @ref StateManager entry
    static constexpr std::array<StateManagerEntry, 5> STATE_ENTRIES = {{
            {StateEntry<ObjectFlamePoleV, &ObjectFlamePoleV::enterBeforeErupting,
                    &ObjectFlamePoleV::calcBeforeErupting>(0)},
            {StateEntry<ObjectFlamePoleV, &ObjectFlamePoleV::enterErupting,
                    &ObjectFlamePoleV::calcErupting>(1)},
            {StateEntry<ObjectFlamePoleV, nullptr, &ObjectFlamePoleV::calcErupted>(2)},
            {StateEntry<ObjectFlamePoleV, &ObjectFlamePoleV::enterLowering,
                    &ObjectFlamePoleV::calcLowering>(3)},
            {StateEntry<ObjectFlamePoleV, &ObjectFlamePoleV::enterDormant,
                    &ObjectFlamePoleV::calcDormant>(4)},
    }};
};

} // namespace Kinoko::Field
