#include "Mdush.hh"

namespace Field
{

Mdush::~Mdush() = default;

EGG::Matrix34f &Mdush::getUpdatedMatrix() {
    updateMatrix();
    return m_transformationMatrix;
}
f32 Mdush::getScaleY() const {
    return m_scale.y;
}

f32 Mdush::colRadiusAdditionalLength() const {
    return 0.0f;
}

} // namespace Field
