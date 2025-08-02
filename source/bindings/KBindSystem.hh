#pragma once

#include "Common.hh"
#include "egg/core/Disposer.hh"
#include "egg/core/SceneManager.hh"
#include "game/system/KPadController.hh"
#include "game/system/RaceConfig.hh"

class KBindSystem final : public EGG::Disposer {
public:
    KBindSystem();

    /// @brief Initializes the scene manager and starts the root scene
    void init();

    /// @brief Steps the simulation by a frame
    void step();

    /// @brief Sets the course for the race. Must be called before init()
    void SetCourse(Course course);

    /// @brief Configures a racer. Must be called before init()
    void SetPlayer(int slot, Character character, Vehicle vehicle, bool driftIsAuto);

    /// @brief Gets the host controller for changing controller state
    /// At the moment Kinoko only supports one racer, so this returns the controller for player 0
    /// @return A pointer to the KPadHostController for the local player
    [[nodiscard]] static System::KPadHostController *GetHostController();

    static KBindSystem *CreateInstance();

    static void DestroyInstance();

    [[nodiscard]] static KBindSystem *Instance() {
        return s_instance;
    }

private:
    ~KBindSystem() override;
    KBindSystem(const KBindSystem &) = delete;
    KBindSystem(KBindSystem &&) = delete;

    System::RaceConfig::Scenario m_scenario;
    EGG::SceneManager *m_sceneMgr;
    static KBindSystem *s_instance;
};
