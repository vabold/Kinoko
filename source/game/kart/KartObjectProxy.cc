#include "KartObjectProxy.hh"

#include "game/kart/KartObject.hh"

namespace Kart {

KartObjectProxy::KartObjectProxy() : m_accessor(nullptr) {
    if (s_list) {
        s_list->append(this);
    }
}

KartObjectProxy::~KartObjectProxy() = default;

Abstract::List *KartObjectProxy::list() const {
    return s_list;
}

void KartObjectProxy::ApplyAll(KartAccessor &pointers) {
    for (Abstract::Node *node = s_list->head(); node; node = s_list->getNext(node)) {
        node->data<KartObjectProxy>()->m_accessor = &pointers;
    }
}

Abstract::List *KartObjectProxy::s_list = nullptr;

} // namespace Kart
