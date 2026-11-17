#include "ObjectEscalatorGroup.hh"

#include "game/field/obj/ObjectEscalator.hh"

#include <egg/core/Heap.hh>

namespace Kinoko::Field {

/// @addr{0x8080178C}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Applies a vertical and forward offset to represent the true center of the escalator
/// group. Additionally adds a 75% scale to expand the collision radius of the group. Creates
/// and loads the left and right escalator and positions them relative to the group.
ObjectEscalatorGroup::ObjectEscalatorGroup(const System::MapdataGeoObj &params)
    : ObjectCollidable(params),
      m_rightEscalator(EGG::egg_new<ObjectEscalator>(params, false)),
      m_leftEscalator(EGG::egg_new<ObjectEscalator>(params, true)) {
    constexpr f32 X_OFFSET = 1465.0f;
    constexpr f32 Y_OFFSET = 510.0f;
    constexpr f32 Z_OFFSET = 50.0f;
    constexpr EGG::Vector3f POS_OFFSET = EGG::Vector3f(0.0f, Y_OFFSET, Z_OFFSET);
    constexpr EGG::Vector3f RIGHT_OFFSET = EGG::Vector3f(X_OFFSET, -Y_OFFSET, -Z_OFFSET);
    constexpr EGG::Vector3f LEFT_OFFSET = EGG::Vector3f(-X_OFFSET, -Y_OFFSET, -Z_OFFSET);
    constexpr EGG::Vector3f SCALE = EGG::Vector3f(1.75f, 1.75f, 1.75f);

    addPos(POS_OFFSET);
    setScale(SCALE);

    calcTransform();
    m_rightEscalator->m_initialPos = pos() + transform().multVector33(RIGHT_OFFSET);
    m_leftEscalator->m_initialPos = pos() + transform().multVector33(LEFT_OFFSET);

    m_rightEscalator->load();
    m_leftEscalator->load();
}

/// @addr{0x80802D20}
/// @brief Default virtual destructor
ObjectEscalatorGroup::~ObjectEscalatorGroup() = default;

/// @copybrief ObjectBase::createCollision()
/// @details Not overridden in the base game, but it's effectively a no-op because the
/// corresponding kcl (`monte_a`) doesn't have any primitive collision.
void ObjectEscalatorGroup::createCollision() {}

/// @addr{0x80802D18}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectEscalatorGroup::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x80802D00}
/// @copybrief ObjectBase::getResources()
/// @return The resource name for the dancing Piantas between the escalators (`monte_a`).
const char *ObjectEscalatorGroup::getResources() const {
    return "monte_a";
}

/// @addr{0x80802D0C}
/// @copybrief ObjectBase::getKclName()
/// @return The model name of the dancing Piantas between the escalators (`monte_a`).
const char *ObjectEscalatorGroup::getKclName() const {
    return "monte_a";
}

} // namespace Kinoko::Field
