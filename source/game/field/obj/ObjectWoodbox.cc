#include "ObjectWoodbox.hh"

namespace Field {

/// @addr{0x8077E5E4}
ObjectWoodbox::ObjectWoodbox(const System::MapdataGeoObj &params) : ObjectBreakable(params) {}

/// @addr{0x8077E620}
ObjectWoodbox::~ObjectWoodbox() = default;

/// @addr{0x8077EBB8}
void ObjectWoodbox::calcCollisionTransform() {
    constexpr f32 HALF_SIZE = 100.0f;
    constexpr EGG::Vector3f POS_OFFSET = EGG::Vector3f(0.0f, HALF_SIZE, 0.0f);

    if (!m_collision) {
        return;
    }

    calcTransform();
    EGG::Matrix34f mat = transform();
    mat.setBase(3, pos() + POS_OFFSET);
    m_collision->transform(mat, scale(), getCollisionTranslation());
}

} // namespace Field
