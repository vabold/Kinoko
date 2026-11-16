#pragma once

#include "game/field/obj/ObjectBase.hh"

namespace Kinoko {

namespace Kart {

class KartObject;
enum class Reaction;

} // namespace Kart

namespace Field {

class ObjectCollisionBase;

/// @brief Abstract base class for all objects that can collide with karts
/// @details Declares virtual methods for collision checks and collision callbacks.
class ObjectCollidable : public ObjectBase {
public:
    ObjectCollidable(const System::MapdataGeoObj &params);
    ObjectCollidable(const char *name, const EGG::Vector3f &pos, const EGG::Vector3f &rot,
            const EGG::Vector3f &scale);
    ~ObjectCollidable() override;

    void load() override;
    void calcCollisionTransform() override;
    [[nodiscard]] f32 getCollisionRadius() const override;

    virtual void loadAABB(f32 maxSpeed);
    virtual void loadAABB(f32 radius, f32 maxSpeed);
    virtual void processKartReactions(Kart::KartObject *kartObj, Kart::Reaction &reactionOnKart,
            Kart::Reaction &reactionOnObj);
    virtual Kart::Reaction onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
            Kart::Reaction reactionOnObj, EGG::Vector3f &hitDepth);
    virtual bool checkCollision(ObjectCollisionBase *lhs, EGG::Vector3f &dist);

    /// @beginGetters

    /// @addr{0x80573518}
    /// @brief Gets a pointer to the GJK collision object
    /// @return A pointer to the @ref ObjectCollisionBase GJK collision object, @ref m_collision
    /// @details In the base game, this is a virtual function. Since no derived class
    /// overrides this function, we can devirtualize for Kinoko.
    [[nodiscard]] ObjectCollisionBase *collision() const {
        return m_collision;
    }

    /// @endGetters

protected:
    void createCollision() override;

    [[nodiscard]] virtual const EGG::Vector3f &getCollisionTranslation() const;
    [[nodiscard]] virtual const EGG::Vector3f &collisionCenter() const;

    void registerManagedObject();

    ObjectCollisionBase *m_collision; ///< Pointer to the associated GJK collision object
};

} // namespace Field

} // namespace Kinoko
