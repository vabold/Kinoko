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
    void calcSinkDepth();
    void trySetTargetSinkDepth(f32 val);
    void calcTargetSinkDepth();

    /// @beginSetters
    /// @addr{0x8056E424}
    void setAngle(f32 val) {
        m_anAngle = val;
    }
    /// @endSetters

    /// @beginGetters
    [[nodiscard]] KartPhysics *physics() const {
        return m_physics;
    }

    [[nodiscard]] f32 sinkDepth() const {
        return m_sinkDepth;
    }
    /// @endGetters

protected:
    KartPhysics *m_physics;
    f32 m_anAngle;   ///< @rename
    f32 m_sinkDepth; ///< Vehicle offset applied downward into collision
    f32 m_targetSinkDepth;
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
