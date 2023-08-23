#include "SceneCreatorDynamic.hh"

#include <game/scene/RootScene.hh>

namespace Host {

EGG::Scene *SceneCreatorDynamic::create(int sceneId) const {
    return create(static_cast<SceneId>(sceneId));
}

void SceneCreatorDynamic::destroy(int sceneId) const {
    destroy(static_cast<SceneId>(sceneId));
}

// TODO
EGG::Scene *SceneCreatorDynamic::create(SceneId sceneId) const {
    switch (sceneId) {
    case SceneId::Root:
        return new Scene::RootScene;
    case SceneId::Race:
        return nullptr;
    default:
        K_PANIC("Unreachable scene creation!");
    }
}

// TODO
void SceneCreatorDynamic::destroy(SceneId sceneId) const {
    switch (sceneId) {
    case SceneId::Root:
        break;
    case SceneId::Race:
        break;
    default:
        K_PANIC("Unreachable scene deletion!");
    }
}

} // namespace Host
