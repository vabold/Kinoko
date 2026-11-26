#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Kinoko::Field {

class ObjectWoodboxWSub;

/// @brief Represents a wooden box spawner for @ref ObjectWoodboxWSub boxes which follow a rail,
/// like on Toad's Factory
class ObjectWoodboxW final : public ObjectCollidable {
public:
    ObjectWoodboxW(const System::MapdataGeoObj &params);
    ~ObjectWoodboxW() override;

    void init() override;
    void calc() override;
    void createCollision() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

private:
    owning_span<ObjectWoodboxWSub *> m_boxes; ///< Pointers to the wooden boxes that spawn
    s32 m_spawnTimer;                         ///< Frames until the next box spawns
    u32 m_nextBoxIdx;                         ///< Index of the next box to spawn
    const s32 m_spawnInterval;                ///< The fixed interval between spawns
};

} // namespace Kinoko::Field
