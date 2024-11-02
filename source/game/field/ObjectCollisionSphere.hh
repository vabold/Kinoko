#pragma once

#include "game/field/ObjectCollisionBase.hh"

namespace Field {

class ObjectCollisionSphere : public ObjectCollisionBase {
public:
    ObjectCollisionSphere(f32 radius, const EGG::Vector3f &center);
    ~ObjectCollisionSphere() override;

    f32 getBoundingRadius() const override;
    const EGG::Vector3f &getSupport(const EGG::Vector3f &v) const override;

private:
    f32 m_radius;
    EGG::Vector3f m_pos;

    f32 m_worldRadius;
    EGG::Vector3f m_worldPos;

    EGG::Vector3f m_center;
};

} // namespace Field
