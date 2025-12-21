#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectFlamePole.hh"
#include "game/field/obj/ObjectKCL.hh"

namespace Field {

class ObjectFlamePole;

/// @brief Represents the geysers on Bowser's Castle.
class ObjectFlamePoleFoot final : public ObjectKCL, public StateManager {
public:
    ObjectFlamePoleFoot(const System::MapdataGeoObj &params);
    ~ObjectFlamePoleFoot() override;

    void init() override;
    void calc() override;

    /// @addr{0x80681590}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x806814C4}
    [[nodiscard]] f32 getCollisionRadius() const override {
        ASSERT(m_mapObj);
        return 245.0f * static_cast<f32>(m_mapObj->setting(2));
    }

    /// @addr{0x8067FBB8}
    [[nodiscard]] const EGG::Matrix34f &getUpdatedMatrix(u32 /*timeOffset*/) override {
        calcTransform();
        m_workMatrix = m_transform;
        return m_transform;
    }

    [[nodiscard]] f32 getScaleY(u32 timeOffset) const override;
    [[nodiscard]] bool checkCollision(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;
    [[nodiscard]] bool checkCollisionCached(f32 radius, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos, KCLTypeMask mask, CollisionInfo *info,
            KCLTypeMask *maskOut, u32 timeOffset) override;

private:
    /// @addr{0x8067F2DC}
    void enterExpanding() {
        m_heightOffset = 0.0f;
    }

    void enterEruptingUp();

    /// @addr{0x8067F538}
    void enterEruptingStay() {
        m_scaledHeight = m_initScaledHeight;
    }

    /// @addr{0x8067F5EC}
    void enterEruptingDown() {
        m_scaledHeight = m_heightOffset;
    }

    /// @addr{0x8067F650}
    void enterDormant() {
        m_pole->setActive(false);
        m_pole->disableCollision();
    }

    /// @addr{0x8067F6B0}
    void enterState5() {}

    /// @addr{0x8067F2F0}
    void calcExpanding() {}

    void calcEruptingUp();
    void calcEruptingStay();

    /// @addr{0x8067F604}
    void calcEruptingDown() {
        m_heightOffset =
                m_scaledHeight - m_scaleDelta * static_cast<f32>(m_cycleFrame - m_stateStart[3]);
    }

    /// @addr{0x8067F6AC}
    void calcDormant() {}

    /// @addr{0x8067F6B4}
    void calcState5() {}

    void calcStates();

    EGG::Matrix34f m_workMatrix;
    ObjectFlamePole *m_pole;
    const s32 m_extraCycleFrames; ///< Used to increase the animation cycle beyond the typical 540
                                  ///< frames
    const s32 m_initDelay;
    f32 m_poleScale;
    s32 m_eruptUpDuration;
    s32 m_eruptDownDuration;
    std::array<s32, 6> m_stateStart;
    f32 m_eruptDownVel;
    f32 m_initScaledHeight;
    s32 m_cycleFrame;
    f32 m_heightOffset; ///< Cyclical offset that adjusts the pole's height
    f32 m_scaledHeight;
    f32 m_scaleDelta; ///< Change in scale per frame at end of cycle
    f32 m_eruptAccel;
    f32 m_initEruptVel;

    static u32 FLAMEPOLE_COUNT;

    static constexpr std::array<StateManagerEntry, 6> STATE_ENTRIES = {{
            {StateEntry<ObjectFlamePoleFoot, &ObjectFlamePoleFoot::enterExpanding,
                    &ObjectFlamePoleFoot::calcExpanding>(0)},
            {StateEntry<ObjectFlamePoleFoot, &ObjectFlamePoleFoot::enterEruptingUp,
                    &ObjectFlamePoleFoot::calcEruptingUp>(1)},
            {StateEntry<ObjectFlamePoleFoot, &ObjectFlamePoleFoot::enterEruptingStay,
                    &ObjectFlamePoleFoot::calcEruptingStay>(2)},
            {StateEntry<ObjectFlamePoleFoot, &ObjectFlamePoleFoot::enterEruptingDown,
                    &ObjectFlamePoleFoot::calcEruptingDown>(3)},
            {StateEntry<ObjectFlamePoleFoot, &ObjectFlamePoleFoot::enterDormant,
                    &ObjectFlamePoleFoot::calcDormant>(4)},
            {StateEntry<ObjectFlamePoleFoot, &ObjectFlamePoleFoot::enterState5,
                    &ObjectFlamePoleFoot::calcState5>(5)},
    }};

    static constexpr s32 CYCLE_FRAMES = 540;
};

} // namespace Field
