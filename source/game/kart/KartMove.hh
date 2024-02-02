#pragma once

#include "game/kart/KartObjectProxy.hh"

namespace Kart {

class KartMove : public KartObjectProxy {
public:
    KartMove();

    void setInitialPhysicsValues(const EGG::Vector3f &pos, const EGG::Vector3f &angles);
    void setKartSpeedLimit();

    void calc();

    const EGG::Vector3f &scale() const;
    f32 hardSpeedLimit() const;

private:
    EGG::Vector3f m_scale;
    f32 m_hardSpeedLimit;
};

class KartMoveBike : public KartMove {};

} // namespace Kart
