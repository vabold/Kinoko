#pragma once

#include "Common.hh"

#include "game/kart/KartObjectManager.hh"

#include "game/system/KPadController.hh"
#include "game/system/RaceConfig.hh"

#include <egg/core/SceneManager.hh>

#include <host/KSystem.hh>

class KBindSystem final : public KSystem {
public:
    KBindSystem();
    ~KBindSystem() override;

    void init() override;
    void calc() override;
    bool run() override {
        return true;
    }
    void parseOptions(int /*argc*/, char ** /*argv*/) override {}

    /// @brief Sets the course for the race. Must be called before init()
    void SetCourse(Course course);

    /// @brief Configures a racer. Must be called before init()
    void SetPlayer(int slot, Character character, Vehicle vehicle, bool driftIsAuto);

    /// @brief Gets the host controller for changing controller state
    /// At the moment Kinoko only supports one racer, so this returns the controller for player 0
    /// @return A pointer to the KPadHostController for the local player
    [[nodiscard]] static System::KPadHostController *GetHostController();

    /// @brief Gets the KartObjectProxy instance for a specific slot
    /// @return A pointer to the KartObjectInstance for the specified slot
    [[nodiscard]] Kart::KartObjectProxy *GetKart(int slot);

    static KBindSystem *CreateInstance();

    static void DestroyInstance();

    [[nodiscard]] static KBindSystem *Instance() {
        return s_instance;
    }

private:
    KBindSystem(const KBindSystem &) = delete;
    KBindSystem(KBindSystem &&) = delete;

    System::RaceConfig::Scenario m_scenario;
    EGG::SceneManager *m_sceneMgr;
    static KBindSystem *s_instance;
};
