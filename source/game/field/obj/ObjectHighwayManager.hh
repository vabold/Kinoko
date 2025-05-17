#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectCarTGE;

class ObjectHighwayManager : public ObjectCollidable {
public:
    ObjectHighwayManager();
    ~ObjectHighwayManager() override;

    void init() override;
    void calc() override;

    /// @addr{0x806D5C6C}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x806D5C68}
    void loadGraphics() override {}

    /// @addr{0x806D5C60}
    void createCollision() override {}

    /// @addr{0x806D5C64}
    void loadRail() override {}

    [[nodiscard]] u32 squashTimer() const {
        return m_squashTimer;
    }

private:
    void calcSquash();

    std::span<ObjectCarTGE *> m_cars;
    u32 m_squashTimer; ///< Normally an array, one for each player.

    static constexpr u32 SQUASH_MAX = 600;
};

} // namespace Field
