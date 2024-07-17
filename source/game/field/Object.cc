#include "Object.hh"

#include <egg/math/Math.hh>

namespace Field {

/// @addr{0x8081F828}
Object::Object(const System::MapdataGeoObj &params)
    : m_id(params.id()), m_pos(params.pos()), m_rot(params.rot() * DEG2RAD),
      m_scale(params.scale()) {}

} // namespace Field
