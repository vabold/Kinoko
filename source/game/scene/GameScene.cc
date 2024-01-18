#include "GameScene.hh"

#include "game/system/InputManager.hh"

#include <egg/core/SceneManager.hh>

namespace Scene {

GameScene::GameScene() {
    m_nextSceneId = -1;
}

GameScene::~GameScene() = default;

void GameScene::calc() {
    System::InputManager::Instance()->calc();
    calcEngines();
}

void GameScene::enter() {
    configure();
    initScene();
}

void GameScene::exit() {
    deinitScene();
}

void GameScene::reinit() {
    deinitScene();
    if (m_nextSceneId < 0) {
        onReinit();
        initScene();
    } else {
        m_sceneMgr->changeSiblingScene(m_nextSceneId);
    }
}

void GameScene::appendResource(System::MultiDvdArchive * /*arc*/, s32 /*id*/) {}

void GameScene::initScene() {
    createEngines();
    initEngines();
}

void GameScene::deinitScene() {
    if (m_nextSceneId >= 0) {
        return;
    }

    destroyEngines();
    System::InputManager::Instance()->clear();
}

} // namespace Scene
