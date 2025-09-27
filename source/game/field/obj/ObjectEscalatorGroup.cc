#include "ObjectEscalatorGroup.hh"

#include "game/field/obj/ObjectEscalator.hh"

namespace Field {

/// @addr{0x8080178C}
ObjectEscalatorGroup::ObjectEscalatorGroup(const System::MapdataGeoObj &params)
    : ObjectCollidable(params) {
    constexpr f32 Y_OFFSET = 510.0f;
    constexpr f32 Z_OFFSET = 50.0f;
    constexpr EGG::Vector3f POS_OFFSET = EGG::Vector3f(0.0f, Y_OFFSET, Z_OFFSET);
    constexpr f32 ESCALATOR_X_OFFSET = 1465.0f;
    constexpr EGG::Vector3f RIGHT_OFFSET = EGG::Vector3f(ESCALATOR_X_OFFSET, -Y_OFFSET, -Z_OFFSET);
    constexpr EGG::Vector3f LEFT_OFFSET = EGG::Vector3f(-ESCALATOR_X_OFFSET, -Y_OFFSET, -Z_OFFSET);

    m_rightEscalator = new ObjectEscalator(params, false);
    m_leftEscalator = new ObjectEscalator(params, true);

    m_rightEscalator->m_initialPos = m_pos + POS_OFFSET + m_transform.multVector33(RIGHT_OFFSET);
    m_leftEscalator->m_initialPos = m_pos + POS_OFFSET + m_transform.multVector33(LEFT_OFFSET);

    m_rightEscalator->load();
    m_leftEscalator->load();
}

/// @addr{0x80802D20}
ObjectEscalatorGroup::~ObjectEscalatorGroup() = default;

} // namespace Field
