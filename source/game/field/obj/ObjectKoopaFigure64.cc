#include "game/field/obj/ObjectKoopaFigure64.hh"

#include "game/field/ObjectCollisionBase.hh"
#include "game/field/ObjectDirector.hh"

#include "game/system/RaceManager.hh"
#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

/// @addr{0x806DA914}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Determines if the statue is big based on param setting 2. Initializes @ref
/// m_startDelay based on param setting 3.
ObjectKoopaFigure64::ObjectKoopaFigure64(const System::MapdataGeoObj &params)
    : ObjectCollidable(params),
      m_isBigStatue(params.setting(1) == 1),
      m_startDelay(static_cast<u32>(params.setting(2))) {}

/// @addr{0x806DB114}
/// @brief Default virtual destructor
ObjectKoopaFigure64::~ObjectKoopaFigure64() = default;

/// @addr{0x806DAA44}
/// @copybrief ObjectBase::init()
/// @details If the statue is big, increases the statue's collision radius by a factor of 20.
/// Initializes @ref m_cycleFrame to @ref CYCLE_DURATION so that a new cycle can immediately start
/// and disables collision for big statues.
void ObjectKoopaFigure64::init() {
    constexpr f32 BIG_SCALE = 20.0f;

    if (m_isBigStatue) {
        const auto &flowTable = ObjectDirector::Instance()->flowTable();
        const auto *collisionSet = flowTable.set(flowTable.slot(id()));
        ASSERT(collisionSet);
        s16 radius = parse<s16>(collisionSet->params.cylinder.radius);
        resize(BIG_SCALE * static_cast<f32>(radius), 0.0f);
    }

    m_cycleFrame = CYCLE_DURATION;

    if (m_isBigStatue) {
        disableCollision();
    }
}

/// @addr{0x806DAB5C}
/// @copybrief ObjectBase::calc()
/// @details Waits until @ref m_startDelay frames have elapsed in the race. If the statue is at the
/// end of its cycle, then resets @ref m_cycleFrame to zero and enables collision for big statues.
/// If the big statue has been breathing fire for @ref FIRE_DURATION, then disables collision so the
/// statue is idle until the end of its cycle.
void ObjectKoopaFigure64::calc() {
    u32 timer = System::RaceManager::Instance()->timer();
    if (timer < m_startDelay) {
        return;
    }

    bool endOfCycle = CYCLE_DURATION == m_cycleFrame;
    if (endOfCycle) {
        m_cycleFrame = 0;

        if (m_isBigStatue) {
            enableCollision();
        }
    }

    if (m_cycleFrame >= FIRE_DURATION && !endOfCycle && m_isBigStatue) {
        disableCollision();
    }

    ++m_cycleFrame;
}

/// @addr{0x806DB154}
/// @copybrief ObjectBase::createCollision()
/// @details Collision is only created for the big statue, not the small ones.
void ObjectKoopaFigure64::createCollision() {
    if (m_isBigStatue) {
        ObjectCollidable::createCollision();
    }
}

/// @addr{0x806DAFB8}
/// @copybrief ObjectBase::calcCollisionTransform()
/// @details Moves the collision transform to match the fire position relative to the statue.
void ObjectKoopaFigure64::calcCollisionTransform() {
    constexpr EGG::Vector3f FIRE_POS_OFFSET = EGG::Vector3f(0.0f, -280.0f, 4000.0f);

    calcTransform();
    EGG::Matrix34f rotMat = transform();
    rotMat.setBase(3, EGG::Vector3f::zero);

    EGG::Matrix34f targetMat = EGG::Matrix34f::ident;
    targetMat.setBase(3, FIRE_POS_OFFSET);

    EGG::Matrix34f posMat = EGG::Matrix34f::ident;
    posMat.setBase(3, pos());

    m_collision->transform(posMat.multiplyTo(rotMat.multiplyTo(targetMat)), scale());
}

/// @addr{0x806C0854}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
/// @note The game defines this in an `ObjectSmoke` base class, but we don't implement it.
ObjectBase::LoadFlags ObjectKoopaFigure64::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x806DB168}
/// @copybrief ObjectCollidable::onCollision()
/// @param reactionOnKart The reaction that should be applied to the kart upon collision
/// @param hitDepth The depth of the collision along each axis
/// @return @ref Kart::Reaction::FireSpin
/// @details This is only called as a result of the player colliding with the fire that the
/// statue breathes. Also sets the hit depth to zero.
Kart::Reaction ObjectKoopaFigure64::onCollision(Kart::KartObject * /*kartObj*/,
        Kart::Reaction reactionOnKart, Kart::Reaction /*reactionOnObj*/, EGG::Vector3f &hitDepth) {
    hitDepth.setZero();
    return reactionOnKart;
}

} // namespace Kinoko::Field
