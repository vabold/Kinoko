#pragma once

#include "game/kart/KartObjectProxy.hh"

namespace Kart {

class KartMove : public KartObjectProxy {
public:
    KartMove();

    virtual void setTurnParams();
    virtual void init(bool b1, bool b2);
    virtual f32 leanRot() const;

    void setInitialPhysicsValues(const EGG::Vector3f &position, const EGG::Vector3f &angles);
    void setKartSpeedLimit();

    void calc();
    void calcTop();
    void calcDirs();
    void calcRotation();
    virtual void calcVehicleRotation(f32 /*turn*/) {}

    void setFloorCollisionCount(u16 count);

    const EGG::Vector3f &scale() const;
    f32 hardSpeedLimit() const;
    const EGG::Vector3f &smoothedUp() const;
    f32 totalScale() const;
    const EGG::Vector3f &dir() const;
    u16 floorCollisionCount() const;

private:
    f32 m_hardSpeedLimit;
    EGG::Vector3f m_smoothedUp;
    EGG::Vector3f m_up;
    EGG::Vector3f m_dir;
    u16 m_floorCollisionCount;
    f32 m_realTurn;
    EGG::Vector3f m_scale;
    f32 m_totalScale;
};

class KartMoveBike : public KartMove {
public:
    KartMoveBike();
    ~KartMoveBike();

    void setTurnParams() override;
    void init(bool b1, bool b2) override;

    f32 leanRot() const override;

private:
    f32 m_leanRot;
};

} // namespace Kart
