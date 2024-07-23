#include "ObjectBase.hh"

#include <egg/math/Math.hh>

namespace Field {

/// @addr{0x8081F828}
ObjectBase::ObjectBase(const System::MapdataGeoObj &params)
    : m_id(static_cast<ObjectId>(params.id())), m_pos(params.pos()), m_rot(params.rot() * DEG2RAD),
      m_scale(params.scale()) {}

/// @addr{0x8067E3C4}
ObjectBase::~ObjectBase() = default;

} // namespace Field
