#pragma once

#include "game/field/obj/ObjectBreakable.hh"

namespace Field {

class ObjectWoodbox : public ObjectBreakable {
public:
    ObjectWoodbox(const System::MapdataGeoObj &params);
    ~ObjectWoodbox() override;

    void init() override;

    /// @addr{0x8077ED7C}
    [[nodiscard]] const char *getKclName() const override {
        return "ironbox"; // woodbox when not in TTs
    }

    void calcCollisionTransform() override;

private:
    static constexpr f32 HALF_SIZE = 100.0f;
    f32 m_downwardsVelocity;
};

} // namespace Field
