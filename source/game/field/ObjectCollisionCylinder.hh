#pragma once

#include "game/field/ObjectCollisionBase.hh"

namespace Field {

class ObjectCollisionCylinder : public ObjectCollisionBase {
public:
    ObjectCollisionCylinder(f32 radius, f32 height, const EGG::Vector3f &center);
    ~ObjectCollisionCylinder() override;

    void transform(const EGG::Matrix34f &mat, const EGG::Vector3f &scale,
            const EGG::Vector3f &speed) override;

    /// @addr{0x8083618C}
    const EGG::Vector3f &getSupport(const EGG::Vector3f &v) const override {
        return m_top.dot(v) > m_bottom.dot(v) ? m_top : m_bottom;
    }

    /// @addr{0x80836498}
    f32 getBoundingRadius() const override {
        return m_worldRadius;
    }

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
