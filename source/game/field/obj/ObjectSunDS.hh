#pragma once

#include "game/field/StateManager.hh"
#include "game/field/obj/ObjectProjectileLauncher.hh"

namespace Kinoko::Field {

/// @brief Represents the sun on DS Desert Hills that launches @ref ObjectFireSnake projectiles
/// @details Follows a rail, stopping for a duration specified by the rail point. @ref
/// ObjectSunManager interfaces with this class via @ref launchPointIdx() to determine if a @ref
/// ObjectFireSnake projectile should be launched.
class ObjectSunDS final : public ObjectProjectileLauncher, private StateManager {
public:
    ObjectSunDS(const System::MapdataGeoObj &params);
    ~ObjectSunDS() override;

    void init() override;
    void calc() override;
    [[nodiscard]] LoadFlags loadFlags() const override;
    [[nodiscard]] s16 launchPointIdx() const override;

private:
    void enterStill();
    void enterRevolving();

    void calcStill();

    void calcRail();
    void calcPos();
    void checkStop();

    const f32 m_revolutionSpeed; ///< Speed of sun revolving around the course
    const s32 m_startFrame;      ///< The sun is inactive until this frame
    u32 m_stillDuration;         ///< How long the sun remains stationary for before moving again

    /// @brief The enter and calc functions for each @ref StateManager entry
    static constexpr std::array<StateManagerEntry, 2> STATE_ENTRIES = {{
            {StateEntry<ObjectSunDS, &ObjectSunDS::enterStill, &ObjectSunDS::calcStill>(0)},
            {StateEntry<ObjectSunDS, &ObjectSunDS::enterRevolving, nullptr>(1)},
    }};
};

} // namespace Kinoko::Field
