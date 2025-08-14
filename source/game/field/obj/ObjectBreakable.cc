#include "ObjectBreakable.hh"

/// @addr{0x8076EBE0}
ObjectBreakable::ObjectBreakable(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_collisionEnabled(false) {}

/// @addr{0x8076EC28}
ObjectBreakable::~ObjectBreakable() = default;

/// @addr{0x8076EC68}
void ObjectBreakable::load() {
    ObjectCollidable::load();
    m_collisionEnabled = true;
}

/// @addr{0x8076ED1C}
void ObjectBreakable::calc() {
    if (m_collisionEnabled) {
        calcCollisionTransform();
    }
}

Kart::Reaction ObjectBreakable::onCollision(Kart::KartObject * /*kartObj*/, Kart::Reaction reactionOnKart,
        Kart::Reaction /*reactionOnObj*/, EGG::Vector3f & /*hitDepth*/) {
    if ((reactionOnKart == Kart::Reaction::None || reactionOnKart == Kart::Reaction::UNK_7) && m_collisionEnabled) {
        m_collisionEnabled = false;
    }

    return reactionOnKart;
}


} // namespace Field