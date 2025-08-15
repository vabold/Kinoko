#include "ObjectWoodbox.hh"

/// @addr{0x8077E678}
void ObjectWoodbox::init() {
    m_initialHeight = m_pos.y;
    m_c0 = 0;
}

/// @addr{0x8077EBB8}
void ObjectWoodbox::calcCollisionTransform() {
    if (m_collision) {
        calcTransform();
        EGG::Matrix34f mat = m_transform;
        mat.setBase(3, m_pos + EGG::Vector3f(0.0f, 100.0f, 0.0f));
        m_collision->transform(mat, m_scale, getCollisionTranslation());
    }
}

/// @addr{0x8077ECE4}
void ObjectWoodbox::registerCollisionEntityFromParams() {
    f32 speed = static_cast<f32>(m_mapObj->setting(0));
    loadAABB(speed);
}

// @addr{0x8077E750}
void ObjectWoodbox::respawn() {
    enableCollision();
    m_pos.y = m_initialHeight + 5000.0f;
    m_flags.setBit(eFlags::Position);
}