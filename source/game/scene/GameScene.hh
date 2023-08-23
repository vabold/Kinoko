#pragma once

#include <egg/core/Scene.hh>

namespace Scene {

class GameScene : public EGG::Scene {
public:
    GameScene();
    ~GameScene() override;

    void calc() final;
    void enter() final;
    void exit() final;
    void reinit() final;

    virtual void createEngines() = 0;
    virtual void initEngines() = 0;
    virtual void calcEngines() = 0;
    virtual void destroyEngines() = 0;
    virtual void configure() = 0;
    virtual void onReinit() {}

private:
    void initScene();
    void deinitScene();

    int m_nextSceneId;
};

} // namespace Scene
