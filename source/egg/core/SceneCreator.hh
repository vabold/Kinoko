#pragma once

namespace EGG {

class Scene;

/// @brief Interface for creating and destroying scenes.
class SceneCreator {
public:
    virtual Scene *create(int sceneID) const = 0;
    virtual void destroy(int sceneID) const = 0;
};

} // namespace EGG
