#pragma once

#include <Common.hh>

#include "game/system/GhostFile.hh"

namespace System {

/// @addr{0x809BD728}
/// @brief Initializes the player with parameters specified in the provided ghost file.
/// @details In the base game, this class is responsible for managing the race and menu scenarios.
/// The menu scenario mostly pertains to character and vehicle selection in the menus prior to
/// starting a race. In Kinoko, we don't have these menus, so we initialize the race directly
/// through this class.
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

    [[nodiscard]] const Scenario &raceScenario() const {
        return m_raceScenario;
    }

    static RaceConfig *CreateInstance();
    static void DestroyInstance();
    [[nodiscard]] static RaceConfig *Instance();

private:
    RaceConfig();
    ~RaceConfig();

    Scenario m_raceScenario;
    RawGhostFile m_ghost;

    static RaceConfig *s_instance; ///< @addr{0x809BD728}
};

} // namespace System
