#include "KPadDirector.hh"

namespace System {

/// @addr{0x805238F0}
void KPadDirector::calc() {
    calcPads();
    m_playerInput.calc();
}

/// @addr{0x805237E8}
void KPadDirector::calcPads() {
    m_ghostController->calc();
}

/// @addr{0x80523724}
void KPadDirector::clear() {}

/// @addr{0x80523690}
void KPadDirector::reset() {
    m_playerInput.reset();
}

/// @addr{0x80524580}
void KPadDirector::startGhostProxies() {
    m_playerInput.startGhostProxy();
}

/// @addr{0x805245DC}
void KPadDirector::endGhostProxies() {
    m_playerInput.endGhostProxy();
}

const KPadPlayer &KPadDirector::playerInput() const {
    return m_playerInput;
}

/// @addr{0x8052453C}
void KPadDirector::setGhostPad(const u8 *inputs, bool driftIsAuto) {
    m_playerInput.setGhostController(m_ghostController, inputs, driftIsAuto);
}

/// @addr{0x8052313C}
KPadDirector *KPadDirector::CreateInstance() {
    ASSERT(!s_instance);
    return s_instance = new KPadDirector;
}

/// @addr{0x8052318C}
void KPadDirector::DestroyInstance() {
    ASSERT(s_instance);
    auto *instance = s_instance;
    s_instance = nullptr;
    delete instance;
}

KPadDirector *KPadDirector::Instance() {
    return s_instance;
}

/// @addr{0x805232F0}
KPadDirector::KPadDirector() {
    m_ghostController = new KPadGhostController;
}

/// @addr{0x805231DC}
KPadDirector::~KPadDirector() {
    if (s_instance) {
        s_instance = nullptr;
        WARN("KPadDirector instance not explicitly handled!");
    }
}

KPadDirector *KPadDirector::s_instance = nullptr; ///< @addr{0x809BD70C}

} // namespace System
