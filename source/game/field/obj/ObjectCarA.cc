#include "ObjectCarA.hh"

#include "game/field/ObjectCollisionCylinder.hh"

#include "game/kart/KartCollide.hh"
#include "game/kart/KartObject.hh"

namespace Field {

/// @addr{0x806B7710}
ObjectCarA::ObjectCarA(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), StateManager(this, STATE_ENTRIES),
      m_finalVel(static_cast<f32>(params.setting(0))),
      m_accel(static_cast<f32>(params.setting(1)) / 10.0f),
      m_stopTime(static_cast<s32>(params.setting(2))) {}

/// @addr{0x806B78CC}
ObjectCarA::~ObjectCarA() = default;

/// @addr{0x806B7CE0}
void ObjectCarA::init() {
    constexpr f32 RADIUS = 400.0f;

    m_railInterpolator->init(0, 0);

    // Time to reach final velocity
    f32 finalTime = m_finalVel / m_accel;
    m_cruiseTime =
            (m_railInterpolator->railLength() - finalTime * m_accel * finalTime) / m_finalVel;

    setMatrixFromOrthonormalBasisAndPos(m_railInterpolator->curTangentDir());

    m_flags.setBit(eFlags::Position);
    m_pos = m_railInterpolator->curPos();
    m_currVel = 0.0f;
    m_motionState = MotionState::Accelerating;
    m_changingDir = false;
    m_currUp = EGG::Vector3f::ey;
    m_currTangent = m_railInterpolator->curTangentDir();
    resize(RADIUS, 0.0f);
    m_nextStateId = 0;
}

/// @addr{0x806B82CC}
void ObjectCarA::calc() {
    StateManager::calc();

    m_railInterpolator->setCurrVel(m_currVel);

    auto status = m_railInterpolator->calc();
    m_changingDir = (status == RailInterpolator::Status::ChangingDirection);

    calcPos();
}

/// @addr{0x806B7B44}
void ObjectCarA::createCollision() {
    constexpr f32 RADIUS = 210.0f;
    constexpr f32 HEIGHT = 200.0f;

    m_collision = new ObjectCollisionCylinder(RADIUS, HEIGHT, collisionCenter());
}

/// @addr{0x806B7BC4}
void ObjectCarA::calcCollisionTransform() {
    ObjectCollisionBase *objCol = collision();
    if (!objCol) {
        return;
    }

    calcTransform();

    EGG::Matrix34f mat;
    SetRotTangentHorizontal(mat, m_transform.base(2), EGG::Vector3f::ey);
    mat.setBase(3, m_transform.base(3) + 50.0f * m_transform.base(1));

    objCol->transform(mat, m_scale,
            -m_railInterpolator->curTangentDir() * m_railInterpolator->getCurrVel());
}

/// @addr{0x806B7E60}
Kart::Reaction ObjectCarA::onCollision(Kart::KartObject *kartObj, Kart::Reaction reactionOnKart,
        Kart::Reaction /*reactionOnObj*/, EGG::Vector3f & /*hitDepth*/) {
    return kartObj->speedRatioCapped() < 0.5f ? Kart::Reaction::WallAllSpeed : reactionOnKart;
}

/// @addr{0x806B84FC}
void ObjectCarA::enterStop() {
    m_currVel = 0.0f;
}

/// @addr{0x806B8838}
void ObjectCarA::enterCruising() {
    m_currVel = m_finalVel;
}

/// @addr{0x806B8588}
void ObjectCarA::calcStop() {
    if (m_currentFrame > static_cast<u32>(m_stopTime)) {
        m_motionState = MotionState::Accelerating;
        m_currentStateId = 1;
    }
}

/// @addr{0x806B86F0}
void ObjectCarA::calcAccel() {
    if (m_motionState == MotionState::Accelerating) {
        m_currVel += m_accel;

        if (m_currVel > m_finalVel) {
            m_currVel = m_finalVel;
            m_motionState = MotionState::Cruising;
            m_nextStateId = 2;
        }
    } else if (m_motionState == MotionState::Decelerating) {
        // The cruising time might've had decimals and needed to end early.
        // If we undershot it, keep a velocity of 0.01f so we can reach the end.
        m_currVel = std::max(0.01f, m_currVel - m_accel);

        if (m_changingDir) {
            m_currVel = 0.0f;

            if (m_railInterpolator->isMovementDirectionForward()) {
                m_railInterpolator->init(0.0f, 0);
            }

            m_motionState = MotionState::Cruising;
            m_nextStateId = 0;
        }
    }
}

/// @addr{0x806B8844}
void ObjectCarA::calcCruising() {
    // We might've had decimals, better to undershoot the cruising time and handle it in decel
    if (static_cast<f32>(m_currentFrame) > m_cruiseTime - 1.0f) {
        m_motionState = MotionState::Decelerating;
        m_nextStateId = 1;
    }
}

/// @addr{0x806B8D3C}
void ObjectCarA::calcPos() {
    m_currUp = Interpolate(0.1f, m_currUp, EGG::Vector3f::ey);
    m_currUp.normalise2();

    setMatrixTangentTo(m_currUp, m_currTangent);

    m_pos = m_railInterpolator->curPos();
    m_flags.setBit(eFlags::Position);
}

} // namespace Field
