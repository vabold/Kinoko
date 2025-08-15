#pragma once

#include "game/field/obj/ObjectBreakable.hh"

namespace Field {

class ObjectWoodbox : public ObjectBreakable {
public:
    /// @addr{0x8077E5E4}
    ObjectWoodbox(const System::MapdataGeoObj &params): ObjectBreakable(params) {}

    /// @addr{0x8077E620}
    ~ObjectWoodbox() = default;

    void init();

    void calcCollisionTransform() override;

    void registerCollisionEntityFromParams();

    /// @addr{0x8077EDAC}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    // @addr{0x8077ed7c}
    [[nodiscard]] char *getKclName() const override {
        return "ironbox"; // woodbox in GPs
    }

    /// @addr{0x8077ED70}
    [[nodiscard]] char *getResourcesNames() const override {
        return "woodbox";
    }

    void respawn();

private:
    float m_initialHeight;
    u32 m_c0;
};

} // namespace Field