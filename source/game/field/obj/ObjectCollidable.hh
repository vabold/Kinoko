#pragma once

#include "game/field/obj/ObjectBase.hh"

#include "game/field/ObjectCollisionBase.hh"

namespace Field {

class ObjectCollidable : public ObjectBase {
public:
    ObjectCollidable(const System::MapdataGeoObj &params);
    ~ObjectCollidable() override;

    void load() override;

    [[nodiscard]] virtual const ObjectCollisionBase *collision() const;

protected:
    virtual void createCollision();
    virtual const EGG::Vector3f &collisionCenter() const;

    ObjectCollisionBase *m_collision;
};

} // namespace Field
