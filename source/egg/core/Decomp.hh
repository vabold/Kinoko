#pragma once

#include <Common.hh>

namespace EGG::Decomp {

s32 getExpandSize(const u8 *src);
s32 decodeSZS(const u8 *src, u8 *dst);

} // namespace EGG::Decomp
