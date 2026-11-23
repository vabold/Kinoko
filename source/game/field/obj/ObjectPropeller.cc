#include "ObjectPropeller.hh"

#include "game/field/ObjectCollisionCylinder.hh"
#include "game/field/ObjectDirector.hh"

#include "game/system/map/MapdataGeoObj.hh"

namespace Kinoko::Field {

/// @addr{0x80764CC8}
/// @copybrief ObjectCollidable::ObjectCollidable(const System::MapdataGeoObj &)
/// @param params The parameters used to initialize the object
/// @details Initializes @ref m_angle to zero and sets all blade collision pointers to
/// `nullptr`.
ObjectPropeller::ObjectPropeller(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_angle(0.0f) {
    m_blades.fill(nullptr);
}

/// @addr{0x80764E34}
/// @brief Virtual destructor that destroys each of the propeller blades' collision object
ObjectPropeller::~ObjectPropeller() {
    for (auto *&blade : m_blades) {
        EGG::egg_delete(blade);
    }
}

/// @addr{0x80764EB4}
/// @copybrief ObjectBase::init()
/// @details Initializes the propeller's angular velocity and initial transformation matrix.
/// Stores the propeller's axis of rotation to @ref m_axis based on @ref m_initMat.
/// @note This function normally calls @ref EGG::Matrix34f::makeR() and then manually sets the
/// position, but we can optimize slightly by calling @ref EGG::Matrix34f::makeRT() instead.
void ObjectPropeller::init() {
    initAngVel();
    m_initMat.makeRT(rot(), pos());
    m_axis = m_initMat.base(2);
}

/// @addr{0x80765068}
/// @copybrief ObjectBase::calc()
/// @details Dispatches to @ref calcAngleAndRot() to update the propeller's rotation angle and
/// transformation matrix.
void ObjectPropeller::calc() {
    calcAngleAndRot();
}

/// @addr{0x807655B4}
/// @copybrief ObjectBase::createCollision()
/// @details Creates cylindrical collision for the shell and each of the 3 blades.
void ObjectPropeller::createCollision() {
    ObjectCollidable::createCollision();

    const auto &colCenter = collisionCenter();
    const auto &flowTable = ObjectDirector::Instance()->flowTable();
    const auto &params = flowTable.set(flowTable.slot(id()))->params.cylinder;
    f32 radius = static_cast<f32>(parse<s16>(params.radius));
    f32 height = static_cast<f32>(parse<s16>(params.height));

    for (auto *&blade : m_blades) {
        blade = EGG::egg_new<ObjectCollisionCylinder>(radius, height, colCenter);
    }
}

/// @addr{0x80765738}
/// @copybrief ObjectBase::calcCollisionTransform()
/// @details Rotates the transformation matrix of each blade around the propeller's center point,
/// such that the blades are spaced evenly at 120-degree intervals.
void ObjectPropeller::calcCollisionTransform() {
    constexpr f32 BLADE_LENGTH = 250.0f;

    for (u32 i = 0; i < m_blades.size(); ++i) {
        EGG::Matrix34f mat;
        mat.setBase(3, EGG::Vector3f::zero);
        mat.setAxisRotation(static_cast<f32>(i * 120) * DEG2RAD, m_axis);
        calcTransform();
        mat = mat.multiplyTo(transform());
        EGG::Vector3f dir = mat.base(1);
        dir.normalise();
        dir *= BLADE_LENGTH;
        mat.setBase(3, pos() + dir);

        auto *&blade = m_blades[i];
        blade->transform(mat, scale());
    }
}

/// @addr{0x80765BC0}
/// @copybrief ObjectBase::loadFlags()
/// @return Returns @ref eLoadFlags::Calc, so that the object is calculated every frame.
ObjectBase::LoadFlags ObjectPropeller::loadFlags() const {
    return LoadFlags(eLoadFlags::Calc);
}

/// @addr{0x80765930}
/// @copybrief ObjectBase::getCollisionRadius()
/// @return The collision radius of the propeller, calculated as `5` times the larger of the X and Z
/// components of its scaled bounding box.
f32 ObjectPropeller::getCollisionRadius() const {
    const auto &flowTable = ObjectDirector::Instance()->flowTable();
    const auto &params = flowTable.set(flowTable.slot(id()))->params.box;
    f32 z = scale().z * static_cast<f32>(parse<s16>(params.z));
    f32 x = scale().x * static_cast<f32>(parse<s16>(params.x));

    return 5.0f * std::max(z, x);
}

/// @addr{0x80765A54}
/// @copybrief ObjectCollidable::checkCollision()
/// @param lhs The object to check collision against (usually the player)
/// @param dist On collision, set to the distance between the player and the colliding blade
/// @return Whether or not a collision occurred
/// @details Checks collision against each of the 3 blades, short-circuiting if a collision is
/// detected. Sets `dist` to the distance between the player and the colliding blade, or zero if no
/// collision occurred.
bool ObjectPropeller::checkCollision(ObjectCollisionBase *lhs, EGG::Vector3f &dist) {
    EGG::Vector3f dist0 = EGG::Vector3f::zero;
    EGG::Vector3f dist1 = EGG::Vector3f::zero;
    EGG::Vector3f dist2 = EGG::Vector3f::zero;

    bool hasCol = lhs->check(*m_blades[0], dist0);
    hasCol = hasCol || lhs->check(*m_blades[1], dist1);
    hasCol = hasCol || lhs->check(*m_blades[2], dist2);

    dist = dist0 + dist1 + dist2;

    return hasCol;
}

/// @addr{0x80765558}
/// @brief Initializes the propeller's angular velocity
/// @details @ref m_angVel is set based on param setting 1, and its direction is flipped if
/// param setting 2 is set to `1`.
void ObjectPropeller::initAngVel() {
    ASSERT(m_mapObj);
    m_angVel = static_cast<f32>(static_cast<s16>(m_mapObj->setting(0)));
    if (m_mapObj->setting(1) == 1) {
        m_angVel = -m_angVel;
    }
}

/// @addr{0x80765190}
/// @brief Calculates the propeller's rotation angle and updates its transformation matrix
/// @details @ref m_angle increments by half of @ref m_angVel. @ref m_curRot is computed by rotating
/// around @ref m_axis by @ref m_angle degrees. Finally, sets the transformation matrix to reflect
/// the updated rotation.
void ObjectPropeller::calcAngleAndRot() {
    m_angle += m_angVel * 0.5f;
    m_curRot = EGG::Matrix34f::ident;
    m_curRot.setAxisRotation(m_angle * DEG2RAD, m_axis);
    EGG::Matrix34f transform = m_curRot.multiplyTo(m_initMat);
    transform.setBase(3, pos());
    setTransform(transform);
}

} // namespace Kinoko::Field
