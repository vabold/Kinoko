#include "ObjectShip64.hh"

#include "game/field/ObjectCollisionCylinder.hh"
#include "game/field/RailInterpolator.hh"

namespace Kinoko::Field {

/// @addr{0x80765C94}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
ObjectShip64::ObjectShip64(const System::MapdataGeoObj &params) : ObjectCollidable(params) {}

/// @addr{0x80765DB0}
/// @brief Virtual destructor that also deletes the paddle wheel collision object
ObjectShip64::~ObjectShip64() {
    EGG::egg_delete(m_paddleWheelCollision);
}

/// @addr{0x80765E30}
/// @copybrief ObjectBase::init()
/// @details Initializes the rail interpolator to the beginning of the rail, sets @ref
/// m_tangent based on the rail interpolator's tangent direction, and enabled per-point velocities.
/// Calls @ref calc() to update the ship once. Lastly, calls @ref calcModel() to update the ship's
/// transformation matrix.
void ObjectShip64::init() {
    m_railInterpolator->init(0, 0);
    m_tangent = m_railInterpolator->curTangentDir();
    m_railInterpolator->setPerPointVelocities(true);
    calc();
    calcModel();
}

/// @addr{0x80766144}
/// @copybrief ObjectBase::calc()
/// @details Updates the rail interpolator and updates the ship's position accordingly.
/// Calls @ref calcTangent() to interpolate the forward direction to create smooth movement
/// along the rail.
void ObjectShip64::calc() {
    m_railInterpolator->calc();
    setPos(m_railInterpolator->curPos());
    calcTangent();
}

/// @addr{0x80766864}
/// @copybrief ObjectBase::createCollision()
/// @details Creates the primary and paddle wheel collision objects for the ship.
void ObjectShip64::createCollision() {
    constexpr f32 RADIUS = 1500.0f;
    constexpr f32 HEIGHT = 3500.0f;

    ObjectCollidable::createCollision();
    m_paddleWheelCollision =
            EGG::egg_new<ObjectCollisionCylinder>(RADIUS, HEIGHT, EGG::Vector3f::zero);
}

/// @addr{0x807668D4}
/// @copybrief ObjectBase::calcCollisionTransform()
/// @details Calculates the transformation matrices for the primary and paddle wheel collision
/// objects based on the ship's current orientation.
void ObjectShip64::calcCollisionTransform() {
    ObjectCollidable::calcCollisionTransform();
    calcTransform();
    EGG::Matrix34f mat = transform();

    EGG::Vector3f v;
    v = mat.base(0);
    v.normalise();

    mat.setAxisRotation(HALF_PI, v);
    mat.setBase(3, pos());
    m_paddleWheelCollision->transform(mat, scale());
}

/// @addr{0x80766CA4}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame
ObjectBase::LoadFlags ObjectShip64::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x80766BB4}
/// @copybrief ObjectBase::getResources()
/// @return The resource name for the ship, `DKShip64`
const char *ObjectShip64::getResources() const {
    return "DKShip64";
}

/// @addr{0x80766BC0}
/// @copybrief ObjectBase::getKclName()
/// @return The model name of the ship, `DKShip64`
const char *ObjectShip64::getKclName() const {
    return "DKShip64";
}

/// @addr{0x80766BCC}
/// @copydoc ObjectCollidable::checkCollision()
/// @details Checks for collision against both the ship and the paddle wheel, short-circuiting if
/// the player collided with the ship. Sets `dist` to reflect the distance between the player and
/// the colliding object.
bool ObjectShip64::checkCollision(ObjectCollisionBase *lhs, EGG::Vector3f &dist) {
    EGG::Vector3f colDist = EGG::Vector3f::zero;
    EGG::Vector3f auxDist = EGG::Vector3f::zero;

    bool has_col =
            lhs->check(*m_collision, colDist) || lhs->check(*m_paddleWheelCollision, auxDist);
    dist = colDist + auxDist;

    return has_col;
}

/// @addr{0x80766754}
/// @brief Calculates the smoothed forward direction of the ship along the rail path
/// @details Interpolates @ref m_tangent towards the rail interpolator's tangent direction with
/// an interpolation rate of `0.2f`. Updates the ship's transformation matrix accordingly.
void ObjectShip64::calcTangent() {
    constexpr f32 INTERP_RATE = 0.2f;

    m_tangent = m_tangent.interpolate(INTERP_RATE, m_railInterpolator->curTangentDir());
    m_tangent.normalise();
    setMatrixFromOrthonormalBasisAndPos(m_tangent);
}

} // namespace Kinoko::Field
