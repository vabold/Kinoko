#include "TimerManager.hh"

namespace System {

/// @addr{0x8051C374}
Timer::Timer() : min(0), sec(0), mil(0), valid(false) {}

Timer::Timer(u16 min_, u8 sec_, u16 mil_) : min(min_), sec(sec_), mil(mil_), valid(true) {}

/// Parses a time from an RKG's 3 byte time format.
/// @see RawGhostFile
Timer::Timer(u32 data) {
    min = static_cast<u16>(data >> 0x19);
    sec = static_cast<u8>(data >> 0x12) & 0x7F;
    mil = static_cast<u16>(data >> 8) & 0x3FF;
    valid = true;
}

/// @addr{0x8051C334}
Timer::~Timer() = default;

/// @addr{Inlined in 0x805327A0}
TimerManager::TimerManager() {
    init();
}

/// @addr{0x805376E0}
TimerManager::~TimerManager() = default;

/// @addr{0x80535864}
void TimerManager::init() {
    m_currentTimer.min = 0;
    m_currentTimer.sec = 0;
    m_currentTimer.mil = 0;
    m_currentTimer.valid = true;

    m_started = false;
    m_frameCounter = 0;
}

/// @addr{0x80535904}
void TimerManager::calc() {
    constexpr f32 REFRESH_PERIOD = 1000.0f / 59.94f;
    constexpr f32 MILLISECONDS_TO_MINUTES = 1.0f / 60000.0f;
    constexpr f32 MILLISECONDS_TO_SECONDS = 1.0f / 1000.0f;
    constexpr f32 SECONDS_TO_MILLISECONDS = 1000.0f;
    constexpr f32 MINUTES_TO_MILLISECONDS = 60000.0f;

    if (!m_started) {
        return;
    }

    u32 minutesMs = static_cast<u32>(static_cast<f32>(m_frameCounter) * REFRESH_PERIOD);
    u16 minutes = static_cast<u16>(static_cast<f32>(minutesMs) * MILLISECONDS_TO_MINUTES);
    u32 secondsMs = static_cast<u32>(
            static_cast<f32>(minutesMs) - static_cast<f32>(minutes) * MINUTES_TO_MILLISECONDS);
    u8 seconds = static_cast<u8>(static_cast<f32>(secondsMs) * MILLISECONDS_TO_SECONDS);
    u16 milliseconds = static_cast<u16>(
            static_cast<f32>(secondsMs) - static_cast<f32>(seconds) * SECONDS_TO_MILLISECONDS);

    m_currentTimer.min = minutes;
    m_currentTimer.sec = seconds;
    m_currentTimer.mil = milliseconds;
    m_currentTimer.valid = true;

    ++m_frameCounter;
}

} // namespace System
