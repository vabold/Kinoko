#include "System.hh"

#include "host/SceneCreatorDynamic.hh"

namespace Host {

int KSystem::main() {
    init();
    m_sceneMgr->changeScene(0);
    run();
    return 0;
}

void KSystem::init() {
    auto *sceneCreator = new SceneCreatorDynamic;
    m_sceneMgr = new EGG::SceneManager(sceneCreator);
}

void KSystem::run() {
    K_LOG("Initialized successfully!");
    K_LOG("The program will now run in an infinite loop to test 'calc' functionality.");
    while (true) {
        m_sceneMgr->calc();
    }
}

KSystem &KSystem::Instance() {
    static KSystem instance;
    return instance;
}

KSystem::KSystem() = default;

KSystem::~KSystem() = default;

} // namespace Host