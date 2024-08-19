#include "ObjectNoImpl.hh"

namespace Field {

ObjectNoImpl::ObjectNoImpl(const System::MapdataGeoObj &params) : ObjectBase(params) {}

ObjectNoImpl::~ObjectNoImpl() = default;

void ObjectNoImpl::init() {}

} // namespace Field
