#include "ObjectPsea.hh"

#include "game/field/ObjectDirector.hh"

namespace Field {

/// @addr{0x8082C234}
ObjectPsea::ObjectPsea(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_period(F_TAU / static_cast<f32>(CYCLE_DURATION)) {
    constexpr EGG::Vector3f POS_OFFSET = EGG::Vector3f(0.0f, 6.75f, 0.0f) * 96.0f;
    constexpr EGG::Vector3f EXTRA_OFFSET = EGG::Vector3f(0.0f, 1.0f, 0.0f);

    m_frame = 0;
    m_initPosY = (m_pos + POS_OFFSET + EXTRA_OFFSET).y;

    auto *objDir = ObjectDirector::Instance();
    ASSERT(!objDir->psea());
    objDir->setPsea(this);
}

/// @addr{0x8082C890}
ObjectPsea::~ObjectPsea() = default;

/// @addr{0x8082C634}
void ObjectPsea::calc() {
    constexpr f32 POS_OFFSET = 9590.399f;
    constexpr f32 AMPLITUDE = 140.0f;

    f32 sin = EGG::Mathf::sin(m_period * static_cast<f32>(m_frame));

    if (CYCLE_DURATION < m_frame++) {
        m_frame = 0;
    }

    m_pos.y = m_initPosY + AMPLITUDE * sin + POS_OFFSET;
    m_flags.setBit(eFlags::Position);
}

} // namespace Field
