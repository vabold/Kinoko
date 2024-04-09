#pragma once

#include <egg/math/Matrix.hh>

namespace Kart {

class KartDynamics {
public:
    KartDynamics();

    virtual void forceUpright() {}
    virtual void stabilize() {}

    void init();
    void resetInternalVelocity();
    void setBspParams(f32 rotSpeed, const EGG::Vector3f &m, const EGG::Vector3f &n,
            bool skipInertia);

    void calc(f32 dt, f32 maxSpeed, bool air);

    void applySuspensionWrench(const EGG::Vector3f &p, const EGG::Vector3f &Flinear,
            const EGG::Vector3f &Frot, bool ignoreX);

    const EGG::Matrix34f &invInertiaTensor() const;
    const EGG::Vector3f &pos() const;
    const EGG::Vector3f &velocity() const;
    f32 gravity() const;
    const EGG::Vector3f &intVel() const;
    const EGG::Quatf &mainRot() const;
    const EGG::Quatf &fullRot() const;
    const EGG::Vector3f &totalForce() const;
    const EGG::Vector3f &extVel() const;
    const EGG::Vector3f &angVel0() const;
    const EGG::Vector3f &angVel2() const;
    f32 speedFix() const;

    void setPos(const EGG::Vector3f &pos);
    void setGravity(f32 gravity);
    void setMainRot(const EGG::Quatf &q);
    void setFullRot(const EGG::Quatf &q);
    void setSpecialRot(const EGG::Quatf &q);
    void setExtraRot(const EGG::Quatf &q);
    void setIntVel(const EGG::Vector3f &v);
    void setStabilizationFactor(f32 val);
    void setTotalForce(const EGG::Vector3f &v);
    void setExtVel(const EGG::Vector3f &v);
    void setAngVel0(const EGG::Vector3f &v);
    void setAngVel2(const EGG::Vector3f &v);
    void setAngVel0YFactor(f32 val);
    void setTop_(const EGG::Vector3f &v);

protected:
    EGG::Matrix34f m_inertiaTensor;
    EGG::Matrix34f m_invInertiaTensor;
    f32 m_angVel0Factor;
    EGG::Vector3f m_pos;
    EGG::Vector3f m_extVel;
    EGG::Vector3f m_acceleration0;
    EGG::Vector3f m_angVel0;
    EGG::Vector3f m_movingObjVel;
    EGG::Vector3f m_angVel1;
    EGG::Vector3f m_movingRoadVel;
    EGG::Vector3f m_velocity;
    f32 m_speedNorm;
    EGG::Vector3f m_angVel2;
    EGG::Quatf m_mainRot;
    EGG::Quatf m_fullRot;
    EGG::Vector3f m_totalForce;
    EGG::Vector3f m_totalTorque;
    EGG::Quatf m_specialRot;
    EGG::Quatf m_extraRot;
    f32 m_gravity;
    EGG::Vector3f m_intVel;
    f32 m_stabilizationFactor;
    f32 m_speedFix;
    EGG::Vector3f m_top_;
    f32 m_angVel0YFactor;
    bool m_forceUpright;
    bool m_noGravity;
};

class KartDynamicsBike : public KartDynamics {
public:
    KartDynamicsBike();
    ~KartDynamicsBike();

private:
    void forceUpright() override;
    void stabilize() override;
};

} // namespace Kart
