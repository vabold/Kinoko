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
    // TODO: read from parameter file
    m_raceScenario.playerCount = 1;
    m_raceScenario.course = Course::SNES_Mario_Circuit_3;
    Player &player = m_raceScenario.players[0];
    player.character = Character::Daisy;
    player.vehicle = Vehicle::Mach_Bike;
    player.type = Player::Type::Ghost;

    size_t size;
    u8 *rkg = Abstract::File::Load("/Tests/rMC3-1-17-844.rkg", size);
    m_raceScenario.ghost = new RawGhostFile(rkg);

    initControllers();
}

void RaceConfig::initControllers() {
    // No need for raw validation, as we already do this
    GhostFile ghost(m_raceScenario.ghost);

    if (ghost.course() != m_raceScenario.course) {
        K_PANIC("Ghost is playing on wrong track! %u", ghost.course());
    }

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
