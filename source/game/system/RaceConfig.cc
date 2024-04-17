#include "RaceConfig.hh"

#include "game/system/GhostFile.hh"
#include "game/system/KPadDirector.hh"

#include <abstract/File.hh>
#include <host/System.hh>

namespace System {

void RaceConfig::init() {
    m_raceScenario.init();
}

void RaceConfig::initRace() {
    // Normally we copy the menu scenario into the race scenario
    // There's no menu scenario in Kinoko, so instead we initialize values here
    // For now, we want to initialize solely based off the ghost file.
    m_raceScenario.playerCount = 1;

    size_t size;
    const auto *testDirector = Host::KSystem::Instance().testDirector();
    u8 *rkg = Abstract::File::Load(testDirector->testCase().rkgPath.data(), size);
    m_ghost = rkg;
    delete[] rkg;
    GhostFile ghost(m_ghost);

    m_raceScenario.course = ghost.course();
    Player &player = m_raceScenario.players[0];
    player.character = ghost.character();
    player.vehicle = ghost.vehicle();
    player.type = Player::Type::Ghost;

    initControllers(ghost);
}

void RaceConfig::initControllers(const GhostFile &ghost) {
    KPadDirector::Instance()->setGhostPad(ghost.inputs(), ghost.driftIsAuto());
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
    playerCount = 0;
    course = Course::GCN_Mario_Circuit;

    for (size_t i = 0; i < players.size(); ++i) {
        Player &player = players[i];
        player.character = Character::Mario;
        player.vehicle = Vehicle::Standard_Kart_M;
        player.type = Player::Type::None;
    }
}

RaceConfig *RaceConfig::s_instance = nullptr;

} // namespace System
