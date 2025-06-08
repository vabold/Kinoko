#include "SceneManager.hh"

#include "egg/core/ExpHeap.hh"

namespace EGG {

/*------------*
    Lifecycle
 *------------*/

/// @addr{0x8023ADDC}
SceneManager::SceneManager(SceneCreator *creator) {
    m_creator = creator;
    m_currentScene = nullptr;
}

SceneManager::~SceneManager() = default;

/*----------*
    Virtual
 *----------*/

/// @addr{0x8023AE60}
void SceneManager::calc() {
    calcCurrentScene();
}

/// @addr{0x8023B588}
void SceneManager::calcCurrentScene() {
    if (!m_currentScene) {
        return;
    }

    m_currentScene->calc();
}

/*----------*
    Methods
 *----------*/

/// @addr{0x8023B248}
void SceneManager::createChildScene(int id, Scene *parent) {
    outgoingParentScene(parent);
    setNextSceneId(id);
    setupNextSceneId();
    createScene(id, parent);
}

/// @addr{0x8023B0E4}
void SceneManager::createScene(int id, Scene *parent) {
    Heap *parentHeap = parent ? parent->heap() : s_rootHeap;

    // We need to preserve the locked status to reinstate it later
    bool locked = parentHeap->tstDisableAllocation();
    if (locked) {
        parentHeap->enableAllocation();
    }

    ExpHeap *newHeap = ExpHeap::create(-1, parentHeap, s_heapOptionFlg);
    s_heapForCreateScene = newHeap;

    if (locked) {
        parentHeap->disableAllocation();
    }

    newHeap->becomeCurrentHeap();
    newHeap->setName("DefaultSceneHeap");

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

/// @addr{0x8023B3F0}
void SceneManager::destroyScene(Scene *scene) {
    scene->exit();
    if (scene->child()) {
        destroyScene(scene->child());
    }

    Scene *parent = scene->parent();
    m_creator->destroy(scene->id());

    // ADDED: The original library does not actually delete the scene, but we run into leaks
    // So, we delete the scene that's provided to the function
    auto *heap = scene->heap();
    delete scene;

    m_currentScene = nullptr;

    if (parent) {
        parent->setChild(nullptr);
        m_currentScene = parent;
    }

    heap->destroy();
    Heap *parentHeap = parent ? parent->heap() : s_rootHeap;
    parentHeap->becomeCurrentHeap();
}

/// @addr{0x8023AF84}
void SceneManager::changeScene(int nextSceneId) {
    while (m_currentScene) {
        destroyCurrentSceneNoIncoming(true);
    }

    changeSiblingScene(nextSceneId);
}

/// @addr{0x8023AFE0}
void SceneManager::changeSiblingScene(int nextSceneId) {
    setNextSceneId(nextSceneId);
    changeSiblingScene();
}

/// @addr{0x8023B064}
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

/// @addr{0x8023B568}
void SceneManager::incomingCurrentScene() {
    if (!m_currentScene) {
        return;
    }

    m_currentScene->incoming_childDestroy();
}

/// @addr{0x8023B92C}
void SceneManager::outgoingParentScene(Scene *parent) {
    parent->outgoing_childCreate();
}

/// @addr{0x8023AEF8}
void SceneManager::reinitCurrentScene() {
    if (!m_currentScene) {
        return;
    }

    m_currentScene->reinit();
}

/// @addr{0x8023B2AC}
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

/// @addr{0x8023B344}
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

/// @addr{0x8023B940}
Scene *SceneManager::findParentScene(int id) const {
    Scene *scene = m_currentScene->parent();
    for (; scene; scene = scene->parent()) {
        if (scene->id() == id) {
            break;
        }
    }

    return scene;
}

/// @addr{0x8023B910}
void SceneManager::setupNextSceneId() {
    m_prevSceneId = m_currentSceneId;
    m_currentSceneId = m_nextSceneId;
    m_nextSceneId = -1;
}

Heap *SceneManager::s_heapForCreateScene = nullptr;
u16 SceneManager::s_heapOptionFlg = 2;

Heap *SceneManager::s_rootHeap = nullptr;

} // namespace EGG
