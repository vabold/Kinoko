#pragma once

#include <Common.hh>

// Credit: kiwi515/ogws

namespace Abstract {
namespace g3d {

enum class AnmPolicy : u32 {
    OneTime = 0,
    Loop = 1,
    Max = 2,
};

[[nodiscard]] f32 PlayPolicy_Onetime(f32 start, f32 end, f32 frame);
[[nodiscard]] f32 PlayPolicy_Loop(f32 start, f32 end, f32 frame);

typedef f32 (*PlayPolicyFunc)(f32 start, f32 end, f32 frame);

[[nodiscard]] inline PlayPolicyFunc GetAnmPlayPolicy(AnmPolicy policy) {
    constexpr size_t count = static_cast<size_t>(AnmPolicy::Max);
    static constexpr PlayPolicyFunc POLICY_TABLE[count] = {
            PlayPolicy_Onetime,
            PlayPolicy_Loop,
    };

    size_t idx = static_cast<size_t>(policy);
    ASSERT(idx < count);
    return POLICY_TABLE[idx];
}

class FrameCtrl {
public:
    FrameCtrl(f32 start, f32 end, PlayPolicyFunc policy)
        : m_frame(0.0f), m_updateRate(1.0f), m_startFrame(start), m_endFrame(end),
          m_playPolicy(policy) {
        ASSERT(m_playPolicy);
    }

    void updateFrame() {
        setFrame(m_updateRate * s_baseUpdateRate + m_frame);
    }

    /// @beginGetters
    [[nodiscard]] f32 frame() const {
        return m_frame;
    }

    [[nodiscard]] f32 rate() const {
        return m_updateRate;
    }

    [[nodiscard]] PlayPolicyFunc playPolicy() const {
        return m_playPolicy;
    }
    /// @endGetters

    /// @beginSetters
    void setFrame(f32 frame) {
        m_frame = m_playPolicy(m_startFrame, m_endFrame, frame);
    }

    void setRate(f32 rate) {
        m_updateRate = rate;
    }

    void setPlayPolicy(PlayPolicyFunc func) {
        ASSERT(func);
        m_playPolicy = func;
    }
    /// @endSetters

private:
    f32 m_frame;
    f32 m_updateRate;
    f32 m_startFrame;
    f32 m_endFrame;
    PlayPolicyFunc m_playPolicy;

    static f32 s_baseUpdateRate;
};

} // namespace g3d
} // namespace Abstract
