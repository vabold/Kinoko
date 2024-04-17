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
    Resource *resource = new Resource(archive, id);
    m_resources.append(resource);
}

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
    for (Abstract::Node *node = m_resources.head(); node;) {
        Resource *resource = node->data<Resource>();
        resourceManager->unmount(resource->archive);
        Abstract::Node *nextNode = m_resources.getNext(node);
        m_resources.remove(node);
        delete resource;
        node = nextNode;
    }
}

} // namespace Scene
