#pragma once

#include "game/field/obj/ObjectBelt.hh"

namespace Field {

/// @brief The conveyers outside of the factory on Toad's Factory.
class ObjectBeltEasy final : public ObjectBelt {
public:
    ObjectBeltEasy(const System::MapdataGeoObj &params);
    ~ObjectBeltEasy() override;

private:
    [[nodiscard]] EGG::Vector3f calcRoadVelocity(u32 variant, const EGG::Vector3f &pos,
            s32 timeOffset) const override;

    /// @addr{0x807FC6C8}
    [[nodiscard]] bool isMoving(u32 variant, const EGG::Vector3f & /*pos*/) const override {
        return variant == 2 || variant == 3;
    }
};

} // namespace Field
