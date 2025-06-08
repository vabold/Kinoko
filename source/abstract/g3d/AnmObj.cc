#include "AnmObj.hh"

#include <cmath>

namespace Abstract {
namespace g3d {

/// @addr{0x800604F0}
f32 PlayPolicy_Onetime(f32 /*start*/, f32 /*end*/, f32 frame) {
    return frame;
}

/// @addr{0x80060500}
f32 PlayPolicy_Loop(f32 start, f32 end, f32 frame) {
    ASSERT(end > start);

    f32 length = end - start;

    if (frame >= 0.0f) {
        return ::fmodf(frame, length);
    }

    f32 offset = ::fmodf(frame + length, length);

    return offset >= 0.0f ? offset : offset + length;
}

f32 FrameCtrl::s_baseUpdateRate = 1.0f;

} // namespace g3d
} // namespace Abstract
