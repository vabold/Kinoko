#include "KartPhysics.hh"

namespace Kart {

KartPhysics::KartPhysics(bool isBike) {
    m_pose = EGG::Matrix34f::ident;
    m_dynamics = isBike ? new KartDynamicsBike : new KartDynamics;
}

void KartPhysics::reset() {
    m_dynamics->init();
    m_decayingStuntRot = EGG::Quatf::ident;
    m_instantaneousStuntRot = EGG::Quatf::ident;
    m_specialRot = EGG::Quatf::ident;
    m_decayingExtraRot = EGG::Quatf::ident;
    m_instantaneousExtraRot = EGG::Quatf::ident;
    m_extraRot = EGG::Quatf::ident;
    m_pose = EGG::Matrix34f::ident;
    m_xAxis = EGG::Vector3f(m_pose(0, 0), m_pose(1, 0), m_pose(2, 0));
    m_yAxis = EGG::Vector3f(m_pose(0, 1), m_pose(1, 1), m_pose(2, 1));
    m_zAxis = EGG::Vector3f(m_pose(0, 2), m_pose(1, 2), m_pose(2, 2));
    m_pos = m_dynamics->pos();
    m_velocity = m_dynamics->velocity();
}

void KartPhysics::updatePose() {
    m_pose.makeQT(m_dynamics->fullRot(), m_dynamics->pos());
    m_xAxis = EGG::Vector3f(m_pose(0, 0), m_pose(1, 0), m_pose(2, 0));
    m_yAxis = EGG::Vector3f(m_pose(0, 1), m_pose(1, 1), m_pose(2, 1));
    m_zAxis = EGG::Vector3f(m_pose(0, 2), m_pose(1, 2), m_pose(2, 2));
}

void KartPhysics::calc(f32 dt, f32 maxSpeed, const EGG::Vector3f & /*scale*/, bool air) {
    m_specialRot = m_instantaneousStuntRot * m_decayingStuntRot;
    m_extraRot = m_instantaneousExtraRot * m_decayingExtraRot;

    m_dynamics->setSpecialRot(m_specialRot);
    m_dynamics->setExtraRot(m_extraRot);

    m_dynamics->calc(dt, maxSpeed, air);

    m_decayingStuntRot = m_decayingStuntRot.slerpTo(EGG::Quatf::ident, 0.1f);
    m_decayingExtraRot = m_decayingExtraRot.slerpTo(EGG::Quatf::ident, 0.1f);

    m_instantaneousStuntRot = EGG::Quatf::ident;
    m_instantaneousExtraRot = EGG::Quatf::ident;
}

KartDynamics *KartPhysics::getDynamics() {
    return m_dynamics;
}

const KartDynamics *KartPhysics::getDynamics() const {
    return m_dynamics;
}

const EGG::Matrix34f &KartPhysics::getPose() const {
    return m_pose;
}

void KartPhysics::setPos(const EGG::Vector3f &pos) {
    m_pos = pos;
}

void KartPhysics::setVelocity(const EGG::Vector3f &vel) {
    m_velocity = vel;
}

KartPhysics *KartPhysics::Create(const KartParam &param) {
    // TODO: Hitboxes and BSP params
    return new KartPhysics(param.isBike());
}

} // namespace Kart
