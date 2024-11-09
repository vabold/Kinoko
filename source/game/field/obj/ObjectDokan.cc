#include "ObjectDokan.hh"

#include "game/field/CollisionDirector.hh"

namespace Field {

/// @addr{0x807787F0}
ObjectDokan::ObjectDokan(const System::MapdataGeoObj &params) : ObjectCollidable(params) {}

/// @addr{0x80778FEC}
ObjectDokan::~ObjectDokan() = default;

/// @addr{0x80778830}
void ObjectDokan::init() {
    m_b0 = false;
}

/// @addr{0x807788C8}
void ObjectDokan::calc() {
    constexpr f32 ACCEL = 2.0f;

    if (!m_b0) {
        return;
    }

    m_velocity.y -= ACCEL;
    m_pos += m_velocity;
    m_flags |= 1;

    calcFloor();
}

/// @addr{0x80778FE4}
u32 ObjectDokan::loadFlags() const {
    return 1;
}

/// @addr{0x807789BC}
void ObjectDokan::calcFloor() {
    constexpr f32 PIPE_RADIUS = 100.0f;
    constexpr f32 PIPE_SQRT_RADIUS = 10.0f;
    constexpr f32 ACCELERATION = 0.2f;

    CourseColMgr::CollisionInfo colInfo;
    EGG::Vector3f pos = m_pos;
    pos.y += PIPE_RADIUS;
    KCLTypeMask typeMask;

    if (!CollisionDirector::Instance()->checkSphereFull(PIPE_RADIUS, pos, EGG::Vector3f::inf,
                KCL_TYPE_64EBDFFF, &colInfo, &typeMask, 0)) {
        return;
    }

    if (typeMask & KCL_TYPE_FLOOR) {
        m_pos.y += colInfo.tangentOff.y;
        m_flags |= 1;
    } else {
        m_velocity.y *= -ACCELERATION;
        m_flags |= 1;
        m_pos.y += colInfo.tangentOff.y;

        if (m_velocity.length() < ACCELERATION * PIPE_SQRT_RADIUS) {
            m_b0 = false;
        }
    }
}

} // namespace Field
