#pragma once

#include <Common.hh>

namespace System {

/// @brief A simple struct to represent a lap or race finish time.
struct Timer {
    Timer(); ///< @unused Creates a zero'd timer.
    Timer(u16 min_, u8 sec_, u16 mil_);
    Timer(u32 data);
    ~Timer();

    std::strong_ordering operator<=>(const Timer &rhs) const {
        if (auto cmp = min <=> rhs.min; cmp != 0) {
            return cmp;
        }

        if (auto cmp = sec <=> rhs.sec; cmp != 0) {
            return cmp;
        }

        if (auto cmp = mil <=> rhs.mil; cmp != 0) {
            return cmp;
        }

        return valid <=> rhs.valid;
    }

    bool operator==(const Timer &rhs) const = default;
    bool operator!=(const Timer &rhs) const = default;

    /// @addr{0x807EE860}
    Timer operator-(const Timer &rhs) const {
        s16 addMin = 0;
        s16 addSec = 0;

        s16 newMs = mil - rhs.mil;
        if (newMs < 0) {
            addSec = -1;
            newMs += 1000;
        }

        s16 newSec = addSec + sec - rhs.sec;
        if (newSec < 0) {
            addMin = -1;
            newSec += 60;
        }

        s16 newMin = addMin + min - rhs.min;
        if (newMin < 0) {
            newMin = 0;
            newSec = 0;
            newMs = 0;
        }

        return Timer(newMin, newSec, newMs);
    }

    Timer operator+(f32 ms) const {
        s16 addMin = 0;
        s16 addSec = 0;

        s16 newMs = static_cast<s16>(ms + static_cast<f32>(mil));
        if (newMs > 999) {
            addSec = 1;
            newMs -= 1000;
        }

        s16 newSec = addSec + sec;
        if (newSec > 59) {
            addMin = 1;
            newSec -= 60;
        }

        s16 newMin = addMin + min;
        if (newMin > 999) {
            newMin = 999;
            newSec = 59;
            newMs = 999;
        }

        return Timer(newMin, newSec, newMs);
    }

    u16 min;
    u8 sec;
    u16 mil; ///< @todo We will likely want to expand this to a float for more precise finish times.
    bool valid;
};

/// @brief Manages the race timer to create lap splits and final times.
class TimerManager {
public:
    TimerManager();
    ~TimerManager();

    void init();
    void calc();

    /// @beginGetters
    [[nodiscard]] const Timer &currentTimer() const {
        return m_currentTimer;
    }
    /// @endGetters

    /// @beginSetters
    void setStarted(bool isSet) {
        m_started = isSet;
    }
    /// @endSetters

private:
    Timer m_currentTimer;
    bool m_started;
    u32 m_frameCounter;
};

} // namespace System
