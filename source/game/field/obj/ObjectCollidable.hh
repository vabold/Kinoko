#pragma once

#include "game/field/obj/ObjectBase.hh"

#include "game/field/ObjectCollisionBase.hh"

namespace Kart {

class KartObject;
enum class Reaction;

} // namespace Kart

namespace Field {

class ObjectCollidable : public ObjectBase {
public:
    ObjectCollidable(const System::MapdataGeoObj &params);
    ~ObjectCollidable() override;

    void load() override;
    void calcCollisionTransform() override;
    f32 getCollisionRadius() const override;
    void createCollision() override;

    virtual void loadAABB(f32 maxSpeed);
    virtual void loadAABB(f32 radius, f32 maxSpeed);
    virtual void processKartReactions(Kart::KartObject *kartObj, Kart::Reaction &reactionOnKart,
            Kart::Reaction &reactionOnObj);
    virtual Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, const EGG::Vector3f &hitDepth);
    virtual void onWallCollision(Kart::KartObject *, const EGG::Vector3f &) {}
    virtual void onObjectCollision(Kart::KartObject *) {}
    virtual bool checkCollision(ObjectCollisionBase *lhs, EGG::Vector3f &dist);
    virtual const EGG::Vector3f &getCollisionTranslation() const;
    [[nodiscard]] virtual ObjectCollisionBase *collision() const;

protected:
    virtual const EGG::Vector3f &collisionCenter() const;

    ObjectCollisionBase *m_collision;
};

} // namespace Field
