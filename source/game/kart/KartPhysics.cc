#include "KartPhysics.hh"

#include <egg/math/Quat.hh>

namespace Kart {

/// @addr{0x8059F5BC}
KartPhysics::KartPhysics(bool isBike) {
    m_pose = EGG::Matrix34f::ident;
    m_dynamics = isBike ? new KartDynamicsBike : new KartDynamics;
    m_hitboxGroup = new CollisionGroup;
    m_fc = 50.0f; // set immediately after in KartPhysics::Create()
}

/// @addr{0x8059F6F8}
KartPhysics::~KartPhysics() {
    delete m_dynamics;
    delete m_hitboxGroup;
}

/// @addr{0x8059F7C8}
void KartPhysics::reset() {
    m_dynamics->init();
    m_hitboxGroup->reset();
    m_decayingStuntRot = EGG::Quatf::ident;
    m_instantaneousStuntRot = EGG::Quatf::ident;
    m_specialRot = EGG::Quatf::ident;
    m_decayingExtraRot = EGG::Quatf::ident;
    m_instantaneousExtraRot = EGG::Quatf::ident;
    m_extraRot = EGG::Quatf::ident;
    m_movingObjVel.setZero();
    m_pose = EGG::Matrix34f::ident;
    m_xAxis = EGG::Vector3f(m_pose[0, 0], m_pose[1, 0], m_pose[2, 0]);
    m_yAxis = EGG::Vector3f(m_pose[0, 1], m_pose[1, 1], m_pose[2, 1]);
    m_zAxis = EGG::Vector3f(m_pose[0, 2], m_pose[1, 2], m_pose[2, 2]);
    m_pos = m_dynamics->pos();
    m_velocity = m_dynamics->velocity();
}

/// @brief Constructs a transformation matrix from rotation and position.
/// @addr{0x805A0340}
void KartPhysics::updatePose() {
    m_pose.makeQT(m_dynamics->fullRot(), m_dynamics->pos());
    m_xAxis = EGG::Vector3f(m_pose[0, 0], m_pose[1, 0], m_pose[2, 0]);
    m_yAxis = EGG::Vector3f(m_pose[0, 1], m_pose[1, 1], m_pose[2, 1]);
    m_zAxis = EGG::Vector3f(m_pose[0, 2], m_pose[1, 2], m_pose[2, 2]);
}

/// @brief Computes trick rotation and calls to KartDynamics::calc().
/// @addr{0x8059F968}
/// @param dt delta time. It's always 1.0f.
/// @param maxSpeed 120.0f, unless we're in a bullet (145.0f)
/// @param air Whether we're touching ground. Currently unused.
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

/// @addr{0x805A04A0}
KartPhysics *KartPhysics::Create(const KartParam &param) {
    KartPhysics *physics = new KartPhysics(param.isBike());

    const BSP &bsp = param.bsp();

    physics->set_fc(physics->hitboxGroup()->initHitboxes(bsp.hitboxes));

    physics->dynamics()->setBspParams(bsp.angVel0Factor, bsp.cuboids[0], bsp.cuboids[1], false);

    return physics;
}

} // namespace Kart
