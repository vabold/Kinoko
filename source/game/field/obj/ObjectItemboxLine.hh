#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectItemboxPress;

/// @brief Object which represents a line of itemboxes, a brick block which may be pressed into an
/// itembox, and a stomper. The stomper is held and managed by @ref ObjectItemboxPress.
class ObjectItemboxLine final : public ObjectCollidable {
public:
    ObjectItemboxLine(const System::MapdataGeoObj &params);
    ~ObjectItemboxLine() override;

    void init() override;
    void calc() override;

    /// @addr{0x8076E9BC}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

private:
    std::span<ObjectItemboxPress *> m_press;
    s32 m_stompCooldown; ///< Number of frames in between stomps
    u32 m_curPressIdx;
    const s32 m_cooldownDuration;
};

} // namespace Field
