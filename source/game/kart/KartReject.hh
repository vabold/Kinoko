#pragma once

#include "game/kart/KartObjectProxy.hh"

namespace Kart {

/// @brief Pertains to handling reject road.
class KartReject : public KartObjectProxy {
public:
    KartReject();
    ~KartReject();

    void reset();

    void calcRejectRoad();
    [[nodiscard]] bool calcRejection();

private:
    f32 m_rejectSign;
};

} // namespace Kart
