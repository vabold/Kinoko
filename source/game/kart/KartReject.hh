#pragma once

#include "game/kart/KartObjectProxy.hh"

namespace Field {
struct CollisionInfo;
}

namespace Kart {

/// @brief Pertains to handling reject road.
class KartReject : public KartObjectProxy {
public:
    KartReject();
    ~KartReject();

    void reset();

    void calcRejectRoad();
    bool calcRejection();

private:
    bool calcCollision(Field::CollisionInfo &colInfo, Field::KCLTypeMask mask,
            EGG::Vector3f &tangentOff);

    f32 m_rejectSign;
};

} // namespace Kart
