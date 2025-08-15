#include "ObjectWoodbox.hh"

#include "game/field/CollisionDirector.hh"

namespace Field {

/// @addr{0x8077E5E4}
ObjectWoodbox::ObjectWoodbox(const System::MapdataGeoObj &params)
    : ObjectBreakable(params) {}

/// @addr{0x8077E620}
ObjectWoodbox::ObjectWoodbox() = default;

/// @addr{0x8077E678}
void ObjectWoodbox::init() {
    ObjectBreakable::init();
    m_initialHeight = m_pos.y;
    m_downwardsVelocity = 0.0f;
}

/// @addr{0x8077EBB8}
void ObjectWoodbox::calcCollisionTransform() {
    if (!m_collision) {
        calcTransform();
        EGG::Vector3f nextPos = m_pos + EGG::Vector3f(0.0f, 100.0f, 0.0f);
        m_transform.setBase(3, nextPos);
        m_collision->transform(m_transform, m_scale, getCollisionTranslation());
    }
}

/// @addr{0x8077E444}
void ObjectWoodbox::enableCollision() override {
    ObjectBreakable::enableCollision();
    m_railInterpolator->init(0.0f, 0);
    m_railInterpolator->setPerPointVelocities(true);
}

/// @addr{0x8077E7B0}
void ObjectWoodbox::onRespawn() {
    m_downwardsVelocity -= 2.0f;
    m_pos.y += m_downwardsVelocity;
    m_flags.setBit(eFlags::Position);
    calcFloor();
}

/// @addr{0x8077E7E0}
void ObjectWoodbox::calcFloor() {
    CollisionInfo colInfo;
    EGG::Vector3f pos = m_pos;
    pos.y += 100.0f;

    if (Field::CollisionDirector::Instance()->checkSphereFull(100.0f, pos, EGG::Vector3f::inf, KCL_TYPE_FLOOR, &colInfo, nullptr, 0)) {
        m_downwardsVelocity *= -0.1f;
        m_pos += colInfo.tangentOff;
        m_flags.setBit(eFlags::Position);

        if (EGG::Mathf::abs(m_downwardsVelocity) < 1.0f) {
            m_state = State::ACTIVE;
        }
    }
}

} // namespace Field