#include "KartBoost.hh"

namespace Kart {

/// @addr{0x80588D28}
KartBoost::KartBoost() : m_timers{0}, m_active{false} {
    m_multiplier = 1.0f;
    m_acceleration = 1.0f;
    m_speedLimit = -1.0f;
}

/// @addr{0x8057811C}
KartBoost::~KartBoost() = default;

/// @stage 2
/// @brief Starts/restarts a boost of the given type
/// @addr{0x80588DB0}
/// @param type The type of boost (e.g. mini-turbo)
/// @param frames The duration of the boost
/// @return whether the boost was activated
bool KartBoost::activate(Type type, s16 frames) {
    bool activated = false;

    size_t t = static_cast<size_t>(type);
    if (!m_active[t] || m_timers[t] < frames) {
        m_timers[t] = frames;
        activated = true;
        m_active[t] = true;
    }

    return activated;
}

/// @stage 2
/// @brief Computes the current frame's boost multiplier, acceleration, and speed limit.
/// @addr{0x80588E24}
/// @return whether a boost is active
bool KartBoost::calc() {
    static constexpr std::array<f32, BOOST_TYPE_COUNT> MULTIPLIERS = {{
            0.2f,
            0.4f,
            0.3f,
    }};

    static constexpr std::array<f32, BOOST_TYPE_COUNT> ACCELERATIONS = {{
            3.0f,
            7.0f,
            6.0f,
    }};

    static constexpr std::array<f32, BOOST_TYPE_COUNT> LIMITS = {{
            -1.0f,
            115.0f,
            -1.0f,
    }};

    m_multiplier = 1.0f;
    m_acceleration = 1.0f;
    m_speedLimit = -1.0f;

    for (size_t i = 0; i < BOOST_TYPE_COUNT; ++i) {
        if (!m_active[i]) {
            continue;
        }

        m_multiplier = 1.0f + MULTIPLIERS[i];
        m_acceleration = ACCELERATIONS[i];
        m_speedLimit = LIMITS[i];

        if (--m_timers[i] <= 0) {
            m_active[i] = false;
        }
    }

    return m_multiplier > 1.0f || m_speedLimit > 0.0f;
}

/// @addr{0x80588D74}
void KartBoost::reset() {
    m_timers.fill(0);
    m_multiplier = 1.0f;
    m_acceleration = 1.0f;
    m_speedLimit = -1.0f;
}

} // namespace Kart
