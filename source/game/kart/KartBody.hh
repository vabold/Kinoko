#pragma once

#include "game/kart/KartObjectProxy.hh"
#include "game/kart/KartPhysics.hh"

namespace Kart {

/// @nosubgrouping
class KartBody : protected KartObjectProxy {
public:
    KartBody(KartPhysics *physics);
    virtual ~KartBody() {}

    [[nodiscard]] virtual EGG::Matrix34f wheelMatrix(u16);

    void reset();

    /// @beginSetters
    void setAngle(f32 val);
    /// @endSetters

    /// @beginGetters
    [[nodiscard]] KartPhysics *physics() const;
    /// @endGetters

protected:
    KartPhysics *m_physics;
    f32 m_anAngle; ///< @rename
};

class KartBodyKart : public KartBody {
public:
    KartBodyKart(KartPhysics *physics);
    ~KartBodyKart() override;
};

class KartBodyBike : public KartBody {
public:
    KartBodyBike(KartPhysics *physics);
    ~KartBodyBike() override;

    [[nodiscard]] EGG::Matrix34f wheelMatrix(u16 wheelIdx) override;
};

class KartBodyQuacker : public KartBodyBike {
public:
    KartBodyQuacker(KartPhysics *physics);
    ~KartBodyQuacker() override;

    [[nodiscard]] EGG::Matrix34f wheelMatrix(u16 wheelIdx) override;
};

} // namespace Kart
