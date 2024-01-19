#pragma once

#include "game/kart/KartObjectProxy.hh"
#include "game/kart/KartPhysics.hh"

namespace Kart {

class KartBody : public KartObjectProxy {
public:
    KartBody(KartPhysics *physics);
    virtual ~KartBody() {}

    void reset();

    KartPhysics *getPhysics() const;

protected:
    KartPhysics *m_physics;
};

class KartBodyKart : public KartBody {
public:
    KartBodyKart(KartPhysics *physics);
};

class KartBodyBike : public KartBody {
public:
    KartBodyBike(KartPhysics *physics);
};

} // namespace Kart
