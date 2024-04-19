#pragma once

#include "Common.hh"

namespace System {

/// @brief A simple struct to represent a lap or race finish time.
struct Timer {
    Timer(); ///< @unused Creates a zero'd timer.
    Timer(u32 data);
    ~Timer();

    u16 min;
    u8 sec;
    u16 mil; ///< @todo We will likely want to expand this to a float for more precise finish times.
};

} // namespace System
