#include "System.hh"

#include "source/host/SceneCreatorDynamic.hh"

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