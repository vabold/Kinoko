#pragma once

#include <Common.hh>

namespace EGG::Decomp {

s32 GetExpandSize(const u8 *src);
s32 DecodeSZS(const u8 *src, u8 *dst);

} // namespace EGG::Decomp
