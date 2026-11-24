#include "ObjectSanbo.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/CollisionInfo.hh"
#include "game/field/RailInterpolator.hh"

namespace Kinoko::Field {

/// @addr{0x80779F3C}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Sets @ref m_yVel to zero.
ObjectSanbo::ObjectSanbo(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_initForward(calcInitForward()) {
    m_yVel = 0.0f;
}

/// @addr{0x8077A1A8}
/// @brief Default virtual destructor
ObjectSanbo::~ObjectSanbo() = default;

/// @addr{0x8077A1E8}
/// @copybrief ObjectBase::init()
/// @details Initializes @ref m_up to the default up vector. Computes the forward direction of the
/// Pokey based on its initial rotation. Sets @ref m_standstill to false. Finally, initializes the
/// rail interpolator to the start of the rail.
void ObjectSanbo::init() {
    m_up = EGG::Vector3f::ey;
    m_standstill = false;
    m_railInterpolator->init(0.0f, 0);
}

/// @addr{0x8077A36C}
/// @copybrief ObjectBase::calc()
/// @details Simply calls @ref calcMove() to update the Pokey's position.
void ObjectSanbo::calc() {
    calcMove();
}

/// @addr{0x8077BD38}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectSanbo::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @brief Helper function to initialize @ref m_initForward
/// @return The normalized forward direction of the Pokey based on its rotation matrix
EGG::Vector3f ObjectSanbo::calcInitForward() const {
    EGG::Matrix34f rotMat;
    rotMat.makeR(rot());
    EGG::Vector3f forward = rotMat.base(2);
    forward.normalise();
    return forward;
}

/// @addr{0x8077A5F8}
/// @brief Runs every frame to update the Pokey's position
/// @details If the Pokey is at a standstill, decrements @ref m_standstillTimer until it reaches
/// zero, at which point the Pokey will start walking again. If the Pokey is moving, then it updates
/// the Pokey's position along the rail, applies a downward gravitational force of `2.0f`, and
/// checks for floor collision. If the Pokey reached the end of the rail, then sets @ref
/// m_standstill to `true` and sets @ref m_standstillTimer based on the current rail point's first
/// setting.
void ObjectSanbo::calcMove() {
    constexpr f32 GRAVITY = 2.0f;

    if (m_standstill) {
        if (--m_stillTimer == 0) {
            m_standstill = false;
        }

        return;
    }

    auto railStatus = m_railInterpolator->calc();
    if (railStatus == RailInterpolator::Status::ChangingDirection) {
        m_standstill = true;
        m_stillTimer = m_railInterpolator->curPoint().setting[0];
    }

    const EGG::Vector3f &railPos = m_railInterpolator->curPos();
    m_yVel -= GRAVITY;
    setPos(EGG::Vector3f(railPos.x, m_yVel + pos().y, railPos.z));

    checkSphere();
}

/// @addr{0x8077A8B8}
/// @brief Handles collision between the Pokey and the floor (including sandcones)
/// @details If a collision with a floor occurs, resets @ref m_yVel to zero, updates the Pokey's
/// position so that it sits on top of the floor, and nudges @ref m_up towards the colliding floor's
/// normal vector. Finally, updates the Pokey's transformation matrix to reflect the current @ref
/// m_up and @ref m_initForward.
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

    setMatrixTangentTo(m_up, m_initForward);
}

} // namespace Kinoko::Field
