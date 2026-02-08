#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectVolcanoBall;

/// @brief The manager class that launch fireballs on Grumble Volcano.
class ObjectVolcanoBallLauncher final : public ObjectCollidable {
public:
    ObjectVolcanoBallLauncher(const System::MapdataGeoObj &params);
    ~ObjectVolcanoBallLauncher() override;

    void init() override;
    void calc() override;

    /// @addr{0x806E3A74}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    };

    /// @addr{0x806E3A70}
    void loadGraphics() override {}

    /// @addr{0x806E3A68}
    void createCollision() override {}

    /// @addr{0x806E3A6C}
    void loadRail() override {}

private:
    std::span<ObjectVolcanoBall *> m_balls;
    const f32 m_initDelay;
    const f32 m_cycleDuration;
    u32 m_currBallIdx;
    bool m_active; ///< False when the volcano is dormant, true after @ref m_initDelay
};

} // namespace Field
