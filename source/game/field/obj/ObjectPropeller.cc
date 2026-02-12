#include "ObjectPropeller.hh"

#include "game/field/ObjectDirector.hh"

namespace Field {

/// @addr{0x80764CC8}
ObjectPropeller::ObjectPropeller(const System::MapdataGeoObj &params)
    : ObjectCollidable(params), m_angle(0.0f) {
    m_blades.fill(nullptr);
}

/// @addr{0x80764E34}
ObjectPropeller::~ObjectPropeller() {
    for (auto *&blade : m_blades) {
        delete blade;
    }
}

/// @addr{0x80764EB4}
void ObjectPropeller::init() {
    ASSERT(m_mapObj);
    m_angVel = static_cast<f32>(static_cast<s16>(m_mapObj->setting(0)));
    if (m_mapObj->setting(1) == 1) {
        m_angVel = -m_angVel;
    }

    m_rotMat.makeR(rot());
    m_rotMat.setBase(3, pos());
    m_axis = m_rotMat.base(2);
}

/// @addr{0x80765068}
void ObjectPropeller::calc() {
    m_angle += m_angVel * 0.5f;
    m_curRot = EGG::Matrix34f::ident;
    m_curRot.setAxisRotation(m_angle * DEG2RAD, m_axis);
    EGG::Matrix34f transform = m_curRot.multiplyTo(m_rotMat);
    transform.setBase(3, pos());
    setTransform(transform);
}

/// @addr{0x807655B4}
void ObjectPropeller::createCollision() {
    ObjectCollidable::createCollision();

    const auto &colCenter = collisionCenter();
    const auto &flowTable = ObjectDirector::Instance()->flowTable();
    const auto &params = flowTable.set(flowTable.slot(id()))->params.cylinder;
    f32 radius = static_cast<f32>(parse<s16>(params.radius));
    f32 height = static_cast<f32>(parse<s16>(params.height));

    for (auto *&blade : m_blades) {
        blade = new ObjectCollisionCylinder(radius, height, colCenter);
    }
}

/// @addr{0x80765738}
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

/// @addr{0x80765930}
f32 ObjectPropeller::getCollisionRadius() const {
    const auto &flowTable = ObjectDirector::Instance()->flowTable();
    const auto &params = flowTable.set(flowTable.slot(id()))->params.box;
    f32 z = scale().z * static_cast<f32>(parse<s16>(params.z));
    f32 x = scale().x * static_cast<f32>(parse<s16>(params.x));

    return 5.0f * std::max(z, x);
}

/// @addr{0x80765A54}
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

} // namespace Field
