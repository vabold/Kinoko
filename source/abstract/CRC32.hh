#pragma once

#include "Common.hh"

namespace Abstract {

/// @addr{0x801D1CA0}
[[nodiscard]] constexpr u32 CalcCRC32(u8 *data, u32 size) {
    /// @addr{0x80252F50}
    constexpr u32 LOOKUP_TABLE[16] = {
            0x00000000,
            0x1DB71064,
            0x3B6E20C8,
            0x26D930AC,
            0x76DC4190,
            0x6B6B51F4,
            0x4DB26158,
            0x5005713C,
            0xEDB88320,
            0xF00F9344,
            0xD6D6A3E8,
            0xCB61B38C,
            0x9B64C2B0,
            0x86D3D2D4,
            0xA00AE278,
            0xBDBDF21C,
    };

    u32 ret = std::numeric_limits<u32>::max();

    for (u32 i = 0; i < size; ++i) {
        ret = (ret >> 4) ^ LOOKUP_TABLE[(ret ^ data[i]) & 0xF];
        ret = (ret >> 4) ^ LOOKUP_TABLE[(ret ^ (data[i] >> 4)) & 0xF];
    }

    return ~ret;
}

} // namespace Abstract
