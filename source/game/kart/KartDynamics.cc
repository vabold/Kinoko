#include "KartDynamics.hh"

namespace Kart {

KartDynamics::KartDynamics() {
    init();
}

void KartDynamics::init() {
    m_pos.setZero();
    m_speed.setZero();
    m_fullRot = EGG::Quatf(1.0f, EGG::Vector3f::zero);
    m_top = EGG::Vector3f::ey;
}

const EGG::Vector3f &KartDynamics::getPos() const {
    return m_pos;
}

const EGG::Vector3f &KartDynamics::getSpeed() const {
    return m_speed;
}

const EGG::Vector3f &KartDynamics::getTop() const {
    return m_top;
}

const EGG::Quatf &KartDynamics::getFullRot() const {
    return m_fullRot;
}

void KartDynamics::setPos(const EGG::Vector3f &pos) {
    m_pos = pos;
}

void KartDynamics::setTop(const EGG::Vector3f &top) {
    m_top = top;
}

void KartDynamics::setMainRot(const EGG::Quatf &q) {
    m_mainRot = q;
}

void KartDynamics::setFullRot(const EGG::Quatf &q) {
    m_fullRot = q;
}

} // namespace Kart
