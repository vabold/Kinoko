#pragma once

#include "game/field/ObjectCollisionBase.hh"

namespace Field {

class ObjectCollisionSphere : public ObjectCollisionBase {
public:
    ObjectCollisionSphere(f32 radius, const EGG::Vector3f &center);
    ~ObjectCollisionSphere() override;

    void transform(const EGG::Matrix34f &mat, const EGG::Vector3f &scale,
            const EGG::Vector3f &speed) override;
    const EGG::Vector3f &getSupport(const EGG::Vector3f &v) const override;

    /// @addr{0x80836B54}
    f32 getBoundingRadius() const override {
        return m_scaledRadius;
    }

private:
    f32 m_radius;
    EGG::Vector3f m_pos;
    f32 m_scaledRadius;
    EGG::Vector3f m_scaledPos;
    EGG::Vector3f m_worldPos;
    EGG::Vector3f m_center;
};

} // namespace Field
