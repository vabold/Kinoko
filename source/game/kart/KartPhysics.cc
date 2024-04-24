#include "KartPhysics.hh"

namespace Kart {

KartPhysics::KartPhysics(bool isBike) {
    m_pose = EGG::Matrix34f::ident;
    m_dynamics = isBike ? new KartDynamicsBike : new KartDynamics;
    m_hitboxGroup = new CollisionGroup;
    m_fc = 50.0f; // set immediately after in KartPhysics::Create()
}

KartPhysics::~KartPhysics() {
    delete m_dynamics;
    delete m_hitboxGroup;
}

void KartPhysics::reset() {
    m_dynamics->init();
    m_hitboxGroup->reset();
    m_decayingStuntRot = EGG::Quatf::ident;
    m_instantaneousStuntRot = EGG::Quatf::ident;
    m_specialRot = EGG::Quatf::ident;
    m_decayingExtraRot = EGG::Quatf::ident;
    m_instantaneousExtraRot = EGG::Quatf::ident;
    m_extraRot = EGG::Quatf::ident;
    m_pose = EGG::Matrix34f::ident;
    m_xAxis = EGG::Vector3f(m_pose[0, 0], m_pose[1, 0], m_pose[2, 0]);
    m_yAxis = EGG::Vector3f(m_pose[0, 1], m_pose[1, 1], m_pose[2, 1]);
    m_zAxis = EGG::Vector3f(m_pose[0, 2], m_pose[1, 2], m_pose[2, 2]);
    m_pos = m_dynamics->pos();
    m_velocity = m_dynamics->velocity();
}

void KartPhysics::updatePose() {
    m_pose.makeQT(m_dynamics->fullRot(), m_dynamics->pos());
    m_xAxis = EGG::Vector3f(m_pose[0, 0], m_pose[1, 0], m_pose[2, 0]);
    m_yAxis = EGG::Vector3f(m_pose[0, 1], m_pose[1, 1], m_pose[2, 1]);
    m_zAxis = EGG::Vector3f(m_pose[0, 2], m_pose[1, 2], m_pose[2, 2]);
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

KartDynamics *KartPhysics::dynamics() {
    return m_dynamics;
}

const KartDynamics *KartPhysics::dynamics() const {
    return m_dynamics;
}

const EGG::Matrix34f &KartPhysics::pose() const {
    return m_pose;
}

CollisionGroup *KartPhysics::hitboxGroup() {
    return m_hitboxGroup;
}

const EGG::Vector3f &KartPhysics::xAxis() const {
    return m_xAxis;
}

const EGG::Vector3f &KartPhysics::yAxis() const {
    return m_yAxis;
}

const EGG::Vector3f &KartPhysics::zAxis() const {
    return m_zAxis;
}

const EGG::Vector3f &KartPhysics::pos() const {
    return m_pos;
}

f32 KartPhysics::fc() const {
    return m_fc;
}

void KartPhysics::setPos(const EGG::Vector3f &pos) {
    m_pos = pos;
}

void KartPhysics::setVelocity(const EGG::Vector3f &vel) {
    m_velocity = vel;
}

void KartPhysics::set_fc(f32 val) {
    m_fc = val;
}

void KartPhysics::composeStuntRot(const EGG::Quatf &rot) {
    m_instantaneousStuntRot *= rot;
}

void KartPhysics::composeDecayingRot(const EGG::Quatf &rot) {
    m_decayingStuntRot *= rot;
}

KartPhysics *KartPhysics::Create(const KartParam &param) {
    KartPhysics *physics = new KartPhysics(param.isBike());

    const BSP &bsp = param.bsp();

    physics->set_fc(physics->hitboxGroup()->initHitboxes(bsp.hitboxes));

    physics->dynamics()->setBspParams(bsp.angVel0Factor, bsp.cuboids[0], bsp.cuboids[1], false);

    return physics;
}

} // namespace Kart
