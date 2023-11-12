#include "Scene.hh"

namespace EGG {

Scene::Scene() {
    m_parent = nullptr;
    m_child = nullptr;
    m_id = -1;
    m_sceneMgr = nullptr;
}

Scene::~Scene() = default;

} // namespace EGG
