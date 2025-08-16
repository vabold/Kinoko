#include "ObjectBreakable.hh"

namespace Field {

/// @addr{0x8076EBE0}
ObjectBreakable::ObjectBreakable(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_state(State::Uninitialized) {}

/// @addr{0x8076EC28}
ObjectBreakable::~ObjectBreakable() = default;

/// @addr{0x8076ED1C}
void ObjectBreakable::calc() {
    switch (m_state) {
    case State::Broken:
        if (m_respawnTimer > 0) {
            if (--m_respawnTimer == 0) {
                onTimerFinish();
            }
        }
        break;

    case State::Respawning:
        onRespawn();
        break;

    case State::Uninitialized:
    case State::Active:
    default:
        break;
    }
}

/// @addr{0x8076f0ac}
Kart::Reaction ObjectBreakable::onCollision(Kart::KartObject * /*kartObj*/,
        Kart::Reaction reactionOnKart, Kart::Reaction /*reactionOnObj*/,
        EGG::Vector3f & /*hitDepth*/) {
    if (reactionOnObj == UNK_3) {
        onBreak(kartObj);
    }

    if (reactionOnObj == UNK_5) {
        // GP implementation? @addr{0x8076F1D4}
    }

    if ((reactionOnKart == Kart::Reaction::None || reactionOnKart == Kart::Reaction::UNK_7) &&
            m_collisionEnabled) {
        m_collisionEnabled = false;
    }

    return reactionOnKart;
}

} // namespace Field
