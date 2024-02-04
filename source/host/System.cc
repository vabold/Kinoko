#include "System.hh"

#include "host/SceneCreatorDynamic.hh"

namespace Host {

int KSystem::main() {
    init();
    m_sceneMgr->changeScene(0);
    if (!m_testDirector->init()) {
        return 1;
    }
    return run() ? 0 : 1;
}

void KSystem::init() {
    auto *sceneCreator = new SceneCreatorDynamic;
    m_sceneMgr = new EGG::SceneManager(sceneCreator);

    m_testDirector = new Test::TestDirector;
}

bool KSystem::run() {
    do {
        m_sceneMgr->calc();
    } while (m_testDirector->calc());

    return m_testDirector->sync();
}

KSystem &KSystem::Instance() {
    static KSystem instance;
    return instance;
}

KSystem::KSystem() = default;

KSystem::~KSystem() = default;

} // namespace Host