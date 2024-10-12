#include "GameScene.hh"

#include "game/system/KPadDirector.hh"
#include "game/system/ResourceManager.hh"

#include <egg/core/SceneManager.hh>

namespace Scene {

/// @addr{0x8051A1E0}
GameScene::GameScene() {
    m_heap->setName("DefaultGameSceneHeap");
    m_nextSceneId = -1;
}

/// @addr{0x8051A3C0}
GameScene::~GameScene() = default;

/// @addr{0x8051B3C8}
void GameScene::calc() {
    System::KPadDirector::Instance()->calc();
    calcEngines();
}

/// @addr{0x8051AB58}
void GameScene::enter() {
    configure();
    initScene();
}

/// @addr{0x8051B250}
void GameScene::exit() {
    deinitScene();
    unmountResources();
}

/// @addr{0x8051B7B0}
void GameScene::reinit() {
    exit();
    if (m_nextSceneId < 0) {
        onReinit();
        initScene();
    } else {
        m_sceneMgr->changeSiblingScene(m_nextSceneId);
    }
}

/// @addr{0x8051AA58}
void GameScene::appendResource(System::MultiDvdArchive *archive, s32 id) {
    m_resources.push_back(new Resource(archive, id));
}

/// @addr{Inlined in 0x8051AA58}
GameScene::Resource::Resource(System::MultiDvdArchive *archive, s32 id)
    : archive(archive), id(id) {}

/// @addr{0x8051A4DC}
void GameScene::initScene() {
    createEngines();
    System::KPadDirector::Instance()->reset();
    initEngines();
}

/// @addr{0x8051B0F4}
void GameScene::deinitScene() {
    if (m_nextSceneId >= 0) {
        return;
    }

    destroyEngines();
    System::KPadDirector::Instance()->clear();
}

/// @addr{0x8051AAE8}
void GameScene::unmountResources() {
    auto *resourceManager = System::ResourceManager::Instance();
    for (auto iter = m_resources.begin(); iter != m_resources.end();) {
        Resource *resource = *iter;
        resourceManager->unmount(resource->archive);
        iter = m_resources.erase(iter);
        delete resource;
    }
}
} // namespace Scene
