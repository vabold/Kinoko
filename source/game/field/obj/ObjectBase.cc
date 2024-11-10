#include "ObjectBase.hh"

#include <egg/math/Math.hh>

namespace Field {

/// @addr{0x8081F828}
ObjectBase::ObjectBase(const System::MapdataGeoObj &params)
    : m_id(static_cast<ObjectId>(params.id())), m_flags(0x3), m_pos(params.pos()),
      m_rot(params.rot() * DEG2RAD), m_scale(params.scale()), m_transform(EGG::Matrix34f::ident) {}

/// @addr{0x8067E3C4}
ObjectBase::~ObjectBase() = default;

/// @addr{0x808217B8}
void ObjectBase::calcModel() {
    if (!(m_flags & 2)) {
        if (!(m_flags & 1)) {
            m_transform.setBase(3, m_pos);
            m_flags |= 4;
        }
    } else {
        m_transform.makeRT(m_rot, m_pos);
        m_flags &= ~0x3;
    }
}

/// @addr{0x806BF434}
u32 ObjectBase::loadFlags() const {
    // TODO: This references LOD to determine load flags
    return 0;
}

/// @addr{0x80681598}
const EGG::Vector3f &ObjectBase::getPosition() const {
    return m_pos;
}

/// @addr{0x8080BDC0}
f32 ObjectBase::getCollisionRadius() const {
    constexpr f32 BASE_RADIUS = 100.0f;

    return BASE_RADIUS;
}

/// @addr{0x80572574}
ObjectId ObjectBase::id() const {
    return m_id;
}

} // namespace Field
