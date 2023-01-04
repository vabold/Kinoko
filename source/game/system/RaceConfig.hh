#pragma once

#include <Common.hh>

namespace System {

class RaceConfig {
public:
    class Scenario {
    public:
        class Player {
        public:
            enum class Type {
                Local = 0, // Inputs managed by ML algorithm
                Ghost = 3, // Inputs managed by ghost
                None = 5,
            };

            Player();
            ~Player();

            void init(s8 controllerId);
            void initLocal(s8 controllerId);
            void initGhost();

            Vehicle vehicle() const;
            Character character() const;
            Type type() const;

        private:
            Vehicle m_vehicle;
            Character m_character;
            Type m_type;
            s8 m_controllerId;
        };

        enum class GameMode {
            Time_Trial = 2,
            Ghost_Race = 5,
        };

        Scenario();
        ~Scenario();

        void init();

        const Player &player(size_t idx) const;
        Player &player(size_t idx);
        size_t playerCount() const;

    private:
        Player m_players[MAX_PLAYER_COUNT];
        size_t m_playerCount;
        GameMode m_gameMode;
        s8 m_lapCount;
    };

    void init();
    void setGhost();

    Scenario &raceScenario();

    static size_t PlayerCount();

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
