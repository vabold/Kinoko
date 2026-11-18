#include "SphereLink.hh"

#include "game/field/CollisionDirector.hh"

namespace Kinoko::Field {

/// @brief Constructor
/// @details Sets @ref m_prev and @ref m_next to `nullptr`.
SphereLink::SphereLink() : m_prev(nullptr), m_next(nullptr) {}

/// @brief Default destructor
SphereLink::~SphereLink() = default;

/// @addr{0x806F1194}
/// @brief Initializes the link's length and resets its internal state
/// @param length The maximum distance between this link and the previous link
void SphereLink::initLinkLen(f32 length) {
    m_linkLen = length;
    init();
}

/// @brief Resets the link's internal state
/// @details Sets @ref m_pos, @ref m_vel, and @ref m_springForce to zero, @ref m_smoothedUp to
/// the world up direction, and sets @ref m_touchingGround to `true`.
void SphereLink::init() {
    m_pos.setZero();
    m_vel.setZero();
    m_springForce.setZero();
    m_smoothedUp = EGG::Vector3f::ey;
    m_touchingGround = true;
}

/// @addr{0x806F2074}
/// @brief Calculates the spring force between this link and the previous link
/// @pre This link must not be the leader (i.e., it must have a @ref m_prev link)
/// @details If there is slack in the chain, then this function early returns. Applies a spring
/// damper force which will pull this link back towards its previous link when it is stretched too
/// far away. Subtracts the resulting increase in force from the previous (forward) link's spring
/// force so that the previous link experiences an equal and opposite reaction.
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
/// @brief Calculates the spring force between this link and its neighbors
/// @details If this is the leader link, the function early returns since we do not want to apply
/// any spring forces to it. If this is the last link in the chain, it only applies the spring force
/// from the previous (forward) link. Otherwise, links in the middle of the chain call @ref
/// calcSpring() and @ref calcStiffness() to enforce the spring constraints between this link and
/// its neighbors.
void SphereLink::calc() {
    if (isLeader()) {
        return;
    }

    if (!m_next) {
        EGG::Vector3f dir = m_prev->m_pos - m_pos;
        f32 dist = dir.normalise();

        if (dist > m_prev->m_linkLen) {
            m_springForce += dir * SPRING_FORCE_MAGNITUDE;
        }

        calcStiffness();
    } else {
        calcSpring();
        calcStiffness();
    }
}

/// @addr{0x806F2490}
/// @brief Enforces positional constraints to keep this link within a certain distance of its
/// previous link
/// @param scale A multiplier for the link length (always 1.0f)
/// @pre This link must not be the leader link.
/// @details This is called after all links have had their spring forces calculated and positions
/// updated accordingly. With the positions updated, this function ensures that this link remains
/// within the allowed distance from its previous link.
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
/// @brief Checks for floor collisions, updates position accordingly, and updates the up vector
/// @details Performs a sphere collision check with a radius of `55.0f` and a position offset by
/// `55.0f` towards the link's up vector. If a collision is detected, the link's position and
/// velocity are updated accordingly and @ref m_springForce is set to @ref GRAVITY in order to
/// cancel out the effect of gravity. For collisions where the push-out vector is steeper than
/// `~45.6°` from vertical, only the y-component of position is updated. Regardless of whether a
/// collision occurs, the link's up vector is interpolated towards the floor normal.
void SphereLink::checkCollision() {
    constexpr f32 RADIUS = 55.0f;
    constexpr f32 UP_INTERP_RATE = 0.1f;
    constexpr f32 FLOOR_VEL_DAMPEN = 0.5f;
    constexpr f32 FLOOR_PUSH_MIN_Y = 0.7f;

    EGG::Vector3f floorNrm = EGG::Vector3f::ey;

    CollisionInfo info;
    KCLTypeMask mask;
    EGG::Vector3f pos = m_pos + m_smoothedUp * RADIUS;

    auto *colDir = CollisionDirector::Instance();
    m_touchingGround = colDir->checkSphereFullPush(RADIUS, pos, EGG::Vector3f::inf, KCL_TYPE_FLOOR,
            &info, &mask, 0);

    if (m_touchingGround) {
        EGG::Vector3f tangentOff = info.tangentOff;

        if (tangentOff.squaredLength() > std::numeric_limits<f32>::epsilon()) {
            tangentOff.normalise2();
        }

        if (EGG::Mathf::abs(tangentOff.y) > FLOOR_PUSH_MIN_Y) {
            m_pos.y += info.tangentOff.y;
        } else {
            m_pos += info.tangentOff;
        }

        if (info.floorDist > -std::numeric_limits<f32>::min()) {
            floorNrm = info.floorNrm;
        }

        m_vel = m_smoothedUp * m_vel.dot(m_smoothedUp) * FLOOR_VEL_DAMPEN;
        m_springForce = GRAVITY;
    }

    m_smoothedUp += (floorNrm - m_smoothedUp) * UP_INTERP_RATE;
}

/// @brief Calculates the new position of the link based on its velocity and spring force
/// @details Updates @ref m_vel based on the spring force and @ref GRAVITY with a damping
/// factor. Updates @ref m_pos accordingly and resets @ref m_springForce to zero.
void SphereLink::calcPos() {
    constexpr f32 VEL_DAMPING = 0.9f;

    m_vel += m_springForce - GRAVITY;
    m_vel *= VEL_DAMPING;
    m_pos += m_vel;
    m_springForce.setZero();
}

/// @addr{0x806F1C58}
/// @brief Calculates the spring force between this link and its neighbors
/// @pre This link must have both a previous and a next link (i.e., it must not be the leader or the
/// last link in the chain).
/// @details If this link is further away than its allowed link length from either its previous or
/// next link, a spring force of magnitude @ref SPRING_FORCE_MAGNITUDE is applied to bring it back
/// within the allowed distance.
void SphereLink::calcSpring() {
    ASSERT(m_prev && m_next);

    EGG::Vector3f dir = m_prev->m_pos - m_pos;
    f32 dist = dir.normalise();

    if (dist > m_prev->m_linkLen) {
        m_springForce += dir * SPRING_FORCE_MAGNITUDE;
    }

    dir = m_next->m_pos - m_pos;
    f32 nextDist = dir.normalise();

    if (nextDist > m_linkLen) {
        m_springForce += dir * SPRING_FORCE_MAGNITUDE;
    }
}

} // namespace Kinoko::Field
