#include "ObjectBase.hh"

#include <egg/math/Math.hh>

namespace Field {

/// @addr{0x8081F828}
ObjectBase::ObjectBase(const System::MapdataGeoObj &params)
    : m_id(static_cast<ObjectId>(params.id())), m_pos(params.pos()), m_rot(params.rot() * DEG2RAD),
      m_scale(params.scale()), m_transform(EGG::Matrix34f::ident) {
    m_dirtyFlags.setBit(eDirtyFlags::Position, eDirtyFlags::Rotation);
}

/// @addr{0x8067E3C4}
ObjectBase::~ObjectBase() = default;

/// @addr{0x808217B8}
void ObjectBase::calcModel() {
    calcTransform();
}

/// @addr{0x80821640}
void ObjectBase::calcTransform() {
    if (m_dirtyFlags.onBit(eDirtyFlags::Rotation)) {
        m_transform.makeRT(m_rot, m_pos);
        m_dirtyFlags.resetBit(eDirtyFlags::Rotation, eDirtyFlags::Position);
    } else if (m_dirtyFlags.onBit(eDirtyFlags::Position)) {
        m_transform.setBase(3, m_pos);
        m_dirtyFlags.setBit(eDirtyFlags::Matrix);
    }
}

} // namespace Field
