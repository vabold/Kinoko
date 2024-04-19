#include "Timer.hh"

namespace System {

/// @addr{0x8051C374}
Timer::Timer() : min(0), sec(0), mil(0) {}

/// Parses a time from an RKG's 3 byte time format.
/// @see RawGhostFile
Timer::Timer(u32 data) {
    min = static_cast<u16>(data >> 0x19);
    sec = static_cast<u8>(data >> 0x12) & 0x7F;
    mil = static_cast<u16>(data >> 8) & 0x3FF;
}

/// @addr{0x8051C334}
Timer::~Timer() = default;

} // namespace System
