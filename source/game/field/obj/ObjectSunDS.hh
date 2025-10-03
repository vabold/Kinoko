#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectProjectileLauncher.hh"

namespace Field {

class ObjectSunDS;

class ObjectSunDS : public ObjectProjectileLauncher, StateManager<ObjectSunDS> {
    friend StateManager<ObjectSunDS>;

public:
    ObjectSunDS(const System::MapdataGeoObj &params);
    ~ObjectSunDS() override;

    /// @addr{0x806DDFD4}
    void init() override {
        m_stillDuration = 0;
    }

    void calc() override;

    /// @addr{0x806DE614}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    // Not overridden in the base game, but has collision mode 0
    void createCollision() override {}

    [[nodiscard]] s16 launchPointIdx() override;

private:
    /// @addr{0x806DE1C8}
    void enterStill() {
        m_railInterpolator->setCurrVel(0.0f);
    }

    /// @addr{0x806DE408}
    void enterRevolving() {
        m_railInterpolator->setCurrVel(m_revolutionSpeed);
    }

    /// @addr{0x806DE1E4}
    void calcStill() {
        if (m_currentFrame >= m_stillDuration) {
            m_nextStateId = 1;
        }
    }

    /// @addr{0x806DE454}
    void calcRevolving() {}

    const f32 m_revolutionSpeed; ///< Speed of sun revolving around the course
    const s32 m_startFrame;      ///< The sun is inactive until this frame
    u32 m_stillDuration;         ///< How long the sun remains stationary for before moving again

    static constexpr std::array<StateManagerEntry<ObjectSunDS>, 2> STATE_ENTRIES = {{
            {0, &ObjectSunDS::enterStill, &ObjectSunDS::calcStill},
            {1, &ObjectSunDS::enterRevolving, &ObjectSunDS::calcRevolving},
    }};
};

} // namespace Field
