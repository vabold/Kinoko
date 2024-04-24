#include "KartBoost.hh"

namespace Kart {

KartBoost::KartBoost() : m_timers{0}, m_active{false} {
    m_multiplier = 1.0f;
    m_acceleration = 1.0f;
    m_speedLimit = -1.0f;
}

KartBoost::~KartBoost() = default;

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

f32 KartBoost::multiplier() const {
    return m_multiplier;
}

f32 KartBoost::acceleration() const {
    return m_acceleration;
}

f32 KartBoost::speedLimit() const {
    return m_speedLimit;
}

} // namespace Kart
