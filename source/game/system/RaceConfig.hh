#pragma once

#include <Common.hh>

#include "game/system/GhostFile.hh"

namespace System {

// TODO: elaborate on implementation
class RaceConfig {
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
    };

    struct Scenario {
    public:
        enum class GameMode {
            Time_Trial = 2,
            Ghost_Race = 5,
            LocalGameModeMax = 6,
        };

        void init();

        inline bool isOnlineGameMode() const {
            return GameMode::LocalGameModeMax < gameMode;
        }
        inline bool isInTimeTrials() const {
            return gameMode == GameMode::Time_Trial || gameMode == GameMode::Ghost_Race;
        }

        std::array<Player, 12> players;
        u8 playerCount;
        Course course;
        GameMode gameMode;
    };

    void init();
    void initRace();
    void initControllers(const GhostFile &ghost);

    const Scenario &raceScenario() const {
        return m_raceScenario;
    }

    static RaceConfig *CreateInstance();
    static void DestroyInstance();
    static RaceConfig *Instance();

private:
    RaceConfig();
    ~RaceConfig();

    Scenario m_raceScenario;
    RawGhostFile m_ghost;

    static RaceConfig *s_instance;
};

} // namespace System
