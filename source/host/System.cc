#include "System.hh"

namespace Host {

void System::init() {}

void System::DestroyInstance() {
    assert(s_instance);
    delete s_instance;
    s_instance = nullptr;
}

System *System::Instance() {
    return s_instance;
}

System *System::CreateInstance() {
    assert(!s_instance);
    return s_instance = new System();
}

System::System() = default;

System::~System() = default;

System *System::s_instance = nullptr;

} // namespace Host
