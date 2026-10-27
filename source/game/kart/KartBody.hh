#pragma once

#include "game/kart/KartObjectProxy.hh"
#include "game/kart/KartPhysics.hh"

namespace Kinoko::Kart {

/// @nosubgrouping
class KartBody : protected KartObjectProxy {
public:
    KartBody(KartPhysics *physics);
    virtual ~KartBody() {}

    [[nodiscard]] virtual EGG::Matrix34f wheelMatrix(u16, const EGG::Matrix34f &bodyMat);
    virtual void setParams(const KartParam::KartDisp &kartDisp,
            const KartParam::BikeDisp &bikeDisp) = 0;

    void init(const KartParam::KartDisp &kartDisp, const KartParam::BikeDisp &bikeDisp);

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

    /// @addr{0x8056CDC4}
    void setParams(const KartParam::KartDisp &, const KartParam::BikeDisp &) override {}
};

class KartBodyBike : public KartBody {
public:
    KartBodyBike(KartPhysics *physics);
    ~KartBodyBike() override;

    [[nodiscard]] EGG::Matrix34f wheelMatrix(u16 wheelIdx, const EGG::Matrix34f &bodyMat) override;
    void setParams(const KartParam::KartDisp &kartDisp,
            const KartParam::BikeDisp &bikeDisp) override;

private:
    EGG::Vector3f m_handlePos; ///< The relative position of the handlebar

    /// @brief The rotation of the handlebar
    /// @details Introduced in Kinoko as a slight performance improvement to prevent unnecessarily
    /// recalculating the handlebar rotation matrix every frame in wheelMatrix().
    EGG::Matrix34f m_handleRotMat;
};

class KartBodyQuacker : public KartBodyBike {
public:
    KartBodyQuacker(KartPhysics *physics);
    ~KartBodyQuacker() override;

    [[nodiscard]] EGG::Matrix34f wheelMatrix(u16 wheelIdx, const EGG::Matrix34f &bodyMat) override;
};

} // namespace Kinoko::Kart
