#pragma once

#include "game/field/ObjectCollisionConvexHull.hh"

namespace Kart {

class KartObject;

} // namespace Kart

namespace Field {

/// @brief Relates a KartObject with its convex hull representation.
class ObjectCollisionKart {
public:
    ObjectCollisionKart();
    ~ObjectCollisionKart();

    void init(u32 idx);

    static constexpr std::span<const EGG::Vector3f> GetVehicleVertices(Vehicle vehicle);

private:
    ObjectCollisionConvexHull *m_convexHull;
    Kart::KartObject *m_kartObject;
    u32 m_playerIdx;
};

} // namespace Field
