#include "RootScene.hh"

#include "source/game/system/InputManager.hh"
#include "source/game/system/RaceConfig.hh"
#include "source/game/system/ResourceManager.hh"

#include <source/egg/core/SceneManager.hh>
#include <source/host/SceneId.hh>

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
