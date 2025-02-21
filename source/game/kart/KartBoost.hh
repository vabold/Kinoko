#pragma once

#include <Common.hh>

namespace Kart {

/// @brief State management for boosts (start boost, mushrooms, mini-turbos)
/// @nosubgrouping
class KartBoost {
public:
    enum class Type {
        AllMt,
        MushroomAndBoostPanel,
        TrickAndZipper,
        Max,
    };

    KartBoost();
    ~KartBoost();

    [[nodiscard]] bool activate(Type type, s16 frames);
    [[nodiscard]] bool calc();
    void reset();

    /// @addr{0x80588E18}
    void resetActive() {
        m_active.fill(false);
    }

    /// @beginGetters
    [[nodiscard]] f32 multiplier() const {
        return m_multiplier;
    }

    [[nodiscard]] f32 acceleration() const {
        return m_acceleration;
    }

    [[nodiscard]] f32 speedLimit() const {
        return m_speedLimit;
    }
    /// @endGetters

private:
    // We need to evaluate this expression early for the array initialization
    static constexpr size_t BOOST_TYPE_COUNT = static_cast<size_t>(Type::Max);

    std::array<s16, BOOST_TYPE_COUNT> m_timers;  ///< Durations for the different boost types.
    std::array<bool, BOOST_TYPE_COUNT> m_active; ///< Whether the different boost types are active.
    f32 m_multiplier;                            ///< Multiplier applied to vehicle speed.
    f32 m_acceleration;
    f32 m_speedLimit;
};

} // namespace Kart
