#include "RaceConfig.hh"

#include <source/host/System.hh>

namespace System {

// TODO: read from parameter file
void RaceConfig::init() {}

RaceConfig *RaceConfig::CreateInstance() {
    assert(!s_instance);
    s_instance = new RaceConfig;
    return s_instance;
}

void RaceConfig::DestroyInstance() {
    assert(s_instance);
    delete s_instance;
    s_instance = nullptr;
}

RaceConfig *RaceConfig::Instance() {
    return s_instance;
}

RaceConfig::RaceConfig() = default;

RaceConfig::~RaceConfig() = default;

RaceConfig *RaceConfig::s_instance = nullptr;

} // namespace System
