#pragma once

#include "game/kart/CollisionGroup.hh"
#include "game/kart/KartObjectProxy.hh"
#include "game/kart/KartParam.hh"

#include <egg/math/Matrix.hh>

namespace Kart {

class WheelPhysics : KartObjectProxy {
public:
    WheelPhysics(u16 wheelIdx, u16 bspWheelIdx);
    ~WheelPhysics();

    void init();
    void initBsp();
    void reset();

    void realign(const EGG::Vector3f &bottom, const EGG::Vector3f &vehicleMovement);

    void updateCollision(const EGG::Vector3f &bottom, const EGG::Vector3f &topmostPos);

    const EGG::Vector3f &pos() const;
    const EGG::Vector3f &lastPosDiff() const;
    f32 suspTravel();
    const EGG::Vector3f &topmostPos() const;
    CollisionGroup *hitboxGroup();
    const CollisionGroup *hitboxGroup() const;
    const EGG::Vector3f &speed() const;
    f32 effectiveRadius() const;
    f32 _74() const;

    void setSuspTravel(f32 suspTravel);
    void setPos(const EGG::Vector3f &pos);
    void setLastPos(const EGG::Vector3f &pos);
    void setLastPosDiff(const EGG::Vector3f &pos);
    void setWheelEdgePos(const EGG::Vector3f &pos);
    void setColVel(const EGG::Vector3f &vec);

private:
    u16 m_wheelIdx;
    u16 m_bspWheelIdx;
    const BSP::Wheel *m_bspWheel;
    CollisionGroup *m_hitboxGroup;
    EGG::Vector3f m_pos;
    EGG::Vector3f m_lastPos;
    EGG::Vector3f m_lastPosDiff;
    f32 m_suspTravel;
    EGG::Vector3f m_colVel;
    EGG::Vector3f m_speed;
    EGG::Vector3f m_wheelEdgePos;
    f32 m_effectiveRadius;
    f32 m_targetEffectiveRadius;
    f32 m_74;
    EGG::Vector3f m_topmostPos;
};

class KartSuspensionPhysics : KartObjectProxy {
public:
    KartSuspensionPhysics(u16 wheelIdx, u16 bspWheelIdx);
    ~KartSuspensionPhysics();

    void init();
    void reset();

    void setInitialState();

    void calcCollision(f32 dt, const EGG::Vector3f &gravity, const EGG::Matrix34f &mat);
    void calcSuspension(const EGG::Vector3f &forward, const EGG::Vector3f &vehicleMovement);

private:
    const BSP::Wheel *m_bspWheel;
    WheelPhysics *m_tirePhysics;
    u16 m_bspWheelIdx;
    u16 m_wheelIdx;
    EGG::Vector3f m_topmostPos;
    f32 m_maxTravelScaled;
    EGG::Vector3f m_bottomDir;
};

} // namespace Kart
