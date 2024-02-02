#pragma once

#include <egg/math/Matrix.hh>

namespace Kart {

class KartDynamics {
public:
    KartDynamics();
    void init();
    void resetInternalVelocity();

    void calc(f32 dt, f32 maxSpeed, bool air);

    const EGG::Vector3f &pos() const;
    const EGG::Vector3f &velocity() const;
    const EGG::Quatf &fullRot() const;

    void setPos(const EGG::Vector3f &pos);
    void setGravity(f32 gravity);
    void setMainRot(const EGG::Quatf &q);
    void setFullRot(const EGG::Quatf &q);
    void setSpecialRot(const EGG::Quatf &q);
    void setExtraRot(const EGG::Quatf &q);

private:
    // These are the only vars needed in setInitialPhysicsValues
    EGG::Vector3f m_pos;
    EGG::Vector3f m_extVel;
    EGG::Vector3f m_acceleration0;
    EGG::Vector3f m_movingObjVel;
    EGG::Vector3f m_movingRoadVel;
    EGG::Vector3f m_velocity;
    f32 m_speedNorm;
    EGG::Quatf m_mainRot;
    EGG::Quatf m_fullRot;
    EGG::Vector3f m_totalForce;
    EGG::Quatf m_specialRot;
    EGG::Quatf m_extraRot;
    f32 m_gravity;
    EGG::Vector3f m_intVel;
    bool m_forceUpright;
    bool m_noGravity;
};

class KartDynamicsBike : public KartDynamics {};

} // namespace Kart
