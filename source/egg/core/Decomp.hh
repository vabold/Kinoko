#pragma once

#include <Common.hh>

namespace Kinoko::EGG::Decomp {

/// @addr{0x802198F8}
/// @brief Checks if the given data is compressed, indicated by the first three bytes being set to
/// `Yaz`
/// @param src Pointer to the data to check
/// @return `true` if the data is compressed, `false` otherwise
/// @details In the base game, this function normally returns an enum to account for other
/// compression formats (`ASH` / `ASR`). We only need to handle Yaz0 compression for Kinoko.
[[nodiscard]] inline constexpr bool CheckCompressed(const u8 *src) {
    return src[0] == 'Y' && src[1] == 'a' && src[2] == 'z';
}

/// @addr{0x8021997C}
[[nodiscard]] inline constexpr s32 GetExpandSize(const u8 *src) {
    return CheckCompressed(src) ? form<s32>(&src[4]) : -1;
}

s32 DecodeSZS(const u8 *src, u8 *dst);

} // namespace Kinoko::EGG::Decomp
