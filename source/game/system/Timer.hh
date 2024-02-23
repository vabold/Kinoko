#pragma once

#include "Common.hh"

namespace System {

struct Timer {
    Timer();
    Timer(u32 data);
    ~Timer();

    u16 min;
    u8 sec;
    u16 mil;
};

} // namespace System
