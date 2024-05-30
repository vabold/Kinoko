#include "Scene.hh"

namespace EGG {

/// @addr{0x8023AD10}
Scene::Scene() {
    m_parent = nullptr;
    m_child = nullptr;
    m_id = -1;
    m_sceneMgr = nullptr;
}

/// @addr{0x8023AD84}
Scene::~Scene() = default;

void Scene::setParent(Scene *parent) {
    m_parent = parent;
}

void Scene::setChild(Scene *child) {
    m_child = child;
}

void Scene::setId(int id) {
    m_id = id;
}

void Scene::setSceneMgr(SceneManager *sceneMgr) {
    m_sceneMgr = sceneMgr;
}

Scene *Scene::parent() const {
    return m_parent;
}

Scene *Scene::child() const {
    return m_child;
}

int Scene::id() const {
    return m_id;
}

SceneManager *Scene::sceneMgr() const {
    return m_sceneMgr;
}

} // namespace EGG
