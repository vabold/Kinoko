#include "ObjectShip64.hh"

namespace Field {

/// @addr{0x80765C94}
ObjectShip64::ObjectShip64(const System::MapdataGeoObj &params) : ObjectCollidable(params) {}

/// @addr{0x80765DB0}
ObjectShip64::~ObjectShip64() {
    delete m_auxCollision;
}

/// @addr{0x80765E30}
void ObjectShip64::init() {
    m_railInterpolator->init(0, 0);

    m_tangent = m_railInterpolator->curTangentDir();
    m_railInterpolator->setPerPointVelocities(true);

    calc();
    calcModel();
}

/// @addr{0x80766144}
void ObjectShip64::calc() {
    m_railInterpolator->calc();

    setPos(m_railInterpolator->curPos());

    m_tangent = Interpolate(0.2f, m_tangent, m_railInterpolator->curTangentDir());
    m_tangent.normalise();

    setMatrixFromOrthonormalBasisAndPos(m_tangent);
}

/// @addr{0x80766864}
void ObjectShip64::createCollision() {
    constexpr f32 RADIUS = 1500.0f;
    constexpr f32 HEIGHT = 3500.0f;

    ObjectCollidable::createCollision();
    m_auxCollision = new ObjectCollisionCylinder(RADIUS, HEIGHT, EGG::Vector3f::zero);
}

/// @addr{0x807668D4}
void ObjectShip64::calcCollisionTransform() {
    ObjectCollidable::calcCollisionTransform();
    calcTransform();
    EGG::Matrix34f mat = transform();

    EGG::Vector3f v;
    v = mat.base(0);
    v.normalise();

    mat.setAxisRotation(F_PI / 2.0f, v);
    mat.setBase(3, v);
    m_auxCollision->transform(mat, scale());
}

/// @addr{0x80766BCC}
bool ObjectShip64::checkCollision(ObjectCollisionBase *lhs, EGG::Vector3f &dist) {
    EGG::Vector3f colDist = EGG::Vector3f::zero;
    EGG::Vector3f auxDist = EGG::Vector3f::zero;

    bool has_col = lhs->check(*m_collision, colDist) || lhs->check(*m_auxCollision, auxDist);
    dist = colDist + auxDist;

    return has_col;
}

} // namespace Field
