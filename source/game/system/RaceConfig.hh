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
        };

        void init();

        std::array<Player, 12> players;
        u8 playerCount;
        Course course;
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
