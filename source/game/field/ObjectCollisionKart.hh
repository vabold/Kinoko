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

    size_t checkCollision(const EGG::Matrix34f &mat, const EGG::Vector3f &v);

    static EGG::Vector3f GetHitDirection(u16 objKartHit);
    static constexpr std::span<const EGG::Vector3f> GetVehicleVertices(Vehicle vehicle);

private:
    ObjectCollisionConvexHull *m_hull;
    Kart::KartObject *m_kartObject;
    u32 m_playerIdx;
};

} // namespace Field
