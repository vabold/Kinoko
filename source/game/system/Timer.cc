#include "Timer.hh"

namespace System {

Timer::Timer() : min(0), sec(0), mil(0) {}

Timer::Timer(u32 data) {
    min = static_cast<u16>(data >> 0x19);
    sec = static_cast<u8>(data >> 0x12) & 0x7F;
    mil = static_cast<u16>(data >> 8) & 0x3FF;
}

Timer::~Timer() = default;

} // namespace System
