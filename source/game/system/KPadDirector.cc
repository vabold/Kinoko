#include "KPadDirector.hh"

namespace System {

void KPadDirector::calc() {
    calcPads();
    m_playerInput.calc();
}

void KPadDirector::calcPads() {
    m_ghostController->calc();
}

void KPadDirector::clear() {}

void KPadDirector::reset() {
    m_playerInput.reset();
}

void KPadDirector::startGhostProxies() {
    m_playerInput.startGhostProxy();
}

const KPadPlayer &KPadDirector::playerInput() const {
    return m_playerInput;
}

void KPadDirector::setGhostPad(const u8 *inputs, bool driftIsAuto) {
    m_playerInput.setGhostController(m_ghostController, inputs, driftIsAuto);
}

KPadDirector *KPadDirector::CreateInstance() {
    assert(!s_instance);
    return s_instance = new KPadDirector;
}

void KPadDirector::DestroyInstance() {
    assert(s_instance);
    delete s_instance;
    s_instance = nullptr;
}

KPadDirector *KPadDirector::Instance() {
    return s_instance;
}

KPadDirector::KPadDirector() {
    m_ghostController = new KPadGhostController;
}

KPadDirector::~KPadDirector() = default;

KPadDirector *KPadDirector::s_instance = nullptr;

} // namespace System
