#include "SceneManager.hh"

namespace EGG {

/*------------*
    Lifecycle
 *------------*/

SceneManager::SceneManager(SceneCreator *creator) {
    m_creator = creator;
    m_currentScene = nullptr;
    m_currentFader = nullptr;
    m_fadeType = FadeType::Idle;
    createDefaultFader();
}

SceneManager::~SceneManager() = default;

/*----------*
    Virtual
 *----------*/

void SceneManager::calc() {
    calcCurrentScene();
    calcCurrentFader();
}

void SceneManager::calcCurrentScene() {
    if (!m_currentScene) {
        return;
    }

    m_currentScene->calc();
}

// This currently does nothing, but in the interest of including the base game's scene management
// system, we add its proper definition in the trimmed library.
void SceneManager::calcCurrentFader() {
    if (!m_currentFader) {
        return;
    }

    if (!m_currentFader->calc()) {
        return;
    }

    switch (m_fadeType) {
    case FadeType::ChangeScene:
        changeScene(m_nextSceneId);
        break;
    case FadeType::ChangeSiblingScene:
        changeSiblingScene();
        break;
    case FadeType::Reinitialize:
        reinitCurrentScene();
        break;
    default:
        break;
    }

    resetAfterFadeType();
}

void SceneManager::createDefaultFader() {
    m_currentFader = new ColorFader;
}

/*----------*
    Methods
 *----------*/

void SceneManager::createChildScene(int id, Scene *parent) {
    outgoingParentScene(parent);
    setNextSceneId(id);
    setupNextSceneId();
    createScene(id, parent);
}

void SceneManager::createScene(int id, Scene *parent) {
    Scene *newScene = m_creator->create(id);

    if (parent) {
        parent->setChild(newScene);
    }

    m_currentScene = newScene;
    newScene->setId(id);
    newScene->setParent(parent);
    newScene->setSceneMgr(this);
    newScene->enter();
}

void SceneManager::destroyScene(Scene *scene) {
    scene->exit();
    if (scene->child()) {
        destroyScene(scene);
    }

    Scene *parent = scene->parent();
    m_creator->destroy(scene->id());
    m_currentScene = nullptr;

    if (!parent) {
        return;
    }

    parent->setChild(nullptr);
    m_currentScene = parent;
}

void SceneManager::changeScene(int nextSceneId) {
    while (m_currentScene) {
        destroyCurrentSceneNoIncoming(true);
    }

    changeSiblingScene(nextSceneId);
}

void SceneManager::changeSiblingScene(int nextSceneId) {
    setNextSceneId(nextSceneId);
    changeSiblingScene();
}

void SceneManager::changeSiblingScene() {
    Scene *parentScene = m_currentScene ? m_currentScene->parent() : nullptr;

    if (m_currentScene) {
        destroyScene(m_currentScene);
        m_currentScene = nullptr;
    }

    int nextSceneId = m_nextSceneId;
    setupNextSceneId();
    createScene(nextSceneId, parentScene);
}

void SceneManager::incomingCurrentScene() {
    if (!m_currentScene) {
        return;
    }

    m_currentScene->incoming_childDestroy();
}

void SceneManager::outgoingParentScene(Scene *parent) {
    parent->outgoing_childCreate();
}

void SceneManager::reinitCurrentScene() {
    if (!m_currentScene) {
        return;
    }

    m_currentScene->reinit();
}

bool SceneManager::changeSiblingSceneAfterFadeOut(int id) {
    if (m_fadeType != FadeType::Idle || !fadeOut()) {
        return false;
    }

    setNextSceneId(id);
    setAfterFadeType(FadeType::ChangeSiblingScene);
    return true;
}

bool SceneManager::reinitCurrentSceneAfterFadeOut() {
    if (m_fadeType != FadeType::Idle || !fadeOut()) {
        return false;
    }

    setAfterFadeType(FadeType::Reinitialize);
    return true;
}

bool SceneManager::destroyCurrentSceneNoIncoming(bool destroyRootIfNoParent) {
    if (!m_currentScene) {
        return false;
    }

    Scene *parent = m_currentScene->parent();
    if (parent) {
        destroyScene(parent->child());
        setNextSceneId(parent->id());
        setupNextSceneId();
        return true;
    } else {
        if (destroyRootIfNoParent) {
            destroyScene(m_currentScene);
            setNextSceneId(-1);
            setupNextSceneId();
        }
        return false;
    }
}

bool SceneManager::destroyToSelectSceneId(int id) {
    Scene *parent = findParentScene(id);
    if (!parent) {
        return false;
    }

    while (parent->id() != currentSceneId()) {
        destroyCurrentSceneNoIncoming(false);
    }
    incomingCurrentScene();
    return true;
}

Scene *SceneManager::findParentScene(int id) {
    Scene *scene = m_currentScene->parent();
    for (; scene; scene = scene->parent()) {
        if (scene->id() == id) {
            break;
        }
    }

    return scene;
}

void SceneManager::setupNextSceneId() {
    m_prevSceneId = m_currentSceneId;
    m_currentSceneId = m_nextSceneId;
    m_nextSceneId = -1;
}

bool SceneManager::fadeIn() {
    return m_currentFader->fadeIn();
}

bool SceneManager::fadeOut() {
    return m_currentFader->fadeOut();
}

void SceneManager::resetAfterFadeType() {
    m_fadeType = FadeType::Idle;
}

} // namespace EGG
