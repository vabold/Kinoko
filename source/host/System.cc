#include "System.hh"

#include <source/game/system/ResourceManager.hh>

void HostSystem::create() {
    System::ResourceManager::CreateInstance();
}

void HostSystem::init() {
    System::ResourceManager::Instance()->load(0, nullptr);
}

void HostSystem::calc() {}

void HostSystem::DestroyInstance() {
    assert(s_instance);
    delete s_instance;
    s_instance = nullptr;
}

HostSystem *HostSystem::Instance() {
    return s_instance;
}

HostSystem *HostSystem::CreateInstance() {
    assert(!s_instance);
    return s_instance = new HostSystem();
}

HostSystem::HostSystem() = default;

HostSystem::~HostSystem() = default;

HostSystem *HostSystem::s_instance = nullptr;
