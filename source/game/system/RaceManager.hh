#pragma once

#include "game/system/KPadController.hh"
#include "game/system/map/MapdataCheckPoint.hh"
#include "game/system/map/MapdataJugemPoint.hh"

#include <egg/math/Vector.hh>

namespace System {

/// @addr{0x809BD730}
/// @brief Manages the timers that track the stages of a race.
/// Also acts as the interface between the physics engine and CourseMap.
/// @details The physics engine leverages the RaceManager in order to determine what stage of the
/// race we're in, as that affects several things like acceleration. This class also retrieves the
/// player start position from CourseMap and communicates it to the physics engine.
/// @nosubgrouping
class RaceManager : EGG::Disposer {
public:
    class Player {
    public:
        Player();
        virtual ~Player() {}

        void init();
        void calc();

        [[nodiscard]] Timer getLapSplit(size_t idx) const;

        /// @beginGetters
        [[nodiscard]] u16 checkpointId() const;
        [[nodiscard]] f32 raceCompletion() const;
        [[nodiscard]] s8 jugemId() const;
        [[nodiscard]] const std::array<Timer, 3> &lapTimers() const;
        [[nodiscard]] const Timer &lapTimer(size_t idx) const;
        [[nodiscard]] const Timer &raceTimer() const;
        [[nodiscard]] const KPad *inputs() const;
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

    [[nodiscard]] bool isStageReached(Stage stage) const;
    [[nodiscard]] MapdataJugemPoint *jugemPoint() const;

    /// @beginGetters
    [[nodiscard]] int getCountdownTimer() const;
    [[nodiscard]] const Player &player() const;
    [[nodiscard]] const TimerManager &timerManager() const;
    [[nodiscard]] Stage stage() const;
    [[nodiscard]] u32 timer() const;
    /// @endGetters

    static RaceManager *CreateInstance();
    [[nodiscard]] static RaceManager *Instance();
    static void DestroyInstance();

private:
    RaceManager();
    ~RaceManager() override;

    Player m_player;
    TimerManager m_timerManager;
    Stage m_stage;
    u16 m_introTimer;
    u32 m_timer;

    static constexpr u16 STAGE_COUNTDOWN_DURATION = 240;

    static RaceManager *s_instance; ///< @addr{0x809BD730}
};

} // namespace System
