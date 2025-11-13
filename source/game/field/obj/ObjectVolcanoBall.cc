#include "ObjectVolcanoBall.hh"

namespace Field {

/// @addr{0x806E2904}
ObjectVolcanoBall::ObjectVolcanoBall(f32 accel, f32 finalVel, f32 endPosY,
        const System::MapdataGeoObj &params, const EGG::Vector3f &vel)
    : ObjectCollidable(params), StateManager(this, STATE_ENTRIES),
      m_burnDuration(params.setting(3)), m_accel(accel), m_finalVel(finalVel), m_endPosY(endPosY),
      m_sqVelXZ(vel.x * vel.x + vel.z * vel.z) {}

/// @addr{0x806E2BE0}
ObjectVolcanoBall::~ObjectVolcanoBall() = default;

/// @addr{0x806E2C4C}
void ObjectVolcanoBall::init() {
    m_railInterpolator->init(0.0f, 0);
    m_pos = m_railInterpolator->curPos();
    m_flags.setBit(eFlags::Position);
}

/// @addr{0x806E3034}
void ObjectVolcanoBall::calcFalling() {
    f32 sqVel = std::max(0.01f, m_finalVel - 2.0f * m_accel * (m_pos.y - m_endPosY));
    m_railInterpolator->setCurrVel(EGG::Mathf::sqrt(m_sqVelXZ + sqVel));

    if (m_railInterpolator->calc() == RailInterpolator::Status::ChangingDirection) {
        m_nextStateId = 2;
    } else {
        EGG::Vector3f tangent = m_railInterpolator->curTangentDir();
        if (EGG::Mathf::abs(tangent.y) > 0.1f) {
            tangent.y = 0.01f;
        }

        tangent.normalise2();
        setMatrixFromOrthonormalBasisAndPos(tangent);

        m_pos = m_railInterpolator->curPos();
        m_flags.setBit(eFlags::Position);
    }
}

} // namespace Field
