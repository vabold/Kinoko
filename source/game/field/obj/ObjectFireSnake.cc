#include "ObjectFireSnake.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/ObjectDirector.hh"

#include "game/system/RaceManager.hh"

namespace Field {

/// @addr{0x806C0D18}
ObjectFireSnakeKid::ObjectFireSnakeKid(const System::MapdataGeoObj &params)
    : ObjectCollidable(params) {}

/// @addr{0x806C2ACC}
ObjectFireSnakeKid::~ObjectFireSnakeKid() = default;

/// @addr{0x806C0F30}
ObjectFireSnake::ObjectFireSnake(const System::MapdataGeoObj &params)
    : StateManager(this, STATE_ENTRIES), ObjectProjectile(params), m_initialPos(params.pos()),
      m_maxAge(params.setting(1)) {
    if (ObjectDirector::Instance()->managedObjects().size() > 0) {
        registerManagedObject();
    }

    for (u32 i = 0; i < 2; ++i) {
        auto *&kid = m_kids[i];
        kid = new ObjectFireSnakeKid(params);
        kid->load();
        f32 scale = (0.75f - 0.25f * static_cast<f32>(i)) * m_scale.y;
        kid->setScale(EGG::Vector3f(scale, scale, scale));
    }

    m_prevTransforms.fill(EGG::Matrix34f::ident);
}

/// @addr{0x806C1284}
ObjectFireSnake::~ObjectFireSnake() = default;

/// @addr{0x806C13B0}
void ObjectFireSnake::init() {
    m_nextStateId = 0;
    enterDespawned();

    m_sunPos.setZero();

    calcTransform();
    m_initRot = m_transform.base(0);
    m_bounceDir = m_transform.base(0);
}

/// @addr{0x806C14E4}
void ObjectFireSnake::calc() {
    StateManager::calc();

    if (isCollisionEnabled()) {
        if (++m_age >= static_cast<u16>(m_maxAge) && m_currentStateId == 3) {
            m_nextStateId = 0;
        }
    }

    calcChildren();
}

/// @addr{0x806C23C8}
void ObjectFireSnake::initProjectile(const EGG::Vector3f &pos) {
    m_sunPos = pos;

    m_xzSunDist = m_initialPos - m_sunPos;
    m_xzSunDist.y = 0.0f;
    m_xzSunDist.normalise2();

    EGG::Vector3f xzDist = m_sunPos - m_initialPos;
    xzDist.y = 0.0f;
    f32 dist = xzDist.length();

    m_fallAxis = RotateXZByYaw(F_PI / 2.0f, m_xzSunDist);
    m_xzSpeed = dist * EGG::Mathf::sqrt(GRAVITY * 0.5f / (m_sunPos.y - m_initialPos.y));
    m_fallDuration = static_cast<u16>(dist / m_xzSpeed);
}

/// @addr{0x806C29FC}
void ObjectFireSnake::onLaunch() {
    m_nextStateId = 1;
}

/// @addr{0x806C1930}
void ObjectFireSnake::enterDespawned() {
    if (getUnit()) {
        unregisterCollision();
    }

    m_visualPos = m_sunPos;
    m_pos = m_sunPos;
    m_flags.setBit(eFlags::Position);
}

/// @addr{0x806C19E8}
void ObjectFireSnake::enterFalling() {
    if (!getUnit()) {
        loadAABB(0.0f);
    }
}

/// @addr{0x806C1DCC}
void ObjectFireSnake::enterHighBounce() {
    m_visualPos = m_initialPos;
    m_pos = m_initialPos;
    m_flags.setBit(eFlags::Position);

    f32 rand = System::RaceManager::Instance()->random().getF32();
    m_bounceDir = rand >= 0.5f ? m_initRot : -m_initRot;

    m_age = 0;
}

/// @addr{0x806C2000}
void ObjectFireSnake::enterRest() {
    constexpr f32 XZ_RANGE = 1000.0f * 1000.0f;

    // If it strays too far from original position, start bouncing the opposite direction
    if (m_visualPos.sqDistance(m_initialPos) >= XZ_RANGE) {
        m_bounceDir = m_initialPos - m_visualPos;
        m_bounceDir.normalise2();
    } else {
        // 45 degree random fluctuation, with a 50% chance to flip direction
        auto &rand = System::RaceManager::Instance()->random();
        f32 angle = rand.getF32(90.0f) - 45.0f;

        if (rand.getF32() >= 0.5f) {
            angle += 180.0f;
        }

        m_bounceDir = RotateXZByYaw(angle * DEG2RAD, m_initRot);
    }
}

/// @addr{0x806C1A88}
void ObjectFireSnake::calcFalling() {
    EGG::Vector2f fallVel;
    fallVel.y = -GRAVITY * static_cast<f32>(m_currentFrame);
    f32 fallVelY = fallVel.y;
    f32 posY = m_visualPos.y + fallVel.y;

    if (posY <= m_initialPos.y) {
        m_nextStateId = 2;
        m_pos = m_visualPos * 0.5f + m_initialPos * 0.5f;
        m_flags.setBit(eFlags::Position);
    } else {
        fallVel.x = m_xzSpeed;
        m_visualPos.y = posY;
        m_visualPos.x += m_xzSunDist.x * fallVel.x;
        m_visualPos.z += m_xzSunDist.z * fallVel.x;
        fallVel.normalise2();

        f32 angle = fallVel.x;
        EGG::Vector3f up = RotateAxisAngle(EGG::Mathf::acos(angle), m_fallAxis, EGG::Vector3f::ey);
        auto axis = EGG::Vector3f(m_xzSunDist.x * m_xzSpeed, -fallVelY, m_xzSunDist.z * m_xzSpeed);
        axis.normalise2();
        auto spiral = RotateAxisAngle(50.0f * static_cast<f32>(m_currentFrame) * DEG2RAD, axis, up);

        f32 scaledDuration = 0.9f * static_cast<f32>(m_fallDuration);
        f32 phase = m_currentFrame - static_cast<u16>(scaledDuration);
        f32 spiralRadius = phase < 0 ? 200.0f : 200.0f - (200.0f / scaledDuration) * phase;

        m_pos = m_visualPos + spiral * spiralRadius;
        m_flags.setBit(eFlags::Position);
    }
}

/// @addr{0x806C1E90}
void ObjectFireSnake::calcHighBounce() {
    constexpr f32 INITIAL_VELOCITY = 1.6f * 60.0f;

    calcBounce(INITIAL_VELOCITY);
}

/// @addr{0x806C2138}
void ObjectFireSnake::calcRest() {
    if (m_currentFrame >= 30) {
        m_nextStateId = 4;
    }

    calcTransform();

    EGG::Vector3f tangent = Interpolate(0.3f, m_transform.base(2), m_bounceDir);
    tangent.normalise2();

    setMatrixTangentTo(EGG::Vector3f::ey, tangent);
}

/// @addr{0x806C2254}
void ObjectFireSnake::calcBounce() {
    constexpr f32 INITIAL_VELOCITY = 60.0f;

    calcBounce(INITIAL_VELOCITY);
}

/// @addr{0x806C2530}
void ObjectFireSnake::calcChildren() {
    // Shift all matrices up by one index
    for (size_t i = m_prevTransforms.size() - 1; i > 0; --i) {
        m_prevTransforms[i] = m_prevTransforms[i - 1];
    }

    calcTransform();
    m_prevTransforms[0] = m_transform;
    m_prevTransforms[0].setBase(3, m_pos);

    u32 idx = 10;
    for (auto *&kid : m_kids) {
        if (m_currentStateId == 1 && idx == m_currentFrame) {
            if (!kid->getUnit()) {
                kid->loadAABB(0.0f);
            }
        } else if (m_currentStateId == 0 && m_currentFrame == 0) {
            if (kid->getUnit()) {
                kid->unregisterCollision();
            }
        }

        if (m_currentStateId == 1 && m_currentFrame < idx) {
            calcTransform();
            kid->setTransform(m_transform);
        } else {
            kid->setTransform(m_prevTransforms[idx]);
        }

        idx += 10;
    }
}

/// @brief Helper function since high and regular bounce functions only vary by initial velocity.
void ObjectFireSnake::calcBounce(f32 initialVel) {
    // Collision checks only occur after 10 frames in the bounce state.
    constexpr u32 BOUNCE_COL_CHECK_DELAY = 10;
    constexpr f32 RADIUS = 130.0f;
    constexpr f32 BOUNCE_SPEED = 20.0f;

    m_visualPos.x += m_bounceDir.x * BOUNCE_SPEED;
    m_visualPos.z += m_bounceDir.z * BOUNCE_SPEED;
    m_visualPos.y += initialVel - GRAVITY * static_cast<f32>(m_currentFrame);

    CollisionInfo info;

    if (m_currentFrame > BOUNCE_COL_CHECK_DELAY) {
        bool hasCol = CollisionDirector::Instance()->checkSphereFull(RADIUS, m_visualPos,
                EGG::Vector3f::inf, KCL_TYPE_64EBDFFF, &info, nullptr, 0);

        if (hasCol) {
            m_visualPos += info.tangentOff;
            m_nextStateId = 3;
        }
    }

    m_pos = m_visualPos;
    m_flags.setBit(eFlags::Position);
}

} // namespace Field
