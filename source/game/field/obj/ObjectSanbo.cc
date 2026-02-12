#include "ObjectSanbo.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/KColData.hh"

namespace Field {

/// @addr{0x80779F3C}
ObjectSanbo::ObjectSanbo(const System::MapdataGeoObj &params) : ObjectCollidable(params) {
    m_yVel = 0.0f;
}

/// @addr{0x8077A1A8}
ObjectSanbo::~ObjectSanbo() = default;

/// @addr{0x8077A1E8}
void ObjectSanbo::init() {
    m_up = EGG::Vector3f::ey;
    EGG::Matrix34f rotMat;
    rotMat.makeR(rot());
    m_tangent = rotMat.base(2);
    m_tangent.normalise();
    m_standstill = false;
    m_railInterpolator->init(0.0f, 0);
}

/// @addr{0x8077A36C}
void ObjectSanbo::calc() {
    calcMove();
}

/// @addr{0x8077A5F8}
void ObjectSanbo::calcMove() {
    constexpr f32 GRAVITY = 2.0f;

    if (m_standstill) {
        if (--m_stillDuration == 0) {
            m_standstill = false;
        }

        return;
    }

    auto railStatus = m_railInterpolator->calc();
    if (railStatus == RailInterpolator::Status::ChangingDirection) {
        m_standstill = true;
        m_stillDuration = m_railInterpolator->curPoint().setting[0];
    }

    const EGG::Vector3f &railPos = m_railInterpolator->curPos();
    m_yVel -= GRAVITY;
    setPos(EGG::Vector3f(railPos.x, m_yVel + pos().y, railPos.z));

    checkSphere();
}

/// @addr{0x8077A8B8}
/// @brief Handles collision between the pokie and the floor (including sandcones).
void ObjectSanbo::checkSphere() {
    constexpr f32 RADIUS = 10.0f;
    constexpr EGG::Vector3f POS_OFFSET = EGG::Vector3f(0.0f, RADIUS, 0.0f);

    CollisionInfo colInfo;
    colInfo.bbox.setZero();

    EGG::Vector3f norm = m_up;

    if (CollisionDirector::Instance()->checkSphereFull(RADIUS, pos() + POS_OFFSET,
                EGG::Vector3f::inf, KCL_TYPE_FLOOR, &colInfo, nullptr, 0)) {
        m_yVel = 0.0f;
        addPos(colInfo.tangentOff);
        norm = colInfo.floorNrm;
    }

    EGG::Vector3f upNrm = m_up;
    upNrm.normalise();
    norm.normalise();

    m_up += (norm - upNrm).multInv(60.0f);
    m_up.normalise();

    setMatrixTangentTo(m_up, m_tangent);
}

} // namespace Field
