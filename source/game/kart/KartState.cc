#include "KartState.hh"

namespace Kart {

KartState::KartState() {
    m_ground = false;
}

void KartState::init() {
    reset();
}

void KartState::reset() {
    m_ground = false;
}

bool KartState::isGround() const {
    return m_ground;
}

} // namespace Kart
