#include "KartObjectProxy.hh"

#include "game/kart/KartObject.hh"
#include "game/kart/KartSub.hh"

namespace Kart {

KartObjectProxy::KartObjectProxy() : m_accessor(nullptr) {
    if (s_list) {
        s_list->append(this);
    }
}

KartObjectProxy::~KartObjectProxy() = default;

KartBody *KartObjectProxy::body() {
    return m_accessor->m_body;
}

const KartBody *KartObjectProxy::body() const {
    return m_accessor->m_body;
}

KartMove *KartObjectProxy::move() {
    return m_accessor->m_move;
}

const KartMove *KartObjectProxy::move() const {
    return m_accessor->m_move;
}

KartParam *KartObjectProxy::param() {
    return m_accessor->m_param;
}

const KartParam *KartObjectProxy::param() const {
    return m_accessor->m_param;
}

const BSP &KartObjectProxy::bsp() const {
    return param()->bsp();
}

KartPhysics *KartObjectProxy::physics() {
    return m_accessor->m_body->getPhysics();
}

const KartPhysics *KartObjectProxy::physics() const {
    return m_accessor->m_body->getPhysics();
}

KartDynamics *KartObjectProxy::dynamics() {
    return physics()->getDynamics();
}

const KartDynamics *KartObjectProxy::dynamics() const {
    return physics()->getDynamics();
}

KartState *KartObjectProxy::state() {
    return m_accessor->m_state;
}

const KartState *KartObjectProxy::state() const {
    return m_accessor->m_state;
}

KartSub *KartObjectProxy::sub() {
    return m_accessor->m_sub;
}

const KartSub *KartObjectProxy::sub() const {
    return m_accessor->m_sub;
}

const EGG::Vector3f &KartObjectProxy::scale() const {
    return m_accessor->m_move->scale();
}

void KartObjectProxy::setPos(const EGG::Vector3f &pos) {
    dynamics()->setPos(pos);
}

void KartObjectProxy::setRot(const EGG::Quatf &q) {
    dynamics()->setFullRot(q);
    dynamics()->setMainRot(q);
}

const EGG::Vector3f &KartObjectProxy::pos() const {
    return dynamics()->pos();
}

const EGG::Quatf &KartObjectProxy::fullRot() const {
    return dynamics()->fullRot();
}

Abstract::List *KartObjectProxy::list() const {
    return s_list;
}

void KartObjectProxy::ApplyAll(KartAccessor *pointers) {
    for (Abstract::Node *node = s_list->head(); node; node = s_list->getNext(node)) {
        node->data<KartObjectProxy>()->m_accessor = pointers;
    }
}

Abstract::List *KartObjectProxy::s_list = nullptr;

} // namespace Kart
