#pragma once

#include <Types.hh>

#include "game/field/obj/ObjectItemboxBlock.hh"

namespace Kinoko::Field {

/// @brief Object which represents a line of itemboxes, brick blocks which may be pressed into an
/// itembox, and a stomper.
/// @details The stomper is managed by @ref ObjectItemboxBlock. Since we only implement the
/// components relevant to Time Trial mode in Kinoko, the itemboxes and bricks are not implemented
/// in full - only the stomper-related logic.
class ObjectItemboxLine final : public ObjectCollidable {
public:
    ObjectItemboxLine(const System::MapdataGeoObj &params);
    ~ObjectItemboxLine() override;

    void init() override;
    void calc() override;
    [[nodiscard]] LoadFlags loadFlags() const override;

private:
    owning_span<ObjectItemboxBlock *> m_blocks; ///< Array of itembox/block objects
    u32 m_spawnCooldown;               ///< Number of frames until next itembox/block spawns
    u32 m_curBlockIdx;                 ///< Index of the next block to spawn
    const u32 m_spawnCooldownDuration; ///< Total framecount between itembox/block spawns
};

} // namespace Kinoko::Field
