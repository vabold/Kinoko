#pragma once

#include "game/kart/CollisionGroup.hh"
#include "game/kart/KartDynamics.hh"
#include "game/kart/KartParam.hh"

#include <egg/math/Matrix.hh>

namespace Kart {

class KartPhysics {
public:
    KartPhysics(bool isBike);
    ~KartPhysics();

    void reset();
    void updatePose();

    void calc(f32 dt, f32 maxSpeed, const EGG::Vector3f &scale, bool air);

    KartDynamics *dynamics();
    const KartDynamics *dynamics() const;
    const EGG::Matrix34f &pose() const;
    CollisionGroup *hitboxGroup();
    const EGG::Vector3f &xAxis() const;
    const EGG::Vector3f &yAxis() const;
    const EGG::Vector3f &zAxis() const;
    const EGG::Vector3f &pos() const;
    f32 fc() const;

    void setPos(const EGG::Vector3f &pos);
    void setVelocity(const EGG::Vector3f &vel);
    void set_fc(f32 val);
    void composeStuntRot(const EGG::Quatf &rot);
    void composeDecayingRot(const EGG::Quatf &rot);

    static KartPhysics *Create(const KartParam &param);

private:
    KartDynamics *m_dynamics;
    CollisionGroup *m_hitboxGroup;
    EGG::Vector3f m_pos;
    EGG::Quatf m_decayingStuntRot;
    EGG::Quatf m_instantaneousStuntRot;
    EGG::Quatf m_specialRot;
    /// @brief Rotation that occurs when landing from a trick.
    EGG::Quatf m_decayingExtraRot;
    EGG::Quatf m_instantaneousExtraRot;
    EGG::Quatf m_extraRot;
    EGG::Matrix34f m_pose;
    EGG::Vector3f m_xAxis;
    EGG::Vector3f m_yAxis;
    EGG::Vector3f m_zAxis;
    EGG::Vector3f m_velocity;
    f32 m_fc; // collisionLimit?
};

} // namespace Kart
