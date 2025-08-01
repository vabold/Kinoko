#include "KBindSystem.hh"

#include "host/SceneCreatorDynamic.hh"

#include <game/system/KPadController.hh>
#include <game/system/KPadDirector.hh>
#include <game/system/RaceConfig.hh>

#include <egg/core/ExpHeap.hh>
#include <egg/core/SceneManager.hh>

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
    ASSERT(s_instance);
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
                config->raceScenario() = m_scenario;
            },
            nullptr);

    m_sceneMgr->changeScene(static_cast<int>(Host::SceneId::Root));
}

void KBindSystem::step() {
    if (m_sceneMgr) {
        m_sceneMgr->calc();
    }
}

System::KPadHostController *KBindSystem::GetHostController() {
    return System::KPadDirector::Instance()->hostController();
}

void KBindSystem::SetCourse(Course course) {
    m_scenario.course = course;
}

void KBindSystem::SetPlayer(int slot, Character character, Vehicle vehicle, bool driftIsAuto) {
    if (slot < 0 || slot > 12) {
        return;
    }

    m_scenario.players[slot] = {character, vehicle, System::RaceConfig::Player::Type::Local,
            driftIsAuto};

    if (m_scenario.playerCount <= slot) {
        m_scenario.playerCount = slot + 1;
    }
}

KBindSystem *KBindSystem::s_instance = nullptr;
