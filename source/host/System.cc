#include "System.hh"

#include <source/game/kart/KartParam.hh>
#include <source/game/system/RaceConfig.hh>
#include <source/game/system/ResourceManager.hh>

// Empty for now
void HostSystem::initControllers() {}

void HostSystem::create() {
    System::RaceConfig::CreateInstance();
    System::ResourceManager::CreateInstance();

    Kart::KartParam::CreateInstance();
}

void HostSystem::init() {
    for (s8 i = 0; i < m_controllerCount; i++) {
        m_vehicles[i] = Vehicle::Dolphin_Dasher;
        m_characters[i] = Character::Bowser_Jr;
    }

    System::RaceConfig::Instance()->init();
    System::ResourceManager::Instance()->load(0, nullptr);

    Kart::KartParam::Instance()->init();
}

void HostSystem::calc() {}

s8 HostSystem::controllerCount() const {
    return m_controllerCount;
}

s8 HostSystem::ghostCount() const {
    return m_ghostCount;
}

Character HostSystem::character(s8 controllerId) const {
    return m_characters[controllerId];
}

Vehicle HostSystem::vehicle(s8 controllerId) const {
    return m_vehicles[controllerId];
}

Course HostSystem::course() const {
    return m_course;
}

void HostSystem::DestroyInstance() {
    assert(s_instance);
    delete s_instance;
    s_instance = nullptr;
}

HostSystem *HostSystem::Instance() {
    return s_instance;
}

HostSystem *HostSystem::CreateInstance(s8 controllerCount, s8 ghostCount) {
    assert(!s_instance);
    if (controllerCount > MAX_PLAYER_COUNT) {
        K_PANIC("More controllers than player slots!");
    }
    return s_instance = new HostSystem(controllerCount, ghostCount);
}

HostSystem::HostSystem(s8 controllerCount, s8 ghostCount)
    : m_course(Course::SNES_Mario_Circuit_3), m_controllerCount(controllerCount),
      m_ghostCount(ghostCount) {
    m_vehicles = new Vehicle[controllerCount];
    m_characters = new Character[controllerCount];
}

HostSystem::~HostSystem() = default;

HostSystem *HostSystem::s_instance = nullptr;
