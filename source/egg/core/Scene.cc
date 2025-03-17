#include "Scene.hh"

#include "egg/core/SceneManager.hh"

namespace EGG {

/// @addr{0x8023AD10}
Scene::Scene() {
    m_heap = SceneManager::heapForCreateScene();
    m_parent = nullptr;
    m_child = nullptr;
    m_id = -1;
    m_sceneMgr = nullptr;
}

/// @addr{0x8023AD84}
Scene::~Scene() = default;

} // namespace EGG
