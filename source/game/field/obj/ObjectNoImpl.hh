#pragma once

#include "game/field/ObjectDirector.hh"

namespace Kinoko::Field {

/// @brief Represents an object that is explicitly not implemented in Kinoko
/// @details These objects are not implemented because they either do not have collision or do not
/// pertain to Time Trial mode.
class ObjectNoImpl final : public ObjectBase {
public:
    /// @copybrief ObjectBase::ObjectBase(const System::MapdataGeoObj &)
    /// @param params The parameters used to initialize the object
    ObjectNoImpl(const System::MapdataGeoObj &params) : ObjectBase(params) {}

    /// @brief Default virtual destructor
    ~ObjectNoImpl() override = default;

    /// @brief Registers the object to the @ref ObjectDirector
    void load() override {
        ObjectDirector::Instance()->addObjectNoImpl(this);
    }

    /// @copybrief ObjectBase::createCollision()
    /// @details No-op since these objects are not implemented.
    void createCollision() override {}

    /// @copybrief ObjectBase::calcCollisionTransform()
    /// @details No-op since these objects are not implemented.
    void calcCollisionTransform() override {}
};

} // namespace Kinoko::Field
