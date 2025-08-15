#include "ObjectBreakable.hh"

/// @addr{0x8076EBE0}
ObjectBreakable::ObjectBreakable(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_state(UNINITIALIZED) {}

/// @addr{0x8076EC28}
ObjectBreakable::~ObjectBreakable() = default;

/// @addr{0x8076EC68}
void ObjectBreakable::init() {
    enableCollision();
}

/// @addr{0x8076ED1C}
void ObjectBreakable::calc() {
    switch (m_state) {
    case State::BROKEN:
        if (m_respawnTimer > 0) {
            m_respawnTimer--;

            if (m_respawnTimer == 0) {
                enableCollision();
            }
        }
        break;

    case State::RESPAWNING:
        respawnCallback();
        break;

    case State::UNINITIALIZED:
    case State::ACTIVE:
    default:
        break;
    }
}

/// @addr{0x807677E4}
[[nodiscard]] u32 ObjectBreakable::loadFlags() const override {
    return 1;
}

/// @addr{0x8076f0ac}
Kart::Reaction ObjectBreakable::onCollision(Kart::KartObject * /*kartObj*/, Kart::Reaction reactionOnKart,
        Kart::Reaction /*reactionOnObj*/, EGG::Vector3f & /*hitDepth*/) {
    if ((reactionOnKart == Kart::Reaction::None || reactionOnKart == Kart::Reaction::UNK_7) && m_collisionEnabled) {
        m_collisionEnabled = false;
    }

    return reactionOnKart;
}

// @addr{0x8076ED70}
void ObjectBreakable::enableCollision() {
    m_state = State::ACTIVE;
    m_collisionEnabled = true;
}

/// @addr{0x807677E0}
void respawnCallback() {
    return;
}

} // namespace Field