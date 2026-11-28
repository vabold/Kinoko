#include "ObjectDokan.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/ObjectCollisionBase.hh"

#include "game/kart/Reaction.hh"

namespace Kinoko::Field {

/// @addr{0x807787F0}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
ObjectDokan::ObjectDokan(const System::MapdataGeoObj &params) : ObjectCollidable(params) {}

/// @addr{0x80778FEC}
/// @brief Default virtual destructor
ObjectDokan::~ObjectDokan() = default;

/// @addr{0x80778830}
/// @copybrief ObjectBase::init()
/// @details Sets @ref m_isAirborne to `false`.
void ObjectDokan::init() {
    m_isAirborne = false;
}

/// @addr{0x807788C8}
/// @copybrief ObjectBase::calc()
/// @details If the pipe is not airborne, returns early. Otherwise, updates the pipe's position
/// and checks for floor collisions. In Kinoko, this is effectively a no-op since karts can
/// never send the pipe airborne in time trials, but we implement @ref calcPos() and
/// @ref calcFloor() for completeness.
void ObjectDokan::calc() {
    if (!m_isAirborne) {
        return;
    }

    calcPos();
    calcFloor();
}

/// @addr{0x80778D50}
/// @copybrief ObjectBase::calcCollisionTransform()
/// @details The collision transform varies between the SNES Mario Circuit 3 pipes and the N64
/// Mario Raceway piranhas. SNES Mario Circuit 3 pipes (identified with @ref ObjectId::DokanSFC)
/// simply call @ref ObjectCollidable::calcCollisionTransform(). N64 Mario Raceway piranhas
/// (identified with @ref ObjectId::PuchiPakkun), on the other hand, offset their collision 300
/// units upward.
void ObjectDokan::calcCollisionTransform() {
    if (m_id == ObjectId::DokanSFC) {
        ObjectCollidable::calcCollisionTransform();
    } else {
        calcTransform();
        EGG::Matrix34f mat = transform();
        mat.setBase(3, mat.translation() + EGG::Vector3f::ey * 300.0f);
        m_collision->transform(mat, scale(), getCollisionTranslation());
    }
}

/// @addr{0x80778FE4}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectDokan::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x80778C0C}
/// @copybrief ObjectCollidable::onCollision()
/// @param reactionOnKart The reaction that should be applied to the kart upon collision
/// @param reactionOnObj The reaction that should be applied to the object upon collision
/// @return For time trials, always returns @ref Kart::Reaction::WallSpark.
Kart::Reaction ObjectDokan::onCollision(Kart::KartObject * /*kartObj*/,
        Kart::Reaction reactionOnKart, Kart::Reaction reactionOnObj, EGG::Vector3f & /*hitDepth*/) {
    if (reactionOnObj == Kart::Reaction::UNK_3 || reactionOnObj == Kart::Reaction::UNK_5) {
        tryStartAirborne();
    }

    return reactionOnKart;
}

/// @addr{0x8077894C}
/// @brief Applies gravity to velocity and updates position accordingly
/// @details Gravity applies a `2.0f` unit downward acceleration to the pipe's velocity each
/// frame.
void ObjectDokan::calcPos() {
    constexpr f32 GRAVITY = 2.0f;

    m_velocity.y -= GRAVITY;
    addPos(m_velocity);
}

/// @addr{0x807789BC}
/// @brief Performs a collision check against the floor to stop the pipe if it's falling
/// @details If the pipe is colliding with the floor, then flips the direction of the pipe's
/// vertical velocity and dampens it by a factor of `0.2f`. If the pipe's speed is then less than
/// `2.0f`, @ref m_isAirborne is reset to `false`.
void ObjectDokan::calcFloor() {
    constexpr f32 PIPE_RADIUS = 100.0f;
    constexpr f32 PIPE_SQRT_RADIUS = 10.0f;
    constexpr f32 DAMPEN_FACTOR = 0.2f;

    CollisionInfo colInfo;
    EGG::Vector3f colPos = pos();
    colPos.y += PIPE_RADIUS;
    KCLTypeMask typeMask;

    if (!CollisionDirector::Instance()->checkSphereFull(PIPE_RADIUS, colPos, EGG::Vector3f::inf,
                KCL_TYPE_OBJECT_COLLIDABLE, &colInfo, &typeMask, 0)) {
        return;
    }

    addPos(EGG::Vector3f(0.0f, colInfo.tangentOff.y, 0.0f));

    if (typeMask & KCL_TYPE_FLOOR) {
        m_velocity.y *= -DAMPEN_FACTOR;
        if (m_velocity.length() < DAMPEN_FACTOR * PIPE_SQRT_RADIUS) {
            m_isAirborne = false;
        }
    }
}

/// @addr{0x80778BA0}
/// @brief If the pipe is not already airborne, induces upward velocity
/// @details The pipe starts with `100.0f` units of upward velocity when it becomes airborne.
/// This function does nothing if the pipe is already airborne.
void ObjectDokan::tryStartAirborne() {
    constexpr f32 INITIAL_VELOCITY = 100.0f;

    if (!m_isAirborne) {
        m_isAirborne = true;
        m_velocity = INITIAL_VELOCITY * EGG::Vector3f::ey;
    }
}

} // namespace Kinoko::Field
