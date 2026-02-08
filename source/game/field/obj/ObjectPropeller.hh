#pragma once

#include "game/field/ObjectCollisionCylinder.hh"
#include "game/field/obj/ObjectCollidable.hh"

namespace Field {

class ObjectPropeller final : public ObjectCollidable {
public:
    ObjectPropeller(const System::MapdataGeoObj &params);
    ~ObjectPropeller() override;

    void init() override;
    void calc() override;

    /// @addr{0x80765BC0}
    [[nodiscard]] u32 loadFlags() const override {
        return 1;
    }

    void createCollision() override;
    void calcCollisionTransform() override;
    [[nodiscard]] f32 getCollisionRadius() const override;
    bool checkCollision(ObjectCollisionBase *lhs, EGG::Vector3f &dist) override;

private:
    f32 m_angVel;
    f32 m_angle;
    EGG::Vector3f m_axis;
    EGG::Matrix34f m_rotMat;
    EGG::Matrix34f m_curRot;
    std::array<ObjectCollisionCylinder *, 3> m_blades;
};

} // namespace Field
