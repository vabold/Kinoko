#pragma once

#include <Common.hh>

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

        Character m_character;
        Vehicle m_vehicle;
    };

    struct Scenario {
    public:
        enum class GameMode {
            Time_Trial = 2,
            Ghost_Race = 5,
        };

        Player m_players[12];
        u8 m_playerCount;
    };

    void init();

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

    static RaceConfig *s_instance;
};

} // namespace System
