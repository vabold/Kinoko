#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

/// @brief Represents the rising water on GCN Peach Beach and Delfino Pier battle stage.
/// @details In the base game, there are 5 subclasses owned by ObjectPsea. In terms of collision,
/// the water's height is only a function of one of them. We can simplify in Kinoko and remove that
/// subclass entirely since its position can be easily represented without an instance of the class.
class ObjectPsea final : public ObjectCollidable {
public:
    ObjectPsea(const System::MapdataGeoObj &params);
    ~ObjectPsea() override;

    void calc() override;

    /// @addr{0x8082C888}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x8082C884}
    void loadGraphics() override {}

    void createCollision() override {}

private:
    u16 m_frame;
    s16 m_cycleDuration;
    const f32 m_period;
    f32 m_initPosY;

    static constexpr s16 CYCLE_DURATION = 1200;
};

} // namespace Field
