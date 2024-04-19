#include "Decomp.hh"

namespace EGG::Decomp {

/// @addr{0x8021997C}
s32 GetExpandSize(const u8 *src) {
    if (src[0] == 'Y' && src[1] == 'a' && src[2] == 'z') {
        return form<s32>(&src[4]);
    }

    return -1;
}

/// @brief Performs YAZ0 decompression on a given buffer.
/// @return The size of the decompressed data.
/// @addr{0x80218C2C}
s32 DecodeSZS(const u8 *src, u8 *dst) {
    s32 expandSize = GetExpandSize(src);
    s32 srcIdx = 0x10;
    u8 code = 0;

    u8 byte;

    for (s32 destIdx = 0; destIdx < expandSize; code >>= 1) {
        if (!code) {
            code = 0x80;
            byte = src[srcIdx++];
        }

        // Direct copy (code bit = 1)
        if (byte & code) {
            dst[destIdx++] = src[srcIdx++];
        }
        // RLE compressed data (code bit = 0)
        else {
            // Lower nibble of byte1 + byte2
            s32 distToDest = (src[srcIdx] << 8) | src[srcIdx + 1];
            srcIdx += sizeof(u8) * 2;
            s32 runSrcIdx = destIdx - (distToDest & 0xfff);

            // Upper nibble of byte 1
            s32 runLen = ((distToDest >> 12) == 0) ? src[srcIdx++] + 0x12 : (distToDest >> 12) + 2;

            for (; runLen > 0; runLen--, destIdx++, runSrcIdx++) {
                dst[destIdx] = dst[runSrcIdx - 1];
            }
        }
    }
    return expandSize;
}

} // namespace EGG::Decomp
