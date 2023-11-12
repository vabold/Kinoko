#include "RaceConfig.hh"

#include <host/System.hh>

namespace System {

void RaceConfig::init() {
    m_raceScenario.init();
}

void RaceConfig::initRace() {
    // Normally we copy the menu scenario into the race scenario
    // There's no menu scenario in Kinoko, so instead we initialize values here
    // TODO: read from parameter file
    m_raceScenario.m_playerCount = 1;
    m_raceScenario.m_course = Course::SNES_Mario_Circuit_3;
    Player &player = m_raceScenario.m_players[0];
    player.m_character = Character::Daisy;
    player.m_vehicle = Vehicle::Mach_Bike;
    player.m_type = Player::Type::Ghost;
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

void RaceConfig::Scenario::init() {
    m_playerCount = 0;
    m_course = Course::GCN_Mario_Circuit;

    for (size_t i = 0; i < m_players.size(); ++i) {
        Player &player = m_players[i];
        player.m_character = Character::Mario;
        player.m_vehicle = Vehicle::Standard_Kart_M;
        player.m_type = Player::Type::None;
    }
}

RaceConfig *RaceConfig::s_instance = nullptr;

} // namespace System
