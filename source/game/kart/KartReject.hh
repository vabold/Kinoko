#pragma once

#include "game/kart/KartObjectProxy.hh"

namespace Kart {

/// @brief Pertains to handling reject road. Labeled as KartMoveSub130 in Ghidra.
class KartReject : public KartObjectProxy {
public:
    KartReject();
    ~KartReject();

    void reset();

    void calcRejectRoad();
    bool calcHalfpipeRejection();

private:
    f32 m_rejectSign;
};

} // namespace Kart
