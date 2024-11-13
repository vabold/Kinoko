#include "ObjectCollidable.hh"

#include "game/field/ObjectCollisionBox.hh"
#include "game/field/ObjectCollisionCylinder.hh"
#include "game/field/ObjectCollisionSphere.hh"
#include "game/field/ObjectDirector.hh"

#include "game/kart/KartObject.hh"

namespace Field {

/// @addr{0x8081EFEC}
ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &params) : ObjectBase(params) {}

/// @addr{0x8067E384}
ObjectCollidable::~ObjectCollidable() = default;

/// @addr{0x8081F0A0}
void ObjectCollidable::load() {
    createCollision();

    if (m_collision) {
        loadAABB(0.0f);
    }

    ObjectDirector::Instance()->addObject(this);
}

/// @addr{0x8081F7C8}
void ObjectCollidable::calcCollisionTransform() {
    calcTransform();
    m_collision->transform(m_transform, m_scale, getCollisionTranslation());
}

/// @addr{0x806815A0}
/// @brief Finds the radius that fits fully in a BoxColUnit.
/// @details We refer to the collision parameters as a box due to its use of axes.
/// This does not imply that all collidable objects are boxes!
f32 ObjectCollidable::getCollisionRadius() const {
    const auto &flowTable = ObjectDirector::Instance()->flowTable();
    const auto *collisionSet = flowTable.set(flowTable.slot(m_id));

    f32 zRadius = m_scale.z * static_cast<f32>(parse<s16>(collisionSet->params.box.z));
    f32 xRadius = m_scale.x * static_cast<f32>(parse<s16>(collisionSet->params.box.x));

    return std::max(xRadius, zRadius);
}

/// @addr{0x806816D8}
void ObjectCollidable::loadAABB(f32 maxSpeed) {
    loadAABB(getCollisionRadius(), maxSpeed);
}

/// @addr{0x8081F180}
void ObjectCollidable::loadAABB(f32 radius, f32 maxSpeed) {
    auto *boxColMgr = BoxColManager::Instance();
    const EGG::Vector3f &pos = getPosition();
    bool alwaysRecalc = loadFlags() & 0x5;
    m_boxColUnit = boxColMgr->insertObject(radius, maxSpeed, &pos, alwaysRecalc, this);
}

/// @addr{0x8081F66C}
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
Kart::Reaction ObjectCollidable::onCollision(Kart::KartObject * /*kartObj*/,
        Kart::Reaction reactionOnKart, Kart::Reaction /*reactionOnObj*/,
        const EGG::Vector3f & /*hitDepth*/) {
    return reactionOnKart;
}

/// @addr{0x80681748}
bool ObjectCollidable::checkCollision(ObjectCollisionBase *lhs, EGG::Vector3f &dist) {
    return lhs->check(*collision(), dist);
}

/// @addr{0x8068173C}
const EGG::Vector3f &ObjectCollidable::getCollisionTranslation() const {
    return EGG::Vector3f::zero;
}

/// @addr{0x8081F224}
void ObjectCollidable::createCollision() {
    const auto &flowTable = ObjectDirector::Instance()->flowTable();
    const auto *collisionSet = flowTable.set(flowTable.slot(m_id));

    if (!collisionSet) {
        PANIC("Invalid object ID when creating primitive collision! ID: %d",
                static_cast<size_t>(m_id));
    }

    switch (static_cast<CollisionMode>(parse<s16>(collisionSet->mode))) {
    case CollisionMode::Sphere:
        m_collision = new ObjectCollisionSphere(parse<s16>(collisionSet->params.sphere.radius),
                collisionCenter());
        break;
    case CollisionMode::Cylinder:
        m_collision = new ObjectCollisionCylinder(parse<s16>(collisionSet->params.cylinder.radius),
                parse<s16>(collisionSet->params.cylinder.height), collisionCenter());
        break;
    case CollisionMode::Box:
        m_collision = new ObjectCollisionBox(parse<s16>(collisionSet->params.box.x),
                parse<s16>(collisionSet->params.box.y), parse<s16>(collisionSet->params.box.z),
                collisionCenter());
        break;
    default:
        PANIC("Invalid collision mode when creating primitive collision! ID: %d; Mode: %d",
                static_cast<size_t>(m_id), parse<s16>(collisionSet->mode));
        break;
    }
}

/// @addr{0x806816B8}
const EGG::Vector3f &ObjectCollidable::collisionCenter() const {
    return EGG::Vector3f::zero;
}

/// @addr{0x80573518}
ObjectCollisionBase *ObjectCollidable::collision() const {
    return m_collision;
}

} // namespace Field
