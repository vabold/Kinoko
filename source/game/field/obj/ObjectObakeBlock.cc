#include "ObjectObakeBlock.hh"

#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

/// @addr{0x8080AD20}
/// @copybrief ObjectBase::ObjectBase(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Caches the object's initial position to @ref m_initPos and initializes the block's
/// fall state to @ref FallState::Rest. Determines the frame at which the block will start to
/// fall using the param settings, where setting 2 is the number of seconds and setting 3 is the
/// number of additional frames; if both are 0, then the block will never fall. Zeroes @ref
/// m_framesFallen. Calculates @ref m_xzFallVel and @ref m_fallAngVel based on the initial
/// rotation of the block.
ObjectObakeBlock::ObjectObakeBlock(const System::MapdataGeoObj &params)
    : ObjectBase(params),
      m_initPos(params.pos()),
      m_fallState(FallState::Rest),
      m_fallFrame(static_cast<s32>(
              static_cast<s16>(params.setting(2)) + static_cast<s16>(params.setting(1)) * 60)) {
    constexpr f32 FALL_LINEAR_SPEED = 1.0f;
    constexpr f32 FALL_ANGULAR_SPEED = 0.02f;

    m_framesFallen = 0;
    m_xzFallVel.setZero();
    m_fallAngVel.setZero();

    f32 yRot = params.rot().y;

    if (yRot == 0.0f) {
        m_xzFallVel.z = -FALL_LINEAR_SPEED;
        m_fallAngVel.x = -FALL_ANGULAR_SPEED;
    } else if (yRot == 90.0f) {
        m_xzFallVel.x = -FALL_LINEAR_SPEED;
        m_fallAngVel.z = FALL_ANGULAR_SPEED;
    } else if (yRot == 180.0f) {
        m_xzFallVel.z = FALL_LINEAR_SPEED;
        m_fallAngVel.x = FALL_ANGULAR_SPEED;
    } else if (yRot == -90.0f) {
        m_xzFallVel.x = FALL_LINEAR_SPEED;
        m_fallAngVel.z = -FALL_ANGULAR_SPEED;
    }
}

/// @addr{0x8080D8FC}
/// @brief Default virtual destructor
ObjectObakeBlock::~ObjectObakeBlock() = default;

/// @addr{0x8080BC64}
/// @copybrief ObjectBase::calc()
/// @details If the block is at rest, then does nothing. Otherwise, updates the XZ position of
/// the block with a velocity of `2.0f` times @ref m_xzFallVel. Updates the block's rotation
/// based on @ref m_fallAngVel. Updates the block's height based on its initial height and a
/// downward gravitational force of `0.5f`. Finally, increments @ref m_framesFallen and
/// transitions the block to @ref FallState::FinishedFalling if the block has been falling for
/// more than 255 frames.
void ObjectObakeBlock::calc() {
    constexpr f32 XZ_SPEED_SCALAR = 2.0f;
    constexpr s32 FALL_DURATION = 255;
    constexpr f32 GRAVITY = 0.5f;

    if (m_fallState != FallState::Falling) {
        return;
    }

    setPos(m_initPos + m_xzFallVel * (static_cast<f32>(m_framesFallen) * XZ_SPEED_SCALAR));
    setRot(m_fallAngVel * static_cast<f32>(m_framesFallen));
    f32 posY = m_initPos.y -
            (GRAVITY * static_cast<f32>(m_framesFallen)) *
                    (GRAVITY * static_cast<f32>(m_framesFallen));
    setPos(EGG::Vector3f(pos().x, posY, pos().z));

    if (++m_framesFallen > FALL_DURATION) {
        m_fallState = FallState::FinishedFalling;
    }
}

/// @addr{0x8080BDE0}
/// @copybrief ObjectBase::load()
/// @details no-op because collision is handled by @ref ObjectObakeManager
void ObjectObakeBlock::load() {}

/// @addr{0x8080BDDC}
/// @copybrief ObjectBase::createCollision()
/// @details no-op because collision is handled by @ref ObjectObakeManager
void ObjectObakeBlock::createCollision() {}

/// @addr{0x8080BDD4}
/// @copybrief ObjectBase::calcCollisionTransform()
/// @details no-op because collision is handled by @ref ObjectObakeManager
void ObjectObakeBlock::calcCollisionTransform() {}

} // namespace Kinoko::Field
