#include "KartBody.hh"

namespace Kart {

KartBody::KartBody(KartPhysics *physics) : m_physics(physics) {}

void KartBody::reset() {
    m_physics->reset();
}

KartPhysics *KartBody::getPhysics() const {
    return m_physics;
}

KartBodyKart::KartBodyKart(KartPhysics *physics) : KartBody(physics) {}

KartBodyBike::KartBodyBike(KartPhysics *physics) : KartBody(physics) {}

} // namespace Kart
