#pragma once

#include "game/kart/KartObjectProxy.hh"

namespace Kart {

/// @brief Handles the physics and boosts associated with zippers.
/// @nosubgrouping
class KartHalfPipe : public KartObjectProxy {
public:
    KartHalfPipe();
    ~KartHalfPipe();

    void reset();
    void calc();
    void calcLanding(bool);

    /// @addr{0x80574108}
    static consteval f32 TerminalVelocity() {
        return 65.0f;
    }

private:
    bool m_touchingZipper;
    s16 m_timer;
    EGG::Quatf m_rot;
    EGG::Vector3f m_prevPos;
};

} // namespace Kart
