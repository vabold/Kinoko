#pragma once

#include "game/kart/KartObjectProxy.hh"

namespace Kart {

/// @brief Calculates the duration of burnout and rotation induced
/// when holding acceleration too long during the race countdown.
/// @nosubgrouping
class KartBurnout : KartObjectProxy {
public:
    KartBurnout();
    ~KartBurnout();

    void start();
    void calc();

private:
    void calcRotation();
    bool calcEnd(u32 duration);

    void activate();
    void deactivate();
    [[nodiscard]] bool isActive() const;

    u32 m_timer;
    u16 m_phase;
    f32 m_amplitude;
};

} // namespace Kart
