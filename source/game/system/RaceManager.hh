#pragma once

#include "game/system/KPadController.hh"
#include "game/system/Random.hh"
#include "game/system/map/MapdataCheckPoint.hh"
#include "game/system/map/MapdataJugemPoint.hh"

#include <egg/math/Vector.hh>

class SavestateManager;

namespace System {

/// @addr{0x809BD730}
/// @brief Manages the timers that track the stages of a race.
/// Also acts as the interface between the physics engine and CourseMap.
/// @details The physics engine leverages the RaceManager in order to determine what stage of the
/// race we're in, as that affects several things like acceleration. This class also retrieves the
/// player start position from CourseMap and communicates it to the physics engine.
/// @nosubgrouping
class RaceManager : EGG::Disposer {
    friend class ::SavestateManager;

public:
    class Player {
    public:
        Player();
        virtual ~Player() {}

        void init();
        void calc();

        [[nodiscard]] Timer getLapSplit(size_t idx) const;

        /// @beginGetters
        [[nodiscard]] u16 checkpointId() const {
            return m_checkpointId;
        }

        [[nodiscard]] f32 raceCompletion() const {
            return m_raceCompletion;
        }

        [[nodiscard]] s8 jugemId() const {
            return m_jugemId;
        }

        [[nodiscard]] const std::array<Timer, 3> &lapTimers() const {
            return m_lapTimers;
        }

        [[nodiscard]] const Timer &lapTimer(size_t idx) const {
            ASSERT(idx < m_lapTimers.size());
            return m_lapTimers[idx];
        }

        [[nodiscard]] const Timer &raceTimer() const {
            return m_raceTimer;
        }

        [[nodiscard]] const KPad *inputs() const {
            return m_inputs;
        }
        /// @endGetters

    private:
        MapdataCheckPoint *calcCheckpoint(u16 checkpointId, f32 distanceRatio);
        [[nodiscard]] bool areCheckpointsSubsequent(const MapdataCheckPoint *checkpoint,
                u16 nextCheckpointId) const;

        void decrementLap();
        void incrementLap();
        void endRace(const Timer &finishTime);

        u16 m_checkpointId;
        f32 m_raceCompletion;
        f32 m_checkpointFactor; ///< The proportion of a lap for the current checkpoint
        f32 m_checkpointStartLapCompletion;
        f32 m_lapCompletion;
        s8 m_jugemId;
        s16 m_currentLap;
        s8 m_maxLap;
        s8 m_maxKcp;
        std::array<Timer, 3> m_lapTimers;
        Timer m_raceTimer;
        const KPad *m_inputs;
    };

    enum class Stage {
        Intro = 0,
        Countdown = 1,
        Race = 2,
        FinishLocal = 3,
        FinishGlobal = 4,
    };

    void init();

    void findKartStartPoint(EGG::Vector3f &pos, EGG::Vector3f &angles);
    void endPlayerRace(u32 idx);

    void calc();

    /// @addr{0x80536230}
    [[nodiscard]] bool isStageReached(Stage stage) const {
        return static_cast<std::underlying_type_t<Stage>>(m_stage) >=
                static_cast<std::underlying_type_t<Stage>>(stage);
    }

    [[nodiscard]] MapdataJugemPoint *jugemPoint() const;

    /// @beginGetters
    /// @addr{0x80533090}
    [[nodiscard]] int getCountdownTimer() const {
        return STAGE_COUNTDOWN_DURATION - m_timer;
    }

    [[nodiscard]] const Player &player() const {
        return m_player;
    }

    [[nodiscard]] const TimerManager &timerManager() const {
        return m_timerManager;
    }

    [[nodiscard]] Stage stage() const {
        return m_stage;
    }

    [[nodiscard]] u32 timer() const {
        return m_timer;
    }
    /// @endGetters

    static RaceManager *CreateInstance();
    static void DestroyInstance();

    [[nodiscard]] static RaceManager *Instance() {
        return s_instance;
    }

private:
    RaceManager();
    ~RaceManager() override;

    Random m_random;
    Player m_player;
    TimerManager m_timerManager;
    Stage m_stage;
    u16 m_introTimer;
    u32 m_timer;

    static constexpr u16 STAGE_COUNTDOWN_DURATION = 240;
    static constexpr u32 RNG_SEED = 0x74A1B095;

    static RaceManager *s_instance; ///< @addr{0x809BD730}
};

} // namespace System
