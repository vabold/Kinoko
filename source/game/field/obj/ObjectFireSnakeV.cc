#include "ObjectFireSnakeV.hh"

#include "game/field/CollisionDirector.hh"

#include "game/system/RaceManager.hh"

namespace Field {

/// @addr{0x806C2B70}
ObjectFireSnakeV::ObjectFireSnakeV(const System::MapdataGeoObj &params)
    : StateManager(this, STATE_ENTRIES), ObjectFireSnake(params),
      m_cycleDuration(params.setting(1)), m_distFromPipe(static_cast<f32>(params.setting(2))),
      m_fallSpeed(0.0f) {
    m_delayFrame = params.setting(0);
    m_sunPos = m_pos;

    calcTransform();

    m_initRot = m_transform.base(0);
    m_initialPos = m_sunPos + m_initRot * m_distFromPipe;
}

/// @addr{0x806C3548}
ObjectFireSnakeV::~ObjectFireSnakeV() = default;

/// @addr{0x806C2CBC}
void ObjectFireSnakeV::init() {
    m_nextStateId = 1;
    ObjectFireSnake::enterDespawned();

    m_visualPos = m_sunPos;
    m_bounceDir = m_initRot;
}

/// @addr{0x806C2D54}
void ObjectFireSnakeV::calc() {
    if (System::RaceManager::Instance()->timer() >= static_cast<u32>(m_delayFrame)) {
        calcSub();
    }
}

/// @addr{0x806C2DA4}
void ObjectFireSnakeV::calcSub() {
    StateManager::calc();

    u32 frame = System::RaceManager::Instance()->timer() - static_cast<u32>(m_delayFrame);
    if (frame % static_cast<u32>(m_cycleDuration) == 0) {
        m_nextStateId = 1;
    }

    if (m_currentStateId >= 1 && m_currentStateId <= 4) {
        ++m_age;
        if (m_age >= 600 && m_currentStateId == 3) {
            m_nextStateId = 0;
        }
    }

    calcChildren();
}

/// @addr{0x806C30F8}
void ObjectFireSnakeV::enterFalling() {
    constexpr f32 FALL_DURATION = 140.0f;

    ObjectFireSnake::enterFalling();

    if (getUnit()) {
        unregisterCollision();
    }

    m_age = 0;
    m_visualPos = m_sunPos;
    m_bounceDir = m_initRot;
    m_initialPos = m_sunPos + m_initRot * m_distFromPipe;
    m_fallSpeed = m_distFromPipe / FALL_DURATION;
}

/// @addr{0x806C31F0}
void ObjectFireSnakeV::calcFalling() {
    constexpr f32 INITIAL_Y_VELOCITY = 120.0f;
    constexpr f32 AABB_DELAY_FRAMES = 5;

    if (m_currentFrame > AABB_DELAY_FRAMES && !getUnit()) {
        loadAABB(0.0f);
    }

    m_visualPos.x += m_bounceDir.x * m_fallSpeed;
    m_visualPos.z += m_bounceDir.z * m_fallSpeed;
    m_visualPos.y += INITIAL_Y_VELOCITY - GRAVITY * static_cast<f32>(m_currentFrame);

    if (m_currentFrame > COL_CHECK_DELAY_FRAMES) {
        CollisionInfo colInfo;

        bool hasCol = CollisionDirector::Instance()->checkSphereFull(RADIUS, m_visualPos,
                EGG::Vector3f::inf, KCL_TYPE_FLOOR, &colInfo, nullptr, 0);

        if (hasCol) {
            m_visualPos += colInfo.tangentOff;
            m_nextStateId = 2;
        }
    }

    setMatrixTangentTo(EGG::Vector3f::ey, m_bounceDir);
    m_flags.setBit(eFlags::Position);
    m_pos = m_visualPos;
}

/// @addr{0x806C33C4}
void ObjectFireSnakeV::calcHighBounce() {
    constexpr f32 INITIAL_Y_VELOCITY = 90.0f;

    m_visualPos.z += m_bounceDir.z * m_fallSpeed;
    m_visualPos.x += m_bounceDir.x * m_fallSpeed;
    m_visualPos.y += INITIAL_Y_VELOCITY - GRAVITY * static_cast<f32>(m_currentFrame);

    if (m_currentFrame > COL_CHECK_DELAY_FRAMES) {
        CollisionInfo colInfo;

        bool hasCol = CollisionDirector::Instance()->checkSphereFull(RADIUS, m_visualPos,
                EGG::Vector3f::inf, KCL_TYPE_FLOOR, &colInfo, nullptr, 0);

        if (hasCol) {
            m_visualPos += colInfo.tangentOff;
            m_nextStateId = 3;
        }
    }

    setMatrixTangentTo(EGG::Vector3f::ey, m_bounceDir);
    m_flags.setBit(eFlags::Position);
    m_pos = m_visualPos;
}

} // namespace Field
