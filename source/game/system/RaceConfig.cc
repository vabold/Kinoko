#include "RaceConfig.hh"

#include <source/host/System.hh>

namespace System {

void RaceConfig::init() {
    m_raceScenario.init();
}

void RaceConfig::setGhost() {
    // TODO: Relies on ghost file implementation
}

RaceConfig::Scenario &RaceConfig::raceScenario() {
    return m_raceScenario;
}

size_t RaceConfig::PlayerCount() {
    return Instance()->raceScenario().playerCount();
}

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

RaceConfig::Scenario::Scenario() : m_gameMode(GameMode::Time_Trial), m_lapCount(3) {}

RaceConfig::Scenario::~Scenario() = default;

void RaceConfig::Scenario::init() {
    s8 controllerCount = HostSystem::Instance()->controllerCount();
    s8 controllerId = 0;
    s8 ghostCount = HostSystem::Instance()->ghostCount();
    s8 ghostId = 0;

    m_playerCount = controllerCount + ghostCount;

    for (size_t i = 0; i < m_playerCount; i++) {
        if (controllerId < controllerCount) {
            m_players[i].initLocal(controllerId);
            controllerId++;
        } else if (ghostId < ghostCount) {
            m_players[i].initGhost();
            ghostId++;
        } else {
            break;
        }
    }
}

const RaceConfig::Scenario::Player &RaceConfig::Scenario::player(size_t idx) const {
    return m_players[idx];
}

RaceConfig::Scenario::Player &RaceConfig::Scenario::player(size_t idx) {
    return m_players[idx];
}

size_t RaceConfig::Scenario::playerCount() const {
    return m_playerCount;
}

RaceConfig::Scenario::Player::Player()
    : m_vehicle(Vehicle::Flame_Runner), m_character(Character::Funky_Kong), m_type(Type::None),
      m_controllerId(-1) {}

RaceConfig::Scenario::Player::~Player() = default;

void RaceConfig::Scenario::Player::init(s8 controllerId) {
    m_controllerId = controllerId;
    if (controllerId >= 0) {
        initLocal(controllerId);
    } else {
        initGhost();
    }
}

void RaceConfig::Scenario::Player::initLocal(s8 controllerId) {
    m_character = HostSystem::Instance()->character(controllerId);
    m_vehicle = HostSystem::Instance()->vehicle(controllerId);
    m_type = Type::Local;
    K_LOG("Local player initialized with character %d and vehicle %d!", m_character, m_vehicle);
}

void RaceConfig::Scenario::Player::initGhost() {
    // TODO: Parse ghost for character/vehicle
    m_type = Type::Ghost;
    K_LOG("Ghost initialized with character %d and vehicle %d!", m_character, m_vehicle);
}

Vehicle RaceConfig::Scenario::Player::vehicle() const {
    return m_vehicle;
}

Character RaceConfig::Scenario::Player::character() const {
    return m_character;
}

RaceConfig::Scenario::Player::Type RaceConfig::Scenario::Player::type() const {
    return m_type;
}

RaceConfig *RaceConfig::s_instance = nullptr;

} // namespace System
