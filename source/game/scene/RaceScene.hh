#pragma once

#include "game/scene/GameScene.hh"

namespace Scene {

/// @brief Represents an instance of a race
class RaceScene final : public GameScene {
public:
    RaceScene();
    ~RaceScene() override;

    void createEngines() override;
    void initEngines() override;
    void calcEngines() override;
    void destroyEngines() override;
    void configure() override;
    void onReinit() override;
};

} // namespace Scene
