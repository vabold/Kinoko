#include "RaceConfig.hh"

#include "game/system/KPadDirector.hh"

#include <abstract/File.hh>

namespace System {

/// @addr{0x8052DD40}
void RaceConfig::init() {
    m_raceScenario.init();
}

/// @addr{0x805302C4}
/// @details Normally we copy the menu scenario into the race scenario.
/// However, Kinoko doesn't support menus, so we use a callback.
void RaceConfig::initRace() {
    m_raceScenario.playerCount = 1;

    if (s_onInitCallback) {
        s_onInitCallback(this, s_onInitCallbackArg);
    }

    initControllers();
}

/// @addr{0x8052F4E8}
/// @brief Initializes the controllers.
/// @details This is normally scoped within RaceConfig::Scenario, but Kinoko doesn't support menus.
void RaceConfig::initControllers() {
    switch (m_raceScenario.players[0].type) {
    case Player::Type::Ghost:
        initGhost();
        break;
    case Player::Type::Local:
        KPadDirector::Instance()->setHostPad(m_raceScenario.players[0].driftIsAuto);
        break;
    default:
        PANIC("Players must be either local or ghost!");
        break;
    }
}

/// @addr{0x8052EEF0}
/// @brief Initializes the ghost.
/// @details This is normally scoped within RaceConfig::Scenario, but Kinoko doesn't support menus.
void RaceConfig::initGhost() {
    GhostFile ghost(m_ghost);

    m_raceScenario.course = ghost.course();
    Player &player = m_raceScenario.players[0];
    player.character = ghost.character();
    player.vehicle = ghost.vehicle();
    player.driftIsAuto = ghost.driftIsAuto();

    KPadDirector::Instance()->setGhostPad(ghost.inputs(), ghost.driftIsAuto());
}

/// @addr{0x8052FE58}
RaceConfig *RaceConfig::CreateInstance() {
    ASSERT(!s_instance);
    s_instance = new RaceConfig;
    return s_instance;
}

/// @addr{0x8052FFE8}
void RaceConfig::DestroyInstance() {
    ASSERT(s_instance);
    auto *instance = s_instance;
    s_instance = nullptr;
    delete instance;
}

/// @addr{0x8053015C}
RaceConfig::RaceConfig() = default;

/// @addr{0x80530038}
RaceConfig::~RaceConfig() {
    if (s_instance) {
        s_instance = nullptr;
        WARN("RaceConfig instance not explicitly handled!");
    }
}

/// @addr{Inlined in 0x8052DD40}
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

RaceConfig *RaceConfig::s_instance = nullptr; ///< @addr{0x809BD728}

/** @brief Host-agnostic way of initializing RaceConfig.
    The type of the first player *must* be set to either Local or Ghost.

    - If the type is Ghost, m_ghost must be set to a decompressed ghost file.

    - If the type is Local, the race scenario's course and the first player's character, vehicle,
    and driftIsAuto must be set.
*/
RaceConfig::InitCallback RaceConfig::s_onInitCallback = nullptr;

/// @brief The argument sent into the callback. This is expected to be reinterpret_casted.
void *RaceConfig::s_onInitCallbackArg = nullptr;

} // namespace System
