#pragma once

#include "egg/core/Scene.hh"
#include "egg/core/SceneCreator.hh"

class SavestateManager;

namespace EGG {

/// @brief Manages the scene stack and transitions between scenes.
class SceneManager {
    friend class ::SavestateManager;

public:
    /*------------*
        Lifecycle
     *------------*/

    SceneManager(SceneCreator *creator = nullptr);
    virtual ~SceneManager();

    /*----------*
        Virtual
     *----------*/

    virtual void calc();
    virtual void calcCurrentScene();

    /*----------*
        Methods
     *----------*/

    void createChildScene(int nextSceneId, Scene *parent);
    void createScene(int nextSceneId, Scene *parent);
    void destroyScene(Scene *scene);

    void changeScene(int nextSceneId);
    void changeSiblingScene(int nextSceneId);
    void changeSiblingScene();
    void incomingCurrentScene();
    void outgoingParentScene(Scene *parent);
    void reinitCurrentScene();

    bool destroyCurrentSceneNoIncoming(bool destroyRootIfNoParent);
    bool destroyToSelectSceneId(int nextSceneId);
    [[nodiscard]] Scene *findParentScene(int id) const;
    void setupNextSceneId();

    /*----------*
        Getters
     *----------*/

    [[nodiscard]] Scene *currentScene() const {
        return m_currentScene;
    }

    [[nodiscard]] int currentSceneId() const {
        return m_currentSceneId;
    }

    [[nodiscard]] static Heap *heapForCreateScene() {
        return s_heapForCreateScene;
    }

    /*----------*
        Setters
     *----------*/

    void setNextSceneId(int id) {
        m_nextSceneId = id;
    }

    static void SetRootHeap(Heap *heap) {
        s_rootHeap = heap;
    }

private:
    /*----------*
        Members
     *----------*/

    SceneCreator *m_creator;
    Scene *m_currentScene;
    int m_nextSceneId;
    int m_currentSceneId;
    int m_prevSceneId;

    static Heap *s_heapForCreateScene;
    static u16 s_heapOptionFlg;

    static Heap *s_rootHeap;
};

} // namespace EGG
