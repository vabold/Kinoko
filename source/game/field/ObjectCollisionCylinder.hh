#pragma once

#include "game/field/ObjectCollisionBase.hh"

namespace Field {

class ObjectCollisionCylinder : public ObjectCollisionBase {
public:
    ObjectCollisionCylinder(f32 radius, f32 height, const EGG::Vector3f &center);
    ~ObjectCollisionCylinder() override;

private:
    f32 m_radius;
    f32 m_height;
    EGG::Vector3f m_pos;

    f32 m_worldRadius;
    f32 m_worldHeight;
    EGG::Vector3f m_worldPos;

    EGG::Vector3f m_center;
    EGG::Vector3f m_top;
    EGG::Vector3f m_bottom;
};

} // namespace Field
