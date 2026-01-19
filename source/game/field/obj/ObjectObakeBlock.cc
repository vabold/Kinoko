#include "ObjectObakeBlock.hh"

namespace Field {

/// @addr{0x8080AD20}
ObjectObakeBlock::ObjectObakeBlock(const System::MapdataGeoObj &params)
    : ObjectBase(params), m_initialPos(params.pos()), m_fallState(FallState::Rest),
      m_fallFrame(static_cast<s32>(params.setting(2)) + static_cast<s32>(params.setting(1)) * 60) {
    constexpr f32 SI_WIDTH = 162.5f; // Half-width of block in the spatial index
    constexpr f32 FALL_LINEAR_SPEED = 1.0f;
    constexpr f32 FALL_ANGULAR_SPEED = 0.02f;

    m_framesFallen = 0;
    m_fallVel.setZero();
    m_fallAngVel.setZero();

    f32 yRot = params.rot().y;

    if (yRot == 0.0f) {
        m_fallVel.z = -FALL_LINEAR_SPEED;
        m_fallAngVel.x = -FALL_ANGULAR_SPEED;
        m_bbox.min = m_pos + EGG::Vector3f(SI_WIDTH, SI_WIDTH, SI_WIDTH);
        m_bbox.max = m_pos + EGG::Vector3f(-SI_WIDTH, SI_WIDTH, SI_WIDTH);
    } else if (yRot == 90.0f) {
        m_fallVel.x = -FALL_LINEAR_SPEED;
        m_fallAngVel.z = FALL_ANGULAR_SPEED;
        m_bbox.min = m_pos + EGG::Vector3f(SI_WIDTH, SI_WIDTH, SI_WIDTH);
        m_bbox.max = m_pos + EGG::Vector3f(SI_WIDTH, SI_WIDTH, -SI_WIDTH);
    } else if (yRot == 180.0f) {
        m_fallVel.z = FALL_LINEAR_SPEED;
        m_fallAngVel.x = FALL_ANGULAR_SPEED;
        m_bbox.min = m_pos + EGG::Vector3f(SI_WIDTH, SI_WIDTH, -SI_WIDTH);
        m_bbox.max = m_pos + EGG::Vector3f(-SI_WIDTH, SI_WIDTH, -SI_WIDTH);
    } else if (yRot == -90.0f) {
        m_fallVel.x = FALL_LINEAR_SPEED;
        m_fallAngVel.z = -FALL_ANGULAR_SPEED;
        m_bbox.min = m_pos + EGG::Vector3f(-SI_WIDTH, SI_WIDTH, SI_WIDTH);
        m_bbox.max = m_pos + EGG::Vector3f(-SI_WIDTH, SI_WIDTH, -SI_WIDTH);
    }
}

/// @addr{0x8080D8FC}
ObjectObakeBlock::~ObjectObakeBlock() = default;

/// @addr{0x8080BC64}
void ObjectObakeBlock::calc() {
    constexpr s32 FALL_DURATION = 255;

    if (m_fallState != FallState::Falling) {
        return;
    }

    m_pos = m_initialPos + m_fallVel * (static_cast<f32>(m_framesFallen) * 2.0f);
    m_rot = m_fallAngVel * static_cast<f32>(m_framesFallen);
    m_pos.y = m_initialPos.y -
            (0.5f * static_cast<f32>(m_framesFallen)) * (0.5f * static_cast<f32>(m_framesFallen));
    m_flags.setBit(eFlags::Position, eFlags::Rotation);

    if (++m_framesFallen > FALL_DURATION) {
        m_fallState = FallState::FinishedFalling;
    }
}

} // namespace Field
