#pragma once

#include <Common.hh>

namespace Kart {

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

    bool activate(Type type, s16 frames);
    bool calc();

    f32 multiplier() const;
    f32 acceleration() const;
    f32 speedLimit() const;

private:
    // We need to evaluate this expression early for the array initialization
    static constexpr size_t BOOST_TYPE_COUNT = static_cast<size_t>(Type::Max);

    std::array<s16, BOOST_TYPE_COUNT> m_timers;
    std::array<bool, BOOST_TYPE_COUNT> m_active;
    f32 m_multiplier;
    f32 m_acceleration;
    f32 m_speedLimit;
};

} // namespace Kart
