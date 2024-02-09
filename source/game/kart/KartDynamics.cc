#include "KartDynamics.hh"

#include <egg/math/Math.hh>

namespace Kart {

KartDynamics::KartDynamics() {
    m_angVel0Factor = 1.0f;
    m_inertiaTensor = EGG::Matrix34f::ident;
    m_invInertiaTensor = EGG::Matrix34f::ident;
    init();
}

void KartDynamics::init() {
    m_speedNorm = 0.0f;
    m_gravity = -1.0f;
    m_forceUpright = true;
    m_noGravity = false;
    m_angVel0YFactor = 0.0f;
}

void KartDynamics::resetInternalVelocity() {
    m_intVel.setZero();
}

void KartDynamics::setBspParams(f32 rotSpeed, const EGG::Vector3f &m, const EGG::Vector3f &n,
        bool skipInertia) {
    constexpr f32 twelfth = 1.0f / 12.0f;

    m_angVel0Factor = rotSpeed;

    if (skipInertia) {
        return;
    }

    m_inertiaTensor = EGG::Matrix34f::zero;
    m_inertiaTensor(0, 0) = (m.y * m.y + m.z * m.z) * twelfth + n.y * n.y + n.z * n.z;
    m_inertiaTensor(1, 1) = (m.z * m.z + m.x * m.x) * twelfth + n.z * n.z + n.x * n.x;
    m_inertiaTensor(2, 2) = (m.x * m.x + m.y * m.y) * twelfth + n.x * n.x + n.y * n.y;

    m_invInertiaTensor = m_inertiaTensor.inverseTo();
}

void KartDynamics::calc(f32 dt, f32 maxSpeed, bool /*air*/) {
    if (!m_noGravity) {
        m_totalForce.y += m_gravity;
    }

    m_acceleration0 = m_totalForce;
    m_extVel += m_acceleration0 * dt;
    m_extVel *= 0.998f;
    m_angVel0 *= 0.98f;

    EGG::Vector3f playerBack = m_mainRot.rotateVector(EGG::Vector3f::ez);
    EGG::Vector3f playerBackHoriz = playerBack;
    playerBackHoriz.y = 0.0f;

    if (FLT_EPSILON < playerBackHoriz.dot()) {
        playerBackHoriz.normalise();
        m_extVel = m_extVel.rej(playerBackHoriz);
    }

    m_velocity = m_extVel * dt + m_intVel + m_movingObjVel + m_movingRoadVel;
    m_speedNorm = std::min(m_velocity.normalise(), maxSpeed);
    m_velocity *= m_speedNorm;
    m_pos += m_velocity;

    EGG::Vector3f t1 = m_invInertiaTensor.multVector(m_totalTorque) * dt;
    m_angVel0 += (t1 + m_invInertiaTensor.multVector(t1 + m_totalTorque) * dt) * 0.5f;

    m_angVel0.x = std::min(0.4f, std::max(-0.4f, m_angVel0.x));
    m_angVel0.y = std::min(0.4f, std::max(-0.4f, m_angVel0.y));
    m_angVel0.z = std::min(0.8f, std::max(-0.8f, m_angVel0.z));
    m_angVel0.y *= m_angVel0YFactor;

    EGG::Vector3f angVelSum = m_angVel2 + m_angVel1 + m_angVel0Factor * m_angVel0;

    if (FLT_EPSILON < angVelSum.dot()) {
        m_mainRot += m_mainRot * angVelSum * dt * 0.5;

        if (EGG::Mathf::abs(m_mainRot.dot()) < FLT_EPSILON) {
            m_mainRot = EGG::Quatf::ident;
        } else {
            m_mainRot.normalise();
        }
    }

    // TODO: Can we prove we don't need this given above???
    if (EGG::Mathf::abs(m_mainRot.dot()) < FLT_EPSILON) {
        m_mainRot = EGG::Quatf::ident;
    } else {
        m_mainRot.normalise();
    }

    m_fullRot = m_extraRot * m_mainRot * m_specialRot;

    m_mainRot.normalise();
    m_fullRot.normalise();

    m_totalForce.setZero();
    m_totalTorque.setZero();
    m_angVel2.setZero();
}

void KartDynamics::applySuspensionWrench(const EGG::Vector3f &p, const EGG::Vector3f &Flinear,
        const EGG::Vector3f &Frot, bool ignoreX) {
    m_totalForce.y += Flinear.y;
    EGG::Vector3f fBody = m_fullRot.rotateVectorInv(Frot);
    EGG::Vector3f rBody = m_fullRot.rotateVectorInv(p - m_pos);
    EGG::Vector3f torque = rBody.cross(fBody);

    if (ignoreX) {
        torque.x = 0.0f;
    }
    torque.y = 0.0f;
    m_totalTorque += torque;
}

const EGG::Matrix34f &KartDynamics::invInertiaTensor() const {
    return m_invInertiaTensor;
}

const EGG::Vector3f &KartDynamics::pos() const {
    return m_pos;
}

const EGG::Vector3f &KartDynamics::velocity() const {
    return m_velocity;
}

const EGG::Vector3f &KartDynamics::intVel() const {
    return m_intVel;
}

const EGG::Quatf &KartDynamics::mainRot() const {
    return m_mainRot;
}

const EGG::Quatf &KartDynamics::fullRot() const {
    return m_fullRot;
}

const EGG::Vector3f &KartDynamics::extVel() const {
    return m_extVel;
}

const EGG::Vector3f &KartDynamics::angVel0() const {
    return m_angVel0;
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

void KartDynamics::setAngVel0(const EGG::Vector3f &v) {
    m_angVel0 = v;
}

void KartDynamics::setAngVel0YFactor(f32 val) {
    m_angVel0YFactor = val;
}

} // namespace Kart
