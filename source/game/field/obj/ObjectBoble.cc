#include "ObjectBoble.hh"

namespace Field {

/// @addr{0x8075DB3C}
ObjectBoble::ObjectBoble(const System::MapdataGeoObj &params) : ObjectCollidable(params) {}

/// @addr{0x8075E74C}
ObjectBoble::~ObjectBoble() = default;

/// @addr{0x8075DBA0}
void ObjectBoble::init() {
    m_railInterpolator->init(0.0f, 0);
    m_curTangentDir = m_railInterpolator->curTangentDir();
    m_railInterpolator->setPerPointVelocities(true);
    setScale(EGG::Vector3f::unit);
}

/// @addr{0x8075DCA0}
void ObjectBoble::calc() {
    m_railInterpolator->calc();
    setPos(m_railInterpolator->curPos());
    calcTangent();
}

/// @addr{0x8075E070}
void ObjectBoble::calcTangent() {
    m_curTangentDir = Interpolate(0.2f, m_curTangentDir, m_railInterpolator->curTangentDir());
    m_curTangentDir.normalise();

    EGG::Vector3f axis = m_curTangentDir.cross(EGG::Vector3f::ex);
    if (axis.normalise() == 0.0f) {
        axis = m_curTangentDir.cross(EGG::Vector3f::ez);
        axis.normalise();
    }

    setMatrixTangentTo(axis.cross(m_curTangentDir), m_curTangentDir);
}

} // namespace Field
