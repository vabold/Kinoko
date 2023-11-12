#include "InputManager.hh"

namespace System {

void InputManager::calc() {}

void InputManager::clear() {}

void InputManager::init() {}

void InputManager::setGhostPad() {}

InputManager *InputManager::CreateInstance() {
    assert(!s_instance);
    return s_instance = new InputManager;
}

void InputManager::DestroyInstance() {
    assert(s_instance);
    delete s_instance;
    s_instance = nullptr;
}

InputManager *InputManager::Instance() {
    return s_instance;
}

InputManager::InputManager() = default;

InputManager::~InputManager() = default;

InputManager *InputManager::s_instance = nullptr;

} // namespace System
