#include "KartDynamics.hh"

#include <egg/math/Math.hh>

namespace Kart {

/// @addr{0x805B4AF8}
KartDynamics::KartDynamics() {
    m_angVel0Factor = 1.0f;
    m_inertiaTensor = EGG::Matrix34f::ident;
    m_invInertiaTensor = EGG::Matrix34f::ident;
    init();
}

/// @addr{0x8059F6B8}
KartDynamics::~KartDynamics() = default;

/// @addr{0x805B5B68}
/// @brief Stabilizes the kart by rotating towards the y-axis unit vector.
void KartDynamics::stabilize() {
    EGG::Vector3f top = m_mainRot.rotateVector(EGG::Vector3f::ey);
    if (EGG::Mathf::abs(top.dot(m_top)) >= 0.9999f) {
        return;
    }

    EGG::Quatf q;
    q.makeVectorRotation(top, m_top);
    m_mainRot = m_mainRot.slerpTo(q.multSwap(m_mainRot), m_stabilizationFactor);
}

/// @addr{0x805B4B54}
void KartDynamics::init() {
    m_pos = EGG::Vector3f::zero;
    m_extVel = EGG::Vector3f::zero;
    m_acceleration = EGG::Vector3f::zero;
    m_angVel0 = EGG::Vector3f::zero;
    m_movingObjVel = EGG::Vector3f::zero;
    m_angVel1 = EGG::Vector3f::zero;
    m_movingRoadVel = EGG::Vector3f::zero;
    m_velocity = EGG::Vector3f::zero;
    m_speedNorm = 0.0f;
    m_angVel2 = EGG::Vector3f::zero;
    m_mainRot = EGG::Quatf::ident;
    m_fullRot = EGG::Quatf::ident;
    m_totalForce = EGG::Vector3f::zero;
    m_totalTorque = EGG::Vector3f::zero;
    m_specialRot = EGG::Quatf::ident;
    m_extraRot = EGG::Quatf::ident;
    m_gravity = -1.0f;
    m_intVel = EGG::Vector3f::zero;
    m_top = EGG::Vector3f::ey;
    m_forceUpright = true;
    m_noGravity = false;
    m_killExtVelY = false;
    m_stabilizationFactor = 0.1f;
    m_top_ = EGG::Vector3f::ey;
    m_speedFix = 0.0f;
    m_angVel0YFactor = 0.0f;
}

/// @addr{0x805B4E84}
void KartDynamics::setInertia(const EGG::Vector3f &m, const EGG::Vector3f &n) {
    constexpr f32 TWELFTH = 1.0f / 12.0f;

    m_inertiaTensor = EGG::Matrix34f::zero;
    m_inertiaTensor[0, 0] = TWELFTH * (m.y * m.y + m.z * m.z) + (n.y * n.y + n.z * n.z);
    m_inertiaTensor[1, 1] = TWELFTH * (m.z * m.z + m.x * m.x) + (n.z * n.z + n.x * n.x);
    m_inertiaTensor[2, 2] = TWELFTH * (m.x * m.x + m.y * m.y) + (n.x * n.x + n.y * n.y);
    m_inertiaTensor.inverseTo33(m_invInertiaTensor);
}

/// @brief On init, takes elements from the kart's BSP and computes the moment of inertia tensor.
/// @addr{0x805B4DC4}
void KartDynamics::setBspParams(f32 rotSpeed, const EGG::Vector3f &m, const EGG::Vector3f &n,
        bool skipInertia) {
    m_angVel0Factor = rotSpeed;

    if (skipInertia) {
        return;
    }

    setInertia(m, n);
}

/// @stage All
/// @brief Every frame, computes acceleration, velocity, position and rotation of the kart.
/// @addr{0x805B5170}
/// @param dt Delta time. It's always 1.0f.
/// @param maxSpeed Always 120.0f.
void KartDynamics::calc(f32 dt, f32 maxSpeed, bool air) {
    constexpr f32 TERMINAL_Y_VEL = 120.0f;

    if (!m_noGravity) {
        m_totalForce.y += m_gravity;
    }

    m_acceleration = m_totalForce;
    m_extVel += m_acceleration * dt;

    if (m_killExtVelY) {
        m_extVel.y = std::min(0.0f, m_extVel.y);
    }

    m_extVel *= 0.998f;
    m_angVel0 *= 0.98f;

    EGG::Vector3f playerBack = m_mainRot.rotateVector(EGG::Vector3f::ez);
    EGG::Vector3f playerBackHoriz = playerBack;
    playerBackHoriz.y = 0.0f;

    if (playerBackHoriz.squaredLength() > std::numeric_limits<f32>::epsilon()) {
        playerBackHoriz.normalise();
        const auto [proj, rej] = m_extVel.projAndRej(playerBackHoriz);
        const EGG::Vector3f &speedBack = proj;
        m_extVel = rej;

        f32 norm = speedBack.squaredLength();
        norm = norm > std::numeric_limits<f32>::epsilon() ? EGG::Mathf::sqrt(norm) : 0.0f;

        m_speedFix = norm * playerBack.dot(playerBackHoriz);
        if (speedBack.dot(playerBackHoriz) < 0.0f) {
            m_speedFix = -m_speedFix;
        }
    }

    if (air) {
        m_intVel.y = std::min(TERMINAL_Y_VEL, m_intVel.y);
    }

    m_velocity = m_extVel * dt + m_intVel + m_movingObjVel + m_movingRoadVel;
    m_speedNorm = std::min(m_velocity.normalise(), maxSpeed);
    m_velocity *= m_speedNorm;
    m_pos += m_velocity;

    EGG::Vector3f t1 = m_invInertiaTensor.multVector33(m_totalTorque) * dt;
    m_angVel0 += (t1 + m_invInertiaTensor.multVector33(t1 + m_totalTorque) * dt) * 0.5f;

    m_angVel0.x = std::min(0.4f, std::max(-0.4f, m_angVel0.x));
    m_angVel0.y = std::min(0.4f, std::max(-0.4f, m_angVel0.y)) * m_angVel0YFactor;
    m_angVel0.z = std::min(0.8f, std::max(-0.8f, m_angVel0.z));

    if (m_forceUpright) {
        forceUpright();
    }

    EGG::Vector3f angVelSum = m_angVel2 + m_angVel1 + m_angVel0Factor * m_angVel0;

    if (angVelSum.squaredLength() > std::numeric_limits<f32>::epsilon()) {
        m_mainRot += m_mainRot.multSwap(angVelSum) * (dt * 0.5f);

        if (EGG::Mathf::abs(m_mainRot.squaredNorm()) > std::numeric_limits<f32>::epsilon()) {
            m_mainRot.normalise();
        } else {
            m_mainRot = EGG::Quatf::ident;
        }
    }

    if (m_forceUpright) {
        stabilize();
    }

    if (EGG::Mathf::abs(m_mainRot.squaredNorm()) > std::numeric_limits<f32>::epsilon()) {
        m_mainRot.normalise();
    } else {
        m_mainRot = EGG::Quatf::ident;
    }

    m_fullRot = m_extraRot.multSwap(m_mainRot).multSwap(m_specialRot);
    m_fullRot.normalise();

    m_totalForce.setZero();
    m_totalTorque.setZero();
    m_angVel2.setZero();
}

/// @addr{0x805B4D24}
void KartDynamics::reset() {
    m_extVel.setZero();
    m_acceleration.setZero();
    m_angVel0.setZero();
    m_movingObjVel.setZero();
    m_angVel1.setZero();
    m_movingRoadVel.setZero();
    m_angVel2.setZero();
    m_totalForce.setZero();
    m_totalTorque.setZero();
    m_intVel.setZero();
}

/// @stage All
/// @brief Every frame, computes torque from linear motion and rotation.
/// @addr{0x805B6150}
/// @param p Position of the rigid body.
/// @param Flinear Linear motion force
/// @param Frot Rotational force
/// @param ignoreX If true, no x-axis torque is applied (e.g. when in a wheelie)
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

/// @addr{0x805B5CE8}
/// @stage 2
/// @brief Applies a force linearly and rotationally to the kart.
void KartDynamics::applyWrenchScaled(const EGG::Vector3f &p, const EGG::Vector3f &f, f32 scale) {
    m_totalForce += f;

    EGG::Vector3f invForceRot = m_fullRot.rotateVectorInv(f);
    EGG::Vector3f relPos = p - m_pos;
    EGG::Vector3f invPosRot = m_fullRot.rotateVectorInv(relPos);

    m_totalTorque += invPosRot.cross(invForceRot) * scale;
}

KartDynamicsBike::KartDynamicsBike() = default;

/// @addr{0x805B66E4}
KartDynamicsBike::~KartDynamicsBike() = default;

/// @addr{0x805B6438}
void KartDynamicsBike::forceUpright() {
    m_angVel0.z = 0.0f;
}

/// @stage All
/// @brief Stabilizes the bike by rotating towards the y-axis unit vector.
/// @addr{0x805B6448}
void KartDynamicsBike::stabilize() {
    EGG::Vector3f forward = m_top.cross(m_mainRot.rotateVector(EGG::Vector3f::ez)).cross(m_top);
    forward.normalise();
    EGG::Vector3f local_4c = forward.cross(m_top_.cross(forward));
    local_4c.normalise();

    EGG::Vector3f top = m_mainRot.rotateVector(EGG::Vector3f::ey);
    if (EGG::Mathf::abs(top.dot(local_4c)) >= 0.9999f) {
        return;
    }

    EGG::Quatf q;
    q.makeVectorRotation(top, local_4c);
    m_mainRot = m_mainRot.slerpTo(q.multSwap(m_mainRot), m_stabilizationFactor);
}

} // namespace Kart
