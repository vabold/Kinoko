#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectKCL.hh"

namespace Kinoko {

namespace Host {

class Context;

} // namespace Host

namespace Field {

class ObjectFlamePole;

/** @brief Calculates the start frame for each of the @ref ObjectFlamePoleFoot state boundaries
 * @return An array containing the start frames for each of the six states
 * @relates ObjectFlamePoleFoot
 * @details We extract this logic to a helper function in Kinoko since the frame boundaries can be
 * determined at compile-time. Let \f$t\f$ be @ref ObjectFlamePoleFoot::m_cycleFrame and \f$s_i\f$
 * be @ref ObjectFlamePoleFoot::STATE_STARTS "STATE_STARTS[i]". Then
 * \f[\text{stateId}(t) = \begin{cases}
 *     0 & 0 \le t < s_1 \\
 *     1 & s_1 \le t < s_2 \\
 *     2 & s_2 \le t < s_3 \\
 *     3 & s_3 \le t < s_4 \\
 *     4 & s_4 \le t < s_5 \\
 *     5 & t \ge s_5
 * \end{cases}\f] where \f[\begin{aligned}
 * N &= 6 \cdot \text{CYCLE_DURATION} \\
 * s_1 &= \left\lfloor \frac{0.3 N}{7} \right\rfloor \\
 * s_2 &= s_1 + \left\lfloor \frac{0.1 N}{7} \right\rfloor \\
 * s_3 &= s_2 + \left\lfloor \frac{\text{CYCLE_DURATION}}{7} \right\rfloor \\
 * s_4 &= s_3 + \left\lfloor \frac{0.2 N}{7} \right\rfloor \\
 * s_5 &= s_4 + \left\lfloor \frac{0.4 N}{7} \right\rfloor
 * \end{aligned}\f] Simplifying further, we have:
 * \f[\text{stateId}(t) = \begin{cases}
 *     0 & 0 \le t < 138 \\
 *     1 & 138 \le t < 184 \\
 *     2 & 184 \le t < 261 \\
 *     3 & 261 \le t < 353 \\
 *     4 & 353 \le t < 538 \\
 *     5 & t \ge 538
 * \end{cases}\f]
 **/
[[nodiscard]] static consteval std::array<s32, 6> FlamePoleFootStateFrameBoundaries() {
    constexpr u32 CYCLE_DURATION = 540;
    constexpr f32 NORMALIZATION = static_cast<f32>(CYCLE_DURATION) * (7.0f - 1.0f);
    constexpr s32 ERUPT_UP_DURATION = static_cast<s32>(0.1f * NORMALIZATION / 7.0f);
    constexpr s32 ERUPT_DOWN_DURATION = static_cast<s32>(0.2f * NORMALIZATION / 7.0f);

    s32 state1 = static_cast<s32>(0.3f * NORMALIZATION / 7.0f);
    s32 state2 = state1 + ERUPT_UP_DURATION;
    s32 state3 = state2 + static_cast<s32>(static_cast<f32>(CYCLE_DURATION) * 1.0f / 7.0f);
    s32 state4 = state3 + ERUPT_DOWN_DURATION;
    s32 state5 = state4 + static_cast<s32>(0.4f * NORMALIZATION / 7.0f);
    return {{0, state1, state2, state3, state4, state5}};
}

/// @brief Represents the expanding geyser humps at the end of Bowser's Castle.
/// @details Each "foot" owns a @ref ObjectFlamePole. The foot controls updates the pole's position
/// and scale to effectively cause the pole to appear to erupt out of the ground. The pole erupts
/// upwards, dips a bit, rises back up, and then descends back beneath the foot.
class ObjectFlamePoleFoot final : public ObjectKCL, private StateManager {
    /// @brief Grants access to static private members so a @ref Host::Context can restore their
    /// state on context switch
    friend class Host::Context;

public:
    ObjectFlamePoleFoot(const System::MapdataGeoObj &params);
    ~ObjectFlamePoleFoot() override;

    void init() override;
    void calc() override;
    [[nodiscard]] LoadFlags loadFlags() const override;
    [[nodiscard]] f32 getCollisionRadius() const override;

