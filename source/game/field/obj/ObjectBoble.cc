#include "ObjectBoble.hh"

#include "game/field/RailInterpolator.hh"

namespace Kinoko::Field {

/// @addr{0x8075DB3C}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
ObjectBoble::ObjectBoble(const System::MapdataGeoObj &params) : ObjectCollidable(params) {}

/// @addr{0x8075E74C}
/// @brief Default virtual destructor
ObjectBoble::~ObjectBoble() = default;

/// @addr{0x8075DBA0}
/// @copybrief ObjectBase::init()
/// @details Initializes the rail interpolator's position, enables per-point velocities, and sets
/// the initial tangent direction and scale.
void ObjectBoble::init() {
    m_railInterpolator->init(0.0f, 0);
    m_curTangentDir = m_railInterpolator->curTangentDir();
    m_railInterpolator->setPerPointVelocities(true);
    setScale(EGG::Vector3f::unit);
}

/// @addr{0x8075DCA0}
/// @copybrief ObjectBase::calc()
/// @details Updates the rail interpolator, sets the object's position based on the current rail
/// position, and recalculates the tangent direction.
void ObjectBoble::calc() {
    m_railInterpolator->calc();
    setPos(m_railInterpolator->curPos());
    calcTangent();
}

/// @addr{0x8075E744}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectBoble::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x8075E070}
/// @brief Calculates the tangent direction of the object based on the rail interpolator
/// @details Interpolates the rail's tangent direction and updates the object's transformation
/// matrix accordingly.
void ObjectBoble::calcTangent() {
    constexpr f32 INTERP_RATE = 0.2f;

    m_curTangentDir = m_curTangentDir.interpolate(INTERP_RATE, m_railInterpolator->curTangentDir());
    m_curTangentDir.normalise();

    EGG::Vector3f axis = m_curTangentDir.cross(EGG::Vector3f::ex);
    if (axis.normalise() == 0.0f) {
        axis = m_curTangentDir.cross(EGG::Vector3f::ez);
        axis.normalise();
    }

    setMatrixTangentTo(axis.cross(m_curTangentDir), m_curTangentDir);
}

} // namespace Kinoko::Field
