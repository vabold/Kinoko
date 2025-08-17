#include "ObjectBreakable.hh"

namespace Field {

/// @addr{0x8076EBE0}
ObjectBreakable::ObjectBreakable(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_state(State::Uninitialized) {}

/// @addr{0x8076EC28}
ObjectBreakable::~ObjectBreakable() = default;

} // namespace Field
