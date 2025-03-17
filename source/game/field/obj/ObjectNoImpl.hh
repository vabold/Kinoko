#pragma once

#include "game/field/obj/ObjectBase.hh"

namespace Field {

class ObjectNoImpl final : public ObjectBase {
public:
    ObjectNoImpl(const System::MapdataGeoObj &params);
    ~ObjectNoImpl() override;

    void load() override;
    void createCollision() override {}
    void calcCollisionTransform() override {}
};

} // namespace Field
