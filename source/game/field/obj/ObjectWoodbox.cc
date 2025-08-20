#include "ObjectWoodbox.hh"

#include "game/field/CollisionDirector.hh"

namespace Field {

/// @addr{0x8077E5E4}
ObjectWoodbox::ObjectWoodbox(const System::MapdataGeoObj &params) : ObjectBreakable(params) {}

/// @addr{0x8077E620}
ObjectWoodbox::~ObjectWoodbox() = default;

/// @addr{0x8077EBB8}
void ObjectWoodbox::calcCollisionTransform() {
    constexpr f32 HALF_SIZE = 100.0f;

    if (!m_collision) {
        return;
    }

    calcTransform();
    EGG::Vector3f nextPos = m_pos + EGG::Vector3f(0.0f, HALF_SIZE, 0.0f);
    m_transform.setBase(3, nextPos);
    m_collision->transform(m_transform, m_scale, getCollisionTranslation());
}

} // namespace Field
