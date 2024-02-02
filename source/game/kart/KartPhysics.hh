#pragma once

#include "game/kart/KartDynamics.hh"
#include "game/kart/KartParam.hh"

#include <egg/math/Matrix.hh>

namespace Kart {

class KartPhysics {
public:
    KartPhysics(bool isBike);

    void reset();
    void updatePose();

    void calc(f32 dt, f32 maxSpeed, const EGG::Vector3f &scale, bool air);

    KartDynamics *getDynamics();
    const KartDynamics *getDynamics() const;
    const EGG::Matrix34f &getPose() const;

    void setPos(const EGG::Vector3f &pos);
    void setVelocity(const EGG::Vector3f &vel);

    static KartPhysics *Create(const KartParam &param);

private:
    KartDynamics *m_dynamics;
    EGG::Vector3f m_pos;
    EGG::Quatf m_decayingStuntRot;
    EGG::Quatf m_instantaneousStuntRot;
    EGG::Quatf m_specialRot;
    EGG::Quatf m_decayingExtraRot;
    EGG::Quatf m_instantaneousExtraRot;
    EGG::Quatf m_extraRot;
    EGG::Matrix34f m_pose;
    EGG::Vector3f m_xAxis;
    EGG::Vector3f m_yAxis;
    EGG::Vector3f m_zAxis;
    EGG::Vector3f m_velocity;
};

} // namespace Kart
