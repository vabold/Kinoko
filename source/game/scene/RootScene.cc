#include "RootScene.hh"

#include "game/system/KPadDirector.hh"
#include "game/system/RaceConfig.hh"
#include "game/system/ResourceManager.hh"

#include <egg/core/SceneManager.hh>
#include <host/SceneId.hh>

namespace Scene {

/// @addr{0x80542878}
RootScene::RootScene() {
    m_heap->setName("RootSceneHeap");
}

/// @addr{0x805429A8}
RootScene::~RootScene() = default;

/// @addr{0x80543B84}
void RootScene::enter() {
    allocate();
    init();
    m_sceneMgr->createChildScene(static_cast<int>(Host::SceneId::Race), this);
}

/// @addr{0x80542D4C}
void RootScene::allocate() {
    System::ResourceManager::CreateInstance();
    System::KPadDirector::CreateInstance();
    System::RaceConfig::CreateInstance();
}

/// @addr{0x805438B4}
void RootScene::init() {
    System::RaceConfig::Instance()->init();
}

} // namespace Scene
