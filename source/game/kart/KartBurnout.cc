#include "KartBurnout.hh"

#include "game/kart/KartPhysics.hh"
#include "game/kart/KartState.hh"

#include <egg/math/Math.hh>

namespace Kart {

/// @addr{inlined in 0x80577FC4}
KartBurnout::KartBurnout() = default;

/// @addr{0x805781DC}
KartBurnout::~KartBurnout() = default;

/// @addr{0x805890B0}
void KartBurnout::start() {
    activate();
    m_timer = 0;
    m_phase = 0;
    m_amplitude = 1.0f;
}

/// @addr{0x80589118}
void KartBurnout::calc() {
    constexpr u32 BURNOUT_DURATION = 120;

    if (!isActive()) {
        return;
    }

    calcRotation();

    if (calcEnd(BURNOUT_DURATION)) {
        deactivate();
    }
}

/// @addr{0x80589308}
void KartBurnout::calcRotation() {
    constexpr u16 PHASE_INCREMENT = 800;
    constexpr f32 PHASE_TO_FIDX = 1.0f / 256.0f;
    constexpr f32 AMPLITUDE_FACTOR = 40.0f;
    constexpr f32 DAMPENING_THRESHOLD = 30.0f;
    constexpr f32 DAMPENING_FACTOR = 0.97f;

    m_phase += PHASE_INCREMENT;

    f32 sin = EGG::Mathf::SinFIdx(static_cast<f32>(m_phase) * PHASE_TO_FIDX);

    // Apply a dampening effect after burning out for 30 frames
    if (static_cast<f32>(m_timer) > DAMPENING_THRESHOLD) {
        m_amplitude *= DAMPENING_FACTOR;
    }

    f32 pitch = DEG2RAD * (AMPLITUDE_FACTOR * sin) * m_amplitude;

    EGG::Quatf rpy;
    rpy.setRPY(EGG::Vector3f(0.0f, pitch, 0.0f));

    physics()->composeStuntRot(rpy);
}

/// @addr{0x8058920C}
bool KartBurnout::calcEnd(u32 duration) {
    return ++m_timer >= duration;
}

/// @addr{0x80589844}
void KartBurnout::activate() {
    state()->setBurnout(true);
}

/// @addr{0x80589818}
void KartBurnout::deactivate() {
    state()->setBurnout(false);
}

/// @addr{0x80589830}
bool KartBurnout::isActive() const {
    return state()->isBurnout();
}

} // namespace Kart
