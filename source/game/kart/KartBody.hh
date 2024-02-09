#pragma once

#include "game/kart/KartObjectProxy.hh"
#include "game/kart/KartPhysics.hh"

namespace Kart {

class KartBody : public KartObjectProxy {
public:
    KartBody(KartPhysics *physics);
    virtual ~KartBody() {}

    virtual EGG::Matrix34f wheelMatrix(u16);

    void reset();

    KartPhysics *physics() const;

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

    EGG::Matrix34f wheelMatrix(u16 wheelIdx) override;
};

} // namespace Kart
