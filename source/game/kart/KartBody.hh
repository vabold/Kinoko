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

    void setAngle(f32 val);

protected:
    KartPhysics *m_physics;
    f32 m_anAngle;
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
