#pragma once

#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

/// @brief The flamepole that erupts from a geyser on Bowser's Castle.
class ObjectFlamePole final : public ObjectCollidable {
public:
    /// @addr{0x8067E280}
    ObjectFlamePole(const System::MapdataGeoObj &params, const EGG::Vector3f &pos,
            const EGG::Vector3f &rot, const EGG::Vector3f &scale)
        : ObjectCollidable("FlamePoleEff", pos, rot, scale) {}

    /// @addr{0x80681828}
    ~ObjectFlamePole() override = default;

    /// @addr{0x8067E410}
    void calc() override {
        if (m_isActive) {
            resize(RADIUS * m_scale.y, 0.0f);
        }
    }

    /// @addr{0x80681820}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    [[nodiscard]] const char *getResources() const override {
        return "FlamePole";
    }

    void setActive(bool isSet) {
        m_isActive = isSet;
    }

    static constexpr f32 HEIGHT = 384.0f;

private:
    bool m_isActive;

    static constexpr f32 RADIUS = 70.0f;
};

} // namespace Field
