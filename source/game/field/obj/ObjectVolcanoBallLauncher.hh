#pragma once

#include <Types.hh>

#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

class ObjectVolcanoBall;

/// @brief The manager class that launches fireballs on Grumble Volcano
/// @details Lies dormant until @ref m_initDelay frames have elapsed. Then, every @ref
/// m_cycleDuration frames, a fireball is launched. The launcher rotates through an array of
/// fireballs to launch.
class ObjectVolcanoBallLauncher final : public ObjectCollidable {
public:
    ObjectVolcanoBallLauncher(const System::MapdataGeoObj &params);
    ~ObjectVolcanoBallLauncher() override;

    void init() override;
    void calc() override;

    void loadGraphics() override;
    void createCollision() override;
    void loadRail() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

private:
    owning_span<ObjectVolcanoBall *> m_balls; ///< Array of fireballs managed by the launcher
    const f32 m_initDelay;     ///< Frames the launcher lies dormant at the start of the race
    const f32 m_cycleDuration; ///< Frames between consecutive fireball launches
    u32 m_currBallIdx;         ///< Index of the next fireball to be launched
    bool m_active;             ///< False when the volcano is dormant, true after @ref m_initDelay
};

} // namespace Kinoko::Field
