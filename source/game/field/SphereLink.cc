#include "SphereLink.hh"

#include "game/field/CollisionDirector.hh"

namespace Field {

SphereLink::SphereLink() : m_prev(nullptr), m_next(nullptr) {}

SphereLink::~SphereLink() = default;

/// @addr{0x806F1194}
void SphereLink::initLinkLen(f32 length) {
    m_linkLen = length;
    init();
}

void SphereLink::init() {
    m_pos.setZero();
    m_vel.setZero();
    m_springForce.setZero();
    m_up = EGG::Vector3f::ey;
    m_touchingGround = true;
}

/// @addr{0x806F2074}
void SphereLink::calcStiffness() {
    constexpr f32 STIFFNESS = 0.15f;

    ASSERT(!isLeader());

    EGG::Vector3f dir = m_prev->m_pos - m_pos;
    f32 dist = dir.normalise();

    // Nothing to do if there is slack in the chain
    if (dist < m_prev->m_linkLen) {
        return;
    }

    EGG::Vector3f springForce = dir * (STIFFNESS * (dist - m_prev->m_linkLen));
    EGG::Vector3f velDamping = dir * (-STIFFNESS * (m_prev->m_vel - m_vel).dot(dir));
    EGG::Vector3f totalForce = springForce + velDamping;

    m_springForce += totalForce;

    if (!m_prev->isLeader()) {
        m_prev->m_springForce -= totalForce;
    }
}

/// @addr{0x806F1A54}
void SphereLink::calc() {
    if (isLeader()) {
        return;
    }

    if (!m_next) {
        EGG::Vector3f dir = m_prev->m_pos - m_pos;
        f32 dist = dir.normalise();

        if (dist > m_prev->m_linkLen) {
            m_springForce += dir * 2.0f;
        }

        calcStiffness();
    } else {
        calcSpring();
        calcStiffness();
    }
}

/// @addr{0x806F2490}
void SphereLink::calcConstraints(f32 scale) {
    constexpr f32 MIN_PITCH_ANGLE = DEG2FIDX * -75.0f;
    STATIC_ASSERT(MIN_PITCH_ANGLE == -53.333336f);

    ASSERT(!isLeader());

    EGG::Vector3f back = m_prev->m_pos - m_pos;
    f32 dist = back.normalise();
    f32 scaledLen = m_prev->m_linkLen * scale;

    // Nothing to do if there is slack in the chain
    if (dist <= scaledLen) {
        return;
    }

    EGG::Vector3f forward = -back;
    f32 minPitch = EGG::Mathf::SinFIdx(MIN_PITCH_ANGLE);

    if (forward.y > minPitch && !m_touchingGround) {
        EGG::Vector3f horiz = forward;
        if (forward.y >= 0.0f) {
            horiz.x *= -1.0f;
            horiz.z *= -1.0f;
        }
        horiz.y = 0.0f;
        horiz.normalise2();

        EGG::Vector3f axis = forward.cross(horiz);
        axis.normalise2();
        EGG::Matrix34f mat = EGG::Matrix34f::ident;
        mat.setAxisRotation(0.0f, axis);
        EGG::Vector3f newForward = mat.ps_multVector(forward);
        newForward.normalise2();

        m_vel.setZero();
        m_pos = m_prev->m_pos + newForward * scaledLen;
    } else {
        // Wiggler is falling steeply. Snap towards previous link.
        m_vel.setZero();
        m_pos += back * (dist - scaledLen);
    }
}

/// @addr{0x806F2A38}
void SphereLink::checkCollision() {
    constexpr f32 RADIUS = 55.0f;

    EGG::Vector3f floorNrm = EGG::Vector3f::ey;

    CollisionInfo info;
    KCLTypeMask mask;
    EGG::Vector3f pos = m_pos + m_up * RADIUS;

    auto *colDir = CollisionDirector::Instance();
    m_touchingGround = colDir->checkSphereFullPush(RADIUS, pos, EGG::Vector3f::inf, KCL_TYPE_FLOOR,
            &info, &mask, 0);

    if (m_touchingGround) {
        EGG::Vector3f tangentOff = info.tangentOff;

        if (tangentOff.squaredLength() > std::numeric_limits<f32>::epsilon()) {
            tangentOff.normalise2();
        }

        if (EGG::Mathf::abs(tangentOff.y) > 0.7f) {
            m_pos.y += info.tangentOff.y;
        } else {
            m_pos += info.tangentOff;
        }

        if (info.floorDist > -std::numeric_limits<f32>::min()) {
            floorNrm = info.floorNrm;
        }

        m_vel = m_up * m_vel.dot(m_up) * 0.5f;
        m_springForce = GRAVITY;
    }

    m_up += (floorNrm - m_up) * 0.1f;
}

void SphereLink::calcPos() {
    m_vel += m_springForce - GRAVITY;
    m_vel *= 0.9f;
    m_pos += m_vel;
    m_springForce.setZero();
}

/// @addr{0x806F1C58}
void SphereLink::calcSpring() {
    ASSERT(m_prev && m_next);

    EGG::Vector3f dir = m_prev->m_pos - m_pos;
    f32 dist = dir.normalise();

    if (dist > m_prev->m_linkLen) {
        m_springForce += dir * 2.0f;
    }

    dir = m_next->m_pos - m_pos;
    f32 nextDist = dir.normalise();

    if (nextDist > m_linkLen) {
        m_springForce += dir * 2.0f;
    }
}

} // namespace Field
