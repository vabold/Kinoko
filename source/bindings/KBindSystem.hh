#pragma once

#include <vector>

#include "Common.hh"
#include "egg/core/Disposer.hh"
#include "egg/core/SceneManager.hh"
#include "game/system/KPadController.hh"
#include "game/system/RaceConfig.hh"

namespace EGG {
class SceneManager;
}

class KBindSystem final : public EGG::Disposer {
public:
    KBindSystem();

    /// @brief Initializes the scene manager and starts the root scene
    void init();

    /// @brief Steps the simulation by a frame
    void step();

    /// @brief Sets the course for the race. Must be called before init()
    void set_course(Course course);

    /// @brief Configures a racer. Must be called before init()
    void SetPlayer(int slot, Character character, Vehicle vehicle, bool driftIsAuto);

    /// @brief Gets the host controller for changing controller state
    /// At the moment Kinoko only supports one racer, so this returns the controller for player 0
    /// @return A pointer to the KPadHostController for the local player
    System::KPadHostController *get_host_controller();

    static KBindSystem *CreateInstance();

    static void DestroyInstance();

    static KBindSystem *Instance() {
        return s_instance;
    }

private:
    ~KBindSystem() override;
    KBindSystem(const KBindSystem &) = delete;
    KBindSystem(KBindSystem &&) = delete;

    // Scenario Settings
    // We need to store a copy of them locally to prevent a crash, trying
    // to do it via System::RaceConfig::raceScenario() causes a second
    // RaceConfig instance when RootScene is created, causing it to segfault.
    Course m_course = Course::Luigi_Circuit;
    struct PlayerConfig {
        Character character;
        Vehicle vehicle;
        bool driftIsAuto;
    };
    std::vector<PlayerConfig> m_players;

    static KBindSystem *s_instance;
    EGG::SceneManager *m_sceneMgr;
};
