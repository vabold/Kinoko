#pragma once

#include "game/field/ObjectCollisionCylinder.hh"
#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectShip64 final : public ObjectCollidable {
public:
    ObjectShip64(const System::MapdataGeoObj &params);
    ~ObjectShip64();

    void init() override;
    void calc() override;

    /// @addr{0x80766CA4}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    /// @addr{0x80766BB4}
    [[nodiscard]] const char *getResources() const override {
        return "DKShip64";
    }

    /// @addr{0x80766BC0}
    [[nodiscard]] const char *getKclName() const override {
        return "DKShip64";
    }

    void createCollision() override;
    void calcCollisionTransform() override;
    bool checkCollision(ObjectCollisionBase *lhs, EGG::Vector3f &dist) override;

private:
    EGG::Vector3f m_tangent;
    ObjectCollisionCylinder *m_auxCollision;
};

} // namespace Field
