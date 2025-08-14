#include "ObjectBreakable.hh"

/// @addr{0x8076EBE0}
ObjectBreakable::ObjectBreakable(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_active(false) {}

/// @addr{0x8076EC28}
ObjectBreakable::~ObjectBreakable() = default;

/// @addr{0x8076EC68}
void ObjectBreakable::load() {
    ObjectCollidable::load();
    m_active = true;
}

/// @addr{0x8076ED1C}
void ObjectBreakable::calc() {
    if (m_active) {
        calcCollisionTransform();
    }
}

Kart::Reaction ObjectBreakable::onCollision(Kart::KartObject * /*kartObj*/, Kart::Reaction /*reactionOnKart*/,
        Kart::Reaction /*reactionOnObj*/, EGG::Vector3f & /*hitDepth*/) {
    if (m_active) {
        m_active = false;
        return Kart::Reaction::WallAllSpeed;
    }

    return Kart::Reaction::None;
}


} // namespace Field