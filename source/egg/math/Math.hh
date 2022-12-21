#pragma once

#include <Common.hh>

#define DEG2RAD(x) 0.017453292f * x

namespace EGG::Mathf {

f32 sqrt(f32 x);
f32 frsqrt(f32 x);

f32 sin(f32 x);
f32 cos(f32 x);

} // namespace EGG::Mathf
