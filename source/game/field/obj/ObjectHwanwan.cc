#include "ObjectHwanwan.hh"

#include "game/field/CollisionDirector.hh"

namespace Field {

/// @addr{0x806E95B0}
ObjectHwanwan::ObjectHwanwan(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), StateManager(this, STATE_ENTRIES), m_initPos(m_pos) {}

/// @addr{0x806EC6E0}
ObjectHwanwan::~ObjectHwanwan() = default;

/// @addr{0x806E9724}
void ObjectHwanwan::init() {
    m_workPos = m_initPos + EGG::Vector3f::ey * DIAMETER;
    m_extVel.setZero();
    m_bounceVel.setZero();
    m_tangent = EGG::Vector3f::ez;
    m_up = EGG::Vector3f::ey;
    m_targetUp = EGG::Vector3f::ey;
    m_touchingGround = false;
    m_initJumpVel = 0.0f;
    m_targetY = m_workPos.y;

    calcTransform();

    m_nextStateId = 0;
}

/// @addr{0x806E9A78}
void ObjectHwanwan::calc() {
    StateManager::calc();

    m_extVel += m_bounceVel - GRAVITY;
    m_workPos += m_extVel;
    m_bounceVel.setZero();

    checkFloorCollision();

    SetRotTangentHorizontal(m_transform, m_up, m_tangent);
    m_transform.setBase(3, m_workPos);
    m_pos = m_workPos;
}

/// @addr{0x806EA784}
void ObjectHwanwan::checkFloorCollision() {
    constexpr f32 RADIUS = DIAMETER * 0.5f;

    m_touchingGround = false;

    CollisionInfo colInfo;
    KCLTypeMask mask;
    EGG::Vector3f pos = m_workPos + EGG::Vector3f(0.0f, -DIAMETER, 0.0f);

    bool hasCol = CollisionDirector::Instance()->checkSphereFullPush(RADIUS, pos,
            EGG::Vector3f::inf, KCL_TYPE_FLOOR, &colInfo, &mask, 0);

    if (!hasCol || pos.y - m_targetY >= 300.0f) {
        return;
    }

    m_touchingGround = true;

    f32 len = colInfo.tangentOff.length();
    m_workPos += EGG::Vector3f::ey * len;

    if (colInfo.floorDist > -std::numeric_limits<f32>::min()) {
        m_targetUp = colInfo.floorNrm;
    }
    m_extVel.y = 0.0f;
}

/// @addr{0x806EAAE8}
void ObjectHwanwan::calcUp() {
    m_up = Interpolate(0.1f, m_up, m_targetUp);
    if (m_up.squaredLength() > std::numeric_limits<f32>::epsilon()) {
        m_up.normalise2();
    } else {
        m_up = EGG::Vector3f::ey;
    }
}

/// @addr{0x806C5354}
ObjectHwanwanManager::ObjectHwanwanManager(const System::MapdataGeoObj &params)
    : ObjectCollidable(params) {
    m_hwanwan = new ObjectHwanwan(params);
    m_hwanwan->setScale(EGG::Vector3f(2.0f, 2.0f, 2.0f));
    m_hwanwan->load();
}

/// @addr{0x806C56DC}
ObjectHwanwanManager::~ObjectHwanwanManager() = default;

/// @addr{0x806C571C}
void ObjectHwanwanManager::init() {
    m_railInterpolator->init(0.0f, 0);
    m_hwanwan->m_tangent = m_railInterpolator->curTangentDir();
    const auto &curPos = m_railInterpolator->curPos();
    m_hwanwan->m_workPos.x = curPos.x;
    m_hwanwan->m_workPos.z = curPos.z;
    m_hwanwan->m_targetY = curPos.y;

    m_hwanwan->calc();
    m_hwanwan->calc();

    ASSERT(m_mapObj);
    m_railInterpolator->setCurrVel(static_cast<f32>(m_mapObj->setting(0)));
}

/// @addr{0x806C5AC4}
void ObjectHwanwanManager::calc() {
    calcState();

    const auto &curPos = m_railInterpolator->curPos();
    m_hwanwan->m_workPos.x = curPos.x;
    m_hwanwan->m_workPos.z = curPos.z;
    m_hwanwan->m_targetY = curPos.y;
    m_hwanwan->m_tangent = m_railInterpolator->curTangentDir();
}

/// @addr{0x806C5DE0}
void ObjectHwanwanManager::calcState() {
    if (m_railInterpolator->calc() == RailInterpolator::Status::SegmentEnd &&
            m_railInterpolator->curPoint().setting[1] == 1 && m_hwanwan->m_currentStateId != 2) {
        m_hwanwan->m_nextStateId = 1;
    }

    if (m_hwanwan->m_currentStateId == 1 && m_hwanwan->m_currentFrame >= 60) {
        m_hwanwan->m_nextStateId = 0;
    }
}

} // namespace Field
