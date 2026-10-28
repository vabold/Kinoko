#include "TimerManager.hh"

namespace Kinoko::System {

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
    m_started = false;
    m_frameCounter = 0;
}

/// @addr{0x80535904}
/// @note For Kinoko, we omit per-frame @ref Timer calculation as a performance improvement. Upon
/// lap completion, @ref m_frameCounter is used to compute the lap/race time.
void TimerManager::calc() {
    if (!m_started) {
        return;
    }

    ++m_frameCounter;
}

/// @brief Computes the current race timer
/// @return A @ref Timer storing the current race time
Timer TimerManager::currentTimer() const {
    u32 minutesMs = CalcTotalMs(m_frameCounter - 1);
    u16 minutes = CalcMinutes(minutesMs);
    u32 secondsMs = static_cast<u32>(
            static_cast<f32>(minutesMs) - static_cast<f32>(minutes) * MINUTES_TO_MILLISECONDS);
    u8 seconds = static_cast<u8>(static_cast<f32>(secondsMs) * MILLISECONDS_TO_SECONDS);
    u16 milliseconds = static_cast<u16>(
            static_cast<f32>(secondsMs) - static_cast<f32>(seconds) * SECONDS_TO_MILLISECONDS);

    return Timer(minutes, seconds, milliseconds);
}

} // namespace Kinoko::System
