#include "Math.hh"

namespace EGG::Mathf {

f32 sqrt(f32 x) {
  return x > 0.0 ? frsqrt(x) * x : 0.0;
}

f32 frsqrt(f32 x) {
    // frsqrte instruction
    f32 inv = 1.0 / frsqrte(x);

    // Newton-Raphson refinement
    return -(inv * inv * x - 3.0) * inv * 0.5;
}

} // namespace EGG::Mathf
