#include "ObjectParasolR.hh"

namespace Kinoko::Field {

/// @addr{0x8077902C}
ObjectParasolR::ObjectParasolR(const System::MapdataGeoObj &params) : ObjectCollidable(params) {}

/// @addr{0x80779EFC}
ObjectParasolR::~ObjectParasolR() = default;

} // namespace Kinoko::Field
