#include "ObjectCollidable.hh"

#include "game/field/BoxColManager.hh"
#include "game/field/ObjectCollisionBox.hh"
#include "game/field/ObjectCollisionCylinder.hh"
#include "game/field/ObjectCollisionSphere.hh"
#include "game/field/ObjectDirector.hh"

#include "game/kart/KartObject.hh"
#include "game/kart/Reaction.hh"

namespace Kinoko::Field {

/// @addr{0x8081EFEC}
/// @copybrief ObjectBase::ObjectBase(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Initializes @ref m_collision to `nullptr`.
ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &params)
    : ObjectBase(params), m_collision(nullptr) {}

/// @addr{0x8081F064}
/// @copybrief ObjectBase::ObjectBase(const char *, const EGG::Vector3f &, const EGG::Vector3f
/// &, const EGG::Vector3f &)
/// @param name The name of the object
/// @param pos The initial position of the object
/// @param rot The initial rotation of the object
/// @param scale The initial scale of the object
/// @details Initializes @ref m_collision to `nullptr`.
ObjectCollidable::ObjectCollidable(const char *name, const EGG::Vector3f &pos,
        const EGG::Vector3f &rot, const EGG::Vector3f &scale)
    : ObjectBase(name, pos, rot, scale), m_collision(nullptr) {}

/// @addr{0x8067E384}
/// @brief Virtual destructor that destroys the associated collision object
ObjectCollidable::~ObjectCollidable() {
    EGG::egg_delete(m_collision);
}

/// @addr{0x8081F0A0}
/// @copybrief ObjectBase::load()
/// @details This function loads the graphical assets, animations, collision object(s), and rail for
/// the collidable object. It also registers the object with the @ref Field::ObjectDirector.
void ObjectCollidable::load() {
    loadGraphics();
    loadAnims();
    createCollision();

    if (m_collision) {
        loadAABB(0.0f);
    }

    loadRail();

    ObjectDirector::Instance()->addObject(this);
}

/// @addr{0x8081F7C8}
/// @copybrief ObjectBase::calcCollisionTransform()
/// @details Updates the GJK collision transform.
void ObjectCollidable::calcCollisionTransform() {
    calcTransform();
    m_collision->transform(transform(), scale(), getCollisionTranslation());
}

/// @addr{0x806815A0}
/// @copybrief ObjectBase::getCollisionRadius()
/// @return The collision radius of the object, calculated based on its bounding box.
/// @details We refer to the collision parameters as a box due to its use of axes.
/// This does not imply that all collidable objects are boxes!
f32 ObjectCollidable::getCollisionRadius() const {
    const auto &flowTable = ObjectDirector::Instance()->flowTable();
    const auto *collisionSet = flowTable.set(flowTable.slot(id()));

    f32 zRadius = scale().z * static_cast<f32>(parse<s16>(collisionSet->params.box.z));
    f32 xRadius = scale().x * static_cast<f32>(parse<s16>(collisionSet->params.box.x));

    return std::max(xRadius, zRadius);
}

/// @addr{0x806816D8}
/// @brief Creates a BoxColUnit based off the collision radius and the provided maxSpeed
/// @param maxSpeed The maximum speed of the object
void ObjectCollidable::loadAABB(f32 maxSpeed) {
    loadAABB(getCollisionRadius(), maxSpeed);
}

/// @addr{0x8081F180}
/// @brief Creates a @ref BoxColUnit based off the provided radius and maxSpeed
/// @param radius The collision radius of the object
/// @param maxSpeed The maximum speed of the object
void ObjectCollidable::loadAABB(f32 radius, f32 maxSpeed) {
    auto *boxColMgr = BoxColManager::Instance();
    const EGG::Vector3f &pos = getPosition();
    bool alwaysRecalc = loadFlags().onAnyBit(eLoadFlags::Calc, eLoadFlags::AlwaysRecalc);
    m_boxColUnit = boxColMgr->insertObject(radius, maxSpeed, &pos, alwaysRecalc, this);
}

