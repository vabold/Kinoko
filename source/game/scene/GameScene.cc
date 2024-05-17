#include "GameScene.hh"

#include "game/system/KPadDirector.hh"
#include "game/system/ResourceManager.hh"

#include <egg/core/SceneManager.hh>

namespace Scene {

GameScene::GameScene() {
    m_nextSceneId = -1;
}

GameScene::~GameScene() = default;

void GameScene::calc() {
    System::KPadDirector::Instance()->calc();
    calcEngines();
}

void GameScene::enter() {
    configure();
    initScene();
}

void GameScene::exit() {
    deinitScene();
    unmountResources();
}

void GameScene::reinit() {
    exit();
    if (m_nextSceneId < 0) {
        onReinit();
        initScene();
    } else {
        m_sceneMgr->changeSiblingScene(m_nextSceneId);
    }
}

void GameScene::appendResource(System::MultiDvdArchive *archive, s32 id) {
    m_resources.push_back(new Resource(archive, id));
}

GameScene::Resource::Resource(System::MultiDvdArchive *archive, s32 id)
    : archive(archive), id(id) {}

void GameScene::initScene() {
    createEngines();
    System::KPadDirector::Instance()->reset();
    initEngines();
}

void GameScene::deinitScene() {
    if (m_nextSceneId >= 0) {
        return;
    }

    destroyEngines();
    System::KPadDirector::Instance()->clear();
}

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
