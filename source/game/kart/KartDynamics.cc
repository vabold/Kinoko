#include "KartDynamics.hh"

namespace Kart {

KartDynamics::KartDynamics() {
    init();
}

void KartDynamics::init() {
    m_speedNorm = 0.0f;
    m_gravity = -1.0f;
    m_forceUpright = true;
    m_noGravity = false;
}

void KartDynamics::resetInternalVelocity() {
    m_intVel.setZero();
}

void KartDynamics::calc(f32 dt, f32 maxSpeed, bool /*air*/) {
    if (!m_noGravity) {
        m_totalForce.y += m_gravity;
    }

    m_acceleration0 = m_totalForce;
    m_extVel += m_acceleration0 * dt;
    m_extVel *= 0.998f;
    m_velocity = m_extVel * dt + m_intVel + m_movingObjVel + m_movingRoadVel;
    m_speedNorm = std::min(m_velocity.normalise(), maxSpeed);
    m_velocity *= m_speedNorm;
    m_pos += m_velocity;

    m_fullRot = m_extraRot * m_mainRot * m_specialRot;

    m_mainRot.normalise();
    m_fullRot.normalise();

    m_totalForce.setZero();
}

const EGG::Vector3f &KartDynamics::pos() const {
    return m_pos;
}

const EGG::Vector3f &KartDynamics::velocity() const {
    return m_velocity;
}

const EGG::Quatf &KartDynamics::fullRot() const {
    return m_fullRot;
}

const EGG::Vector3f &KartDynamics::extVel() const {
    return m_extVel;
}

void KartDynamics::setPos(const EGG::Vector3f &pos) {
    m_pos = pos;
}

void KartDynamics::setGravity(f32 gravity) {
    m_gravity = gravity;
}

void KartDynamics::setMainRot(const EGG::Quatf &q) {
    m_mainRot = q;
}

void KartDynamics::setFullRot(const EGG::Quatf &q) {
    m_fullRot = q;
}

void KartDynamics::setSpecialRot(const EGG::Quatf &q) {
    m_specialRot = q;
}

void KartDynamics::setExtraRot(const EGG::Quatf &q) {
    m_extraRot = q;
}

void KartDynamics::setExtVel(const EGG::Vector3f &v) {
    m_extVel = v;
}

} // namespace Kart
