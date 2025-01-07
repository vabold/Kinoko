#include "ObjectParasolR.hh"

namespace Field {

/// @addr{0x8077902C}
ObjectParasolR::ObjectParasolR(const System::MapdataGeoObj &params) : ObjectCollidable(params) {}

/// @addr{0x80779EFC}
ObjectParasolR::~ObjectParasolR() = default;

/// @addr{0x80779EF4}
u32 ObjectParasolR::loadFlags() const {
    return 1;
}

} // namespace Field
