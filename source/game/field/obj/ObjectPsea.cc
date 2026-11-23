#include "ObjectPsea.hh"

#include "game/field/ObjectDirector.hh"

namespace Kinoko::Field {

/// @addr{0x8082C234}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @details Calculates @ref m_initPosY based on hard-coded offsets. Initializes @ref m_frame to
/// zero. Finally, assigns this instance to the @ref ObjectDirector.
ObjectPsea::ObjectPsea(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_initPosY(calcInitPosY()) {
    m_frame = 0;

    auto *objDir = ObjectDirector::Instance();
    ASSERT(!objDir->psea());
    objDir->setPsea(this);
}

/// @addr{0x8082C890}
/// @brief Default virtual destructor
ObjectPsea::~ObjectPsea() = default;

/// @addr{0x8082C634}
/// @copybrief ObjectBase::calc()
/// @details Computes the position of the rising water using a sine wave with a period of @ref
/// ANG_FREQ. Increments @ref m_frame, wrapping around to 0 after it exceeds @ref PERIOD.
/// Finally, updates the Y-position of the rising water based on the computed sine value.
void ObjectPsea::calc() {
    constexpr f32 POS_OFFSET = 9590.399f;
    constexpr f32 AMPLITUDE = 140.0f;

    f32 sin = EGG::Mathf::sin(ANG_FREQ * static_cast<f32>(m_frame));

    if (PERIOD < m_frame++) {
        m_frame = 0;
    }

    f32 posY = m_initPosY + AMPLITUDE * sin + POS_OFFSET;
    setPos(EGG::Vector3f(pos().x, posY, pos().z));
}

/// @addr{0x8082C884}
/// @copybrief ObjectBase::loadGraphics()
/// @details This is a no-op in the base game.
void ObjectPsea::loadGraphics() {}

/// @copybrief ObjectBase::createCollision()
/// @details No-op because collision is managed via @ref Kart::KartMove::calcRisingWater().
void ObjectPsea::createCollision() {}

/// @addr{0x8082C888}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectPsea::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @brief Helper function to initialize @ref m_initPosY
/// @return The initial Y position of the rising water after applying hard-coded offsets
f32 ObjectPsea::calcInitPosY() const {
    constexpr EGG::Vector3f POS_OFFSET = EGG::Vector3f(0.0f, 6.75f, 0.0f) * 96.0f;
    constexpr EGG::Vector3f EXTRA_OFFSET = EGG::Vector3f(0.0f, 1.0f, 0.0f);

    return (pos() + POS_OFFSET + EXTRA_OFFSET).y;
}

} // namespace Kinoko::Field
