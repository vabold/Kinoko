#pragma once

#include "game/field/obj/ObjectBase.hh"

#include "game/field/ObjectCollisionBase.hh"

namespace Field {

class ObjectCollidable : public ObjectBase {
public:
    ObjectCollidable(const System::MapdataGeoObj &params);
    ~ObjectCollidable() override;

    void load() override;
    [[nodiscard]] f32 getCollisionRadius() const override;

    virtual void loadAABB(f32 maxSpeed);
    virtual void loadAABB(f32 radius, f32 maxSpeed);
    [[nodiscard]] virtual const ObjectCollisionBase *collision() const;

protected:
    virtual void createCollision();
    virtual const EGG::Vector3f &collisionCenter() const;

    ObjectCollisionBase *m_collision;
};

} // namespace Field
