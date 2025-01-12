#include "ObjectDokan.hh"

#include "game/field/CollisionDirector.hh"

#include "game/kart/KartCollide.hh"

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

/// @addr{0x80778D50}
void ObjectDokan::calcCollisionTransform() {
    if (m_id == ObjectId::DokanSFC) {
        ObjectCollidable::calcCollisionTransform();
    } else {
        // rMR piranhas
        calcTransform();
        EGG::Matrix34f mat = m_transform;
        mat.setBase(3, mat.translation() + EGG::Vector3f::ey * 300.0f);
        m_collision->transform(mat, m_scale, getCollisionTranslation());
    }
}

/// @addr{0x80778C0C}
Kart::Reaction ObjectDokan::onCollision(Kart::KartObject * /*kartObj*/,
        Kart::Reaction reactionOnKart, Kart::Reaction reactionOnObj, EGG::Vector3f & /*hitDepth*/) {
    constexpr f32 INITIAL_VELOCITY = 100.0f;

    if (reactionOnObj == Kart::Reaction::UNK_3 || reactionOnObj == Kart::Reaction::UNK_5) {
        if (!m_b0) {
            m_b0 = true;
            m_velocity = INITIAL_VELOCITY * EGG::Vector3f::ey;
        }
    }

    return reactionOnKart;
}

/// @addr{0x807789BC}
void ObjectDokan::calcFloor() {
    constexpr f32 PIPE_RADIUS = 100.0f;
    constexpr f32 PIPE_SQRT_RADIUS = 10.0f;
    constexpr f32 ACCELERATION = 0.2f;

    CollisionInfo colInfo;
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
