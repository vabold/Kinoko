#include "KBindSystem.hh"

#include "host/SceneCreatorDynamic.hh"

#include <game/system/KPadController.hh>
#include <game/system/KPadDirector.hh>
#include <game/system/RaceConfig.hh>

#include <egg/core/ExpHeap.hh>
#include <egg/core/SceneManager.hh>

KBindSystem *KBindSystem::s_instance = nullptr;

KBindSystem::KBindSystem() : m_sceneMgr(nullptr) {}

KBindSystem::~KBindSystem() {
    delete m_sceneMgr;
}

KBindSystem *KBindSystem::CreateInstance() {
    ASSERT(!s_instance);
    s_instance = new KBindSystem;
    return s_instance;
}

void KBindSystem::DestroyInstance() {
    delete s_instance;
    s_instance = nullptr;
}

void KBindSystem::init() {
    // Create a new sceneCreator (needed to make RaceScene)
    auto *sceneCreator = new Host::SceneCreatorDynamic;
    m_sceneMgr = new EGG::SceneManager(sceneCreator);

    // Register a callback to configure RaceConfig once it's created
    System::RaceConfig::RegisterInitCallback(
            [this](System::RaceConfig *config, void * /* arg */) {
                auto &scenario = config->raceScenario();

                // Set the scenario
                scenario.course = m_course;
                scenario.playerCount = m_players.size();

                for (size_t i = 0; i < m_players.size(); ++i) {
                    scenario.players[i].character = m_players[i].character;
                    scenario.players[i].vehicle = m_players[i].vehicle;
                    scenario.players[i].driftIsAuto = m_players[i].driftIsAuto;
                    scenario.players[i].type = System::RaceConfig::Player::Type::Local;
                }
            },
            nullptr);

    m_sceneMgr->changeScene(static_cast<int>(Host::SceneId::Root));
}

void KBindSystem::step() {
    if (m_sceneMgr) {
        m_sceneMgr->calc();
    }
}

System::KPadHostController *KBindSystem::get_host_controller() {
    return System::KPadDirector::Instance()->hostController();
}

void KBindSystem::set_course(Course course) {
    m_course = course;
}

void KBindSystem::set_player(int slot, Character character, Vehicle vehicle, bool driftIsAuto) {
    if (slot < 0 || slot > 12) {
        return;
    }
    m_players[slot] = {character, vehicle, driftIsAuto};
}
