#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectFlamePoleV final : public ObjectCollidable, public StateManager {
public:
    ObjectFlamePoleV(const System::MapdataGeoObj &params);
    ~ObjectFlamePoleV() override;

    void init() override;
    void calc() override;

    /// @addr{0x806C4898}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x806C488C}
    [[nodiscard]] const char *getResources() const override {
        return "FlamePole_v";
    }

    /// @addr{0x806C4880}
    [[nodiscard]] const char *getKclName() const override {
        return "FlamePoleEff";
    }

private:
    /// @addr{0x806C40E4}
    void enterBeforeErupting() {
        enableCollision();
    }

    /// @addr{0x806C4178}
    void enterErupting() {
        m_currOffsetY = 0.0f;
        CalcEruptionKinematics(ERUPT_FRAMES, m_maxOffsetY, m_initEruptingVel, m_eruptionDecel);
    }

    /// @addr{0x806C4280}
    void enterErupted() {}

    /// @addr{0x806C43BC}
    void enterLowering() {
        m_loweringStartOffsetY = m_currOffsetY;
        m_fallSpeed = m_maxOffsetY / FALL_FRAMES;
    }

    /// @addr{0x806C4478}
    void enterDormant() {
        disableCollision();
    }

    /// @addr{0x806C4130}
    void calcBeforeErupting() {
        if (static_cast<f32>(m_currentFrame) >= BEFORE_ERUPT_FRAMES) {
            m_nextStateId = 1;
        }
    }

    /// @addr{0x806C41F0}
    void calcErupting() {
        if (static_cast<f32>(m_currentFrame) >= ERUPT_FRAMES) {
            m_nextStateId = 2;
        }

        m_currOffsetY =
                CalcParabolicDisplacement(m_initEruptingVel, m_eruptionDecel, m_currentFrame);
    }

    void calcErupted();
    void calcLowering();

    /// @addr{0x806C44C8}
    void calcDormant() {
        if (m_currentFrame >= static_cast<u32>(m_dormantFrames)) {
            m_nextStateId = 0;
        }
    }

    /// @addr{0x806B5A0C}
    /// @brief Computes initVel and accel such that it takes t frames and the peak is maxHeight
    static void CalcEruptionKinematics(f32 t, f32 maxHeight, f32 &initVel, f32 &accel) {
        f32 doublePeak = 2.0f * maxHeight;
        initVel = doublePeak / t;
        accel = initVel * initVel / doublePeak;
    }

    const s32 m_initDelay;
    const s32 m_cycleDuration;
    const s32 m_dormantFrames;
    const f32 m_scaleFactor;
    const f32 m_initPosY;
    f32 m_maxOffsetY;    ///< Max height offset of the pole
    f32 m_eruptionDecel; ///< Effectively the gravity coefficient when erupting
    f32 m_initEruptingVel;
    f32 m_fallSpeed; ///< Rate at which the pole lowers
    const bool m_isBig;
    f32 m_currOffsetY;
    f32 m_loweringStartOffsetY;

    static constexpr f32 ERUPT_FRAMES = 60.0f;        ///< Frames it takes to raise to max height
    static constexpr f32 BEFORE_ERUPT_FRAMES = 50.0f; ///< Delay before pole starts erupting
    static constexpr f32 FALL_FRAMES = 180.0f;        ///< Frames it takes to lower into the ground

    static constexpr std::array<StateManagerEntry, 5> STATE_ENTRIES = {{
            {StateEntry<ObjectFlamePoleV, &ObjectFlamePoleV::enterBeforeErupting,
                    &ObjectFlamePoleV::calcBeforeErupting>(0)},
            {StateEntry<ObjectFlamePoleV, &ObjectFlamePoleV::enterErupting,
                    &ObjectFlamePoleV::calcErupting>(1)},
            {StateEntry<ObjectFlamePoleV, &ObjectFlamePoleV::enterErupted,
                    &ObjectFlamePoleV::calcErupted>(2)},
            {StateEntry<ObjectFlamePoleV, &ObjectFlamePoleV::enterLowering,
                    &ObjectFlamePoleV::calcLowering>(3)},
            {StateEntry<ObjectFlamePoleV, &ObjectFlamePoleV::enterDormant,
                    &ObjectFlamePoleV::calcDormant>(4)},
    }};
};

} // namespace Field
