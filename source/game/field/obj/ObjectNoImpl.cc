#include "ObjectNoImpl.hh"

#include "game/field/ObjectDirector.hh"

namespace Kinoko::Field {

ObjectNoImpl::ObjectNoImpl(const System::MapdataGeoObj &params) : ObjectBase(params) {}

ObjectNoImpl::~ObjectNoImpl() = default;

void ObjectNoImpl::load() {
    ObjectDirector::Instance()->addObjectNoImpl(this);
}

} // namespace Kinoko::Field
