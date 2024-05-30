#pragma once

#include "egg/core/ColorFader.hh"
#include "egg/core/Scene.hh"
#include "egg/core/SceneCreator.hh"

namespace EGG {

/// @brief Manages the scene stack and transitions between scenes.
class SceneManager {
public:
    /*----------*
        Structs
     *----------*/

    enum class FadeType {
        Idle = -1,
        ChangeScene = 0,
        ChangeSiblingScene = 1,
        // Outgoing = 2,
        // Incoming = 3,
        Reinitialize = 4,
    };

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
    virtual void calcCurrentFader();
    virtual void createDefaultFader();

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

    bool changeSiblingSceneAfterFadeOut(int id);
    bool reinitCurrentSceneAfterFadeOut();

    bool destroyCurrentSceneNoIncoming(bool destroyRootIfNoParent);
    bool destroyToSelectSceneId(int nextSceneId);
    Scene *findParentScene(int id);
    void setupNextSceneId();

    bool fadeIn();
    bool fadeOut();
    void resetAfterFadeType();

    /*----------*
        Getters
     *----------*/

    int currentSceneId() const {
        return m_currentSceneId;
    }

    /*----------*
        Setters
     *----------*/

    void setAfterFadeType(FadeType type) {
        m_fadeType = type;
    }

    void setNextSceneId(int id) {
        m_nextSceneId = id;
    }

private:
    /*----------*
        Members
     *----------*/

    SceneCreator *m_creator;
    Scene *m_currentScene;
    ColorFader *m_currentFader;
    FadeType m_fadeType;
    int m_nextSceneId;
    int m_currentSceneId;
    int m_prevSceneId;
};

} // namespace EGG
