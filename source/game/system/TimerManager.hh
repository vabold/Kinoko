#include <Common.hh>

namespace System {

/// @brief A simple struct to represent a lap or race finish time.
struct Timer {
    Timer(); ///< @unused Creates a zero'd timer.
    Timer(u32 data);
    ~Timer();

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
    [[nodiscard]] const Timer &currentTimer() const;
    /// @endGetters

    /// @beginSetters
    void setStarted(bool isSet);
    /// @endSetters

private:
    Timer m_currentTimer;
    bool m_started;
    u32 m_frameCounter;
};

} // namespace System
