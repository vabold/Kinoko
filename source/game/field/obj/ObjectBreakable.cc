#include "ObjectBreakable.hh"

/// @addr{0x8076EBE0}
ObjectBreakable::ObjectBreakable(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_breakableState(UNINITIALIZED), m_collisionEnabled(false) {}

/// @addr{0x8076EC28}
ObjectBreakable::~ObjectBreakable() = default;

/// @addr{0x8076EC68}
void ObjectBreakable::load() {
    ObjectCollidable::load();
    m_collisionEnabled = true;
    // m_breakableState is set to ACTIVE by inherited classes, see 8077E8F4
}

/// @addr{0x8076ED1C}
void ObjectBreakable::calc() {
    switch (m_breakableState) {
    case BreakableState::BROKEN:
        if (m_respawnTimer > 0) {
            m_respawnTimer--;

            if (m_respawnTimer == 0) {
                enableCollision();
            }
        }
        break;

    case BreakableState::HIT: // GP mode spits out an item here {0x8076EdE4}
    case BreakableState::UNINITIALIZED:
    case BreakableState::ACTIVE:
    default:;
    }
}

// @addr{0x8076ED70}
void enableCollision() {
    m_breakableState = BreakableState::ACTIVE;
    m_collisionEnabled = true;
}

Kart::Reaction ObjectBreakable::onCollision(Kart::KartObject * /*kartObj*/, Kart::Reaction reactionOnKart,
        Kart::Reaction /*reactionOnObj*/, EGG::Vector3f & /*hitDepth*/) {
    if ((reactionOnKart == Kart::Reaction::None || reactionOnKart == Kart::Reaction::UNK_7) && m_collisionEnabled) {
        m_collisionEnabled = false;
    }

    return reactionOnKart;
}

} // namespace Field