/// @addr{0x8081F66C}
/// @brief Runs on collision to conditionally modify the hit reaction applied on the player
/// @param kartObj The kart object involved in the collision
/// @param reactionOnKart The reaction that should be applied to the kart upon collision
/// @param reactionOnObj The reaction that should be applied to the object upon collision
/// @details If the kart is driving under 50% of its base speed, then it changes @ref
/// Kart::Reaction::SpinSomeSpeed to @ref Kart::Reaction::WallAllSpeed and @ref
/// Kart::Reaction::SpinHitSomeSpeed to @ref Kart::Reaction::None. Otherwise, if the kart is driving
/// at 50% or more of its base speed, it changes @ref Kart::Reaction::SpinHitSomeSpeed to @ref
/// Kart::Reaction::SpinSomeSpeed.
void ObjectCollidable::processKartReactions(Kart::KartObject *kartObj,
        Kart::Reaction &reactionOnKart, Kart::Reaction &reactionOnObj) {
    // Process the reaction on kart
    if (kartObj->speedRatioCapped() < 0.5f) {
        if (reactionOnKart == Kart::Reaction::SpinSomeSpeed) {
            reactionOnKart = Kart::Reaction::WallAllSpeed;
        } else if (reactionOnKart == Kart::Reaction::SpinHitSomeSpeed) {
            reactionOnKart = Kart::Reaction::None;
        }
    } else {
        if (reactionOnKart == Kart::Reaction::SpinHitSomeSpeed) {
            reactionOnKart = Kart::Reaction::SpinSomeSpeed;
        }
    }

    // Process the reaction on object
    if (reactionOnObj == Kart::Reaction::UNK_3 || reactionOnObj == Kart::Reaction::UNK_4) {
        reactionOnObj = Kart::Reaction::None;
    }
}

/// @addr{0x8068179C}
/// @brief Called when a collision occurs between a kart and this object
/// @param kartObj The kart object involved in the collision
/// @param reactionOnKart The reaction that should be applied to the kart upon collision
/// @param reactionOnObj The reaction that should be applied to the object upon collision
/// @param hitDepth The depth of the collision between the kart and the object
/// @return The reaction that should be applied to the kart
Kart::Reaction ObjectCollidable::onCollision(Kart::KartObject * /*kartObj*/,
        Kart::Reaction reactionOnKart, Kart::Reaction /*reactionOnObj*/,
        EGG::Vector3f & /*hitDepth*/) {
    return reactionOnKart;
}

/// @addr{0x80681748}
/// @brief Performs a collision check between this object and another collision object
/// @param lhs The object to check collision against (usually the player)
/// @param dist If a collision occurs, set to the distance between the two objects
/// @return Whether or not a collision occurred
bool ObjectCollidable::checkCollision(ObjectCollisionBase *lhs, EGG::Vector3f &dist) {
    return lhs->check(*collision(), dist);
}

/// @addr{0x8081F224}
/// @copybrief ObjectBase::createCollision()
/// @details References the object's @ref SObjectCollisionSet::mode to determine what type of object
/// to create. It is either a sphere, cylinder, or box collision object.
/// @note In the base game, this function is a no-op for objects having collision mode 0. To better
/// catch instances where an object may have overridden this function, we instead #PANIC.
void ObjectCollidable::createCollision() {
    const auto &flowTable = ObjectDirector::Instance()->flowTable();
    const auto *collisionSet = flowTable.set(flowTable.slot(id()));

    if (!collisionSet) {
        PANIC("Invalid object ID when creating primitive collision! ID: %zu",
                static_cast<size_t>(id()));
    }

    switch (static_cast<CollisionMode>(parse<s16>(collisionSet->mode))) {
    case CollisionMode::Sphere:
        m_collision = EGG::egg_new<ObjectCollisionSphere>(
                parse<s16>(collisionSet->params.sphere.radius), collisionCenter());
        break;
    case CollisionMode::Cylinder:
        m_collision = EGG::egg_new<ObjectCollisionCylinder>(
                parse<s16>(collisionSet->params.cylinder.radius),
                parse<s16>(collisionSet->params.cylinder.height), collisionCenter());
        break;
    case CollisionMode::Box:
        m_collision = EGG::egg_new<ObjectCollisionBox>(parse<s16>(collisionSet->params.box.x),
                parse<s16>(collisionSet->params.box.y), parse<s16>(collisionSet->params.box.z),
                collisionCenter());
        break;
    default:
        PANIC("Invalid collision mode when creating primitive collision! ID: %zu; Mode: %d",
                static_cast<size_t>(id()), parse<s16>(collisionSet->mode));
        break;
    }
}

/// @addr{0x8068173C}
/// @brief The translation applied on top of the object's world transform for collision
/// checks
/// @return The translation vector applied on top of the object's world transform for
/// collision checks
const EGG::Vector3f &ObjectCollidable::getCollisionTranslation() const {
    return EGG::Vector3f::zero;
}

/// @addr{0x806816B8}
/// @brief Defines a local center for the GJK collision object
/// @return The local center of the GJK collision object, defaulted to @ref EGG::Vector3f::zero.
const EGG::Vector3f &ObjectCollidable::collisionCenter() const {
    return EGG::Vector3f::zero;
}

/// @addr{0x8081F170}
/// @brief Registers this object to the ObjectDirector's vector of managed objects
void ObjectCollidable::registerManagedObject() {
    ObjectDirector::Instance()->addManagedObject(this);
}

} // namespace Kinoko::Field
