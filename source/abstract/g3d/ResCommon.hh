#pragma once

// Credit: kiwi515/ogws

namespace Kinoko::Abstract::g3d {

struct ResBlockHeaderData {
    u32 signature;
    u32 size;
};
STATIC_ASSERT(sizeof(ResBlockHeaderData) == 0x8);

struct ResFileHeaderData {
    u32 signature;
    u16 byteOrder;
    u16 version;
    u32 fileSize;
    u16 headerSize;
    u16 dataBlocks;
};
STATIC_ASSERT(sizeof(ResFileHeaderData) == 0x10);

} // namespace Kinoko::Abstract::g3d
