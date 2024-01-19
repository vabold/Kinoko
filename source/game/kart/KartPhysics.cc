#include "KartPhysics.hh"

namespace Kart {

KartPhysics::KartPhysics(bool isBike) {
    m_pose = EGG::Matrix34f::ident;
    m_dynamics = isBike ? new KartDynamicsBike : new KartDynamics;
}

void KartPhysics::reset() {
    m_dynamics->init();
    m_pose = EGG::Matrix34f::ident;
    m_xAxis = EGG::Vector3f(m_pose(0, 0), m_pose(1, 0), m_pose(2, 0));
    m_yAxis = EGG::Vector3f(m_pose(0, 1), m_pose(1, 1), m_pose(2, 1));
    m_zAxis = EGG::Vector3f(m_pose(0, 2), m_pose(1, 2), m_pose(2, 2));
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

void KartPhysics::updatePose() {
    m_pose.makeQT(m_dynamics->getFullRot(), m_dynamics->getPos());
    m_xAxis = EGG::Vector3f(m_pose(0, 0), m_pose(1, 0), m_pose(2, 0));
    m_yAxis = EGG::Vector3f(m_pose(0, 1), m_pose(1, 1), m_pose(2, 1));
    m_zAxis = EGG::Vector3f(m_pose(0, 2), m_pose(1, 2), m_pose(2, 2));
}

KartPhysics *KartPhysics::Create(const KartParam &param) {
    // TODO: Hitboxes and BSP params
    return new KartPhysics(param.isBike());
}

} // namespace Kart
