#pragma once

#include <Common.hh>

namespace Kinoko::System {

/// @brief A simple struct to represent a lap or race finish time.
struct Timer {
    Timer(); ///< @unused Creates a zeroed timer.
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

    [[nodiscard]] Timer currentTimer() const;

    /// @brief Retrieves the number of frames the timer has been updated since the race started
    /// @return The number of frames since the race started, @ref m_frameCounter
    /// @details @ref currentTimer() reflects frame `m_frameCounter - 1`, or zero if no frames
    /// have elapsed.
    [[nodiscard]] u32 frameCounter() const {
        return m_frameCounter;
    }

    /// @endGetters

    /// @beginSetters
    void setStarted(bool isSet) {
        m_started = isSet;
    }
    /// @endSetters

    /// @brief Finds the first frame counter value whose timer reads at least `minutes` minutes
    /// @param minutes The number of minutes to search for
    /// @return The first frame counter value whose timer reads at least `minutes` minutes
    /// @details Kinoko-specific. The timer's minutes never decrease as the frame counter increases,
    /// so callers can compare against this instead of converting the timer every frame.
    [[nodiscard]] static consteval u32 FirstFrameWithMinutes(u16 minutes) {
        u32 frame = 0;
        while (CalcMinutes(CalcTotalMs(frame)) < minutes) {
            ++frame;
        }

        return frame;
    }

private:
    bool m_started;     ///< Indicates whether the race has started
    u32 m_frameCounter; ///< The number of frames since the race started

    static constexpr f32 REFRESH_PERIOD = 1000.0f / 59.94f;
    static constexpr f32 MILLISECONDS_TO_MINUTES = 1.0f / 60000.0f;
    static constexpr f32 MILLISECONDS_TO_SECONDS = 1.0f / 1000.0f;
    static constexpr f32 SECONDS_TO_MILLISECONDS = 1000.0f;
    static constexpr f32 MINUTES_TO_MILLISECONDS = 60000.0f;

    /// @brief Converts a frame counter value to the total number of elapsed milliseconds
    [[nodiscard]] static constexpr u32 CalcTotalMs(u32 frame) {
        return static_cast<u32>(static_cast<f32>(frame) * REFRESH_PERIOD);
    }

    /// @brief Converts a total number of elapsed milliseconds to whole minutes
    [[nodiscard]] static constexpr u16 CalcMinutes(u32 totalMs) {
        return static_cast<u16>(static_cast<f32>(totalMs) * MILLISECONDS_TO_MINUTES);
    }
};

} // namespace Kinoko::System
