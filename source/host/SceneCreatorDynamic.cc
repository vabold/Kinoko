#include "SceneCreatorDynamic.hh"

#include <game/scene/RaceScene.hh>
#include <game/scene/RootScene.hh>

namespace Host {

EGG::Scene *SceneCreatorDynamic::create(int sceneId) const {
    return create(static_cast<SceneId>(sceneId));
}

void SceneCreatorDynamic::destroy(int sceneId) const {
    destroy(static_cast<SceneId>(sceneId));
}

EGG::Scene *SceneCreatorDynamic::create(SceneId sceneId) const {
    switch (sceneId) {
    case SceneId::Root:
        return new Scene::RootScene;
    case SceneId::Race:
        return new Scene::RaceScene;
    default:
        K_PANIC("Unreachable scene creation!");
    }
}

void SceneCreatorDynamic::destroy(SceneId sceneId) const {
    switch (sceneId) {
    case SceneId::Root:
    case SceneId::Race:
        // The base game doesn't do anything, so we don't either
        break;
    default:
        K_PANIC("Unreachable scene deletion!");
    }
}

} // namespace Host
