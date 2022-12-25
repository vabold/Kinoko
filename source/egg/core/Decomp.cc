#include "Decomp.hh"

#define getSZSExpandSize(src) ((src[4] << 24) | (src[5] << 16) | (src[6] << 8) | src[7])

namespace EGG::Decomp {

s32 getExpandSize(const u8 *src) {
    if (src[0] == 'Y' && src[1] == 'a' && src[2] == 'z') {
        return getSZSExpandSize(src);
    }

    return -1;
}

s32 decodeSZS(const u8 *src, u8 *dst) {
    s32 expandSize = getSZSExpandSize(src);
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
