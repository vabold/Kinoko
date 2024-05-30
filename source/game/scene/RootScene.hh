#pragma once

#include <egg/core/Scene.hh>

namespace Scene {

/// @brief The parent scene for all other scenes.
class RootScene final : public EGG::Scene {
public:
    RootScene();
    ~RootScene() override;

    void enter() override;

private:
    void allocate();
    void init();
};

} // namespace Scene