    [[nodiscard]] const EGG::Matrix34f &getUpdatedMatrix(u32 timeOffset) override;
    [[nodiscard]] f32 getScaleY(u32 timeOffset) const override;
    [[nodiscard]] bool checkCollision(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;
    [[nodiscard]] bool checkCollisionCached(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;

private:
    void enterExpanding();
    void enterEruptingUp();
    void enterEruptingStay();
    void enterEruptingDown();
    void enterDormant();

    void calcEruptingUp();
    void calcEruptingStay();
    void calcEruptingDown();

    void calcStates();
    void calcHeightAndScale();
    void initEruptKinematics();

    ObjectFlamePole *m_pole;      ///< Pointer to the associated flamepole
    const u32 m_extraCycleFrames; ///< Additional dormancy frames
    const u32 m_initDelay;        ///< Frame delay before the state lifecycle begins
    f32 m_maxScale;               ///< Size/speed of the geyser hump during eruption
    f32 m_scaleRate;              ///< Change in scale when the pole is descending
    f32 m_maxHeight;              ///< Peak height of pole's eruption
    s32 m_cycleFrame;             ///< Current frame within lifecycle
    f32 m_heightOffset;           ///< Cyclical offset that adjusts the pole's height
    f32 m_eruptedHeightOffset;    ///< Base height offset for sinusoidal stay and descending motion
    f32 m_fallVel;                ///< Change in scale per frame at end of cycle
    f32 m_eruptAccel;             ///< Deceleration applied to height offset as the pole erupts
    f32 m_initEruptVel;           ///< Initial velocity when the pole erupts

    /// @addr{0x809C21E0}
    /// @brief Global variable that tracks the number of flamepole instances
    /// @details If the size factor (third param setting) for a flame pole is set to 0, then the
    /// flame pole's scale is computed as 3 + (s_flamePoleCount % 3)
    static u32 s_flamePoleCount;

    /// @brief Duration for a full lifecycle of erupting, descending, and dormancy
    static constexpr u32 CYCLE_DURATION = 540;

    /// @brief The numerator for fractional state boundary timing in
    /// @ref FlamePoleFootStateFrameBoundaries()
    static constexpr f32 NORMALIZATION = static_cast<f32>(CYCLE_DURATION) * (7.0f - 1.0f);

    /// @brief Framecount the pole spends erupting upwards
    static constexpr s32 ERUPT_UP_DURATION = static_cast<s32>(0.1f * NORMALIZATION / 7.0f);

    /// @brief Framecount the pole spends lowering into dormancy
    static constexpr s32 ERUPT_DOWN_DURATION = static_cast<s32>(0.2f * NORMALIZATION / 7.0f);

    /// @brief The enter and calc functions for each @ref StateManager entry
    static constexpr std::array<StateManagerEntry, 6> STATE_ENTRIES = {{
            {StateEntry<ObjectFlamePoleFoot, &ObjectFlamePoleFoot::enterExpanding, nullptr>(0)},
            {StateEntry<ObjectFlamePoleFoot, &ObjectFlamePoleFoot::enterEruptingUp,
                    &ObjectFlamePoleFoot::calcEruptingUp>(1)},
            {StateEntry<ObjectFlamePoleFoot, &ObjectFlamePoleFoot::enterEruptingStay,
                    &ObjectFlamePoleFoot::calcEruptingStay>(2)},
            {StateEntry<ObjectFlamePoleFoot, &ObjectFlamePoleFoot::enterEruptingDown,
                    &ObjectFlamePoleFoot::calcEruptingDown>(3)},
            {StateEntry<ObjectFlamePoleFoot, &ObjectFlamePoleFoot::enterDormant, nullptr>(4)},
            {StateEntry<ObjectFlamePoleFoot, nullptr, nullptr>(5)},
    }};

    /// @brief Start frame for intervals of the piecewise state function
    static constexpr std::array<s32, 6> STATE_STARTS = FlamePoleFootStateFrameBoundaries();
};

} // namespace Field

} // namespace Kinoko
