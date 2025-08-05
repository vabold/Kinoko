#pragma once

#include "game/system/GhostFile.hh"

#include <functional>

class SavestateManager;

namespace System {

/// @addr{0x809BD728}
/// @brief Initializes the player with parameters specified in the provided ghost file.
/// @details In the base game, this class is responsible for managing the race and menu scenarios.
/// The menu scenario mostly pertains to character and vehicle selection in the menus prior to
/// starting a race. In Kinoko, we don't have these menus, so we initialize the race directly
/// through this class.
class RaceConfig : EGG::Disposer {
    friend class ::SavestateManager;

public:
    struct Player {
    public:
        enum class Type {
            Local = 0, // Inputs managed by ML algorithm
            Ghost = 3, // Inputs managed by ghost
            None = 5,
        };

        Character character;
        Vehicle vehicle;
        Type type;
        bool driftIsAuto;
    };

    struct Scenario {
    public:
        enum class GameMode {
            Time_Trial = 2,
            Ghost_Race = 5,
        };

        void init();

        std::array<Player, 12> players;
        u8 playerCount;
        Course course;
    };

    typedef std::function<void(RaceConfig *, void *)> InitCallback;

    void init();
    void initRace();
    void initControllers();
    void initGhost();

    [[nodiscard]] const Scenario &raceScenario() const {
        return m_raceScenario;
    }

    [[nodiscard]] Scenario &raceScenario() {
        return m_raceScenario;
    }

    void setGhost(const u8 *rkg) {
        m_ghost = rkg;
    }

    static void RegisterInitCallback(const InitCallback &callback, void *arg) {
        s_onInitCallback = callback;
        s_onInitCallbackArg = arg;
    }

    static RaceConfig *CreateInstance();
    static void DestroyInstance();

    [[nodiscard]] static RaceConfig *Instance() {
        return s_instance;
    }

private:
    RaceConfig();
    ~RaceConfig() override;

    Scenario m_raceScenario;
    RawGhostFile m_ghost;

    static RaceConfig *s_instance; ///< @addr{0x809BD728}
    static InitCallback s_onInitCallback;
    static void *s_onInitCallbackArg;
};

} // namespace System
