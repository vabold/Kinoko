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
    [[nodiscard]] f32 getCollisionRadius() const override;

    virtual void loadAABB(f32 maxSpeed);
    virtual void loadAABB(f32 radius, f32 maxSpeed);
    virtual void processKartReactions(Kart::KartObject *kartObj, Kart::Reaction &reactionOnKart,
            Kart::Reaction &reactionOnObj);

    /// @addr{0x8068179C}
    virtual Kart::Reaction onCollision(Kart::KartObject * /*kartObj*/,
            Kart::Reaction reactionOnKart, Kart::Reaction /*reactionOnObj*/,
            EGG::Vector3f & /*hitDepth*/) {
        return reactionOnKart;
    }

    virtual void onWallCollision(Kart::KartObject *, const EGG::Vector3f &) {}
    virtual void onObjectCollision(Kart::KartObject *) {}

    /// @addr{0x80681748}
    virtual bool checkCollision(ObjectCollisionBase *lhs, EGG::Vector3f &dist);

    /// @addr{0x8068173C}
    virtual const EGG::Vector3f &getCollisionTranslation() const {
        return EGG::Vector3f::zero;
    }

    /// @addr{0x80573518}
    [[nodiscard]] virtual ObjectCollisionBase *collision() const {
        return m_collision;
    }

protected:
    void createCollision() override;

    /// @addr{0x806816B8}
    virtual const EGG::Vector3f &collisionCenter() const {
        return EGG::Vector3f::zero;
    }

    ObjectCollisionBase *m_collision;
};

} // namespace Field
