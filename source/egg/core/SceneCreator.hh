#pragma once

namespace EGG {

class Scene;

class SceneCreator {
public:
    virtual Scene *create(int sceneID) const = 0;
    virtual void destroy(int sceneID) const = 0;
};

} // namespace EGG
