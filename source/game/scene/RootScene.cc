#include "RootScene.hh"

#include "game/system/InputManager.hh"
#include "game/system/RaceConfig.hh"
#include "game/system/ResourceManager.hh"

#include <egg/core/SceneManager.hh>
#include <host/SceneId.hh>

namespace Scene {

RootScene::RootScene() = default;

RootScene::~RootScene() = default;

void RootScene::enter() {
    allocate();
    init();
    m_sceneMgr->createChildScene(static_cast<int>(Host::SceneId::Race), this);
}

void RootScene::allocate() {
    System::ResourceManager::CreateInstance();
    System::InputManager::CreateInstance();
    System::RaceConfig::CreateInstance();
}

void RootScene::init() {
    System::RaceConfig::Instance()->init();
    System::InputManager::Instance()->init();
}

} // namespace Scene
