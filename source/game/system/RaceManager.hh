#pragma once

#include "game/system/KPadController.hh"

#include <egg/math/Vector.hh>

namespace System {

class RaceManagerPlayer {
public:
    RaceManagerPlayer();
    virtual ~RaceManagerPlayer() {}

    [[nodiscard]] const KPad *inputs() const;

private:
    const KPad *m_inputs;
};

/// @addr{0x809BD730}
/// @brief Manages the timers that track the stages of a race.
/// Also acts as the interface between the physics engine and CourseMap.
/// @details The physics engine leverages the RaceManager in order to determine what stage of the
/// race we're in, as that affects several things like acceleration. This class also retrieves the
/// player start position from CourseMap and communicates it to the physics engine.
/// @nosubgrouping
class RaceManager : EGG::Disposer {
public:
    enum class Stage {
        Intro = 0,
        Countdown = 1,
        Race = 2,
        FinishLocal = 3,
        FinishGlobal = 4,
    };

    void findKartStartPoint(EGG::Vector3f &pos, EGG::Vector3f &angles);

    void calc();

    [[nodiscard]] bool isStageReached(Stage stage) const;

    /// @beginGetters
    [[nodiscard]] int getCountdownTimer() const;
    [[nodiscard]] const RaceManagerPlayer &player() const;
    [[nodiscard]] Stage stage() const;
    /// @endGetters

    static RaceManager *CreateInstance();
    [[nodiscard]] static RaceManager *Instance();
    static void DestroyInstance();

private:
    RaceManager();
    ~RaceManager() override;

    RaceManagerPlayer m_player;
    Stage m_stage;
    u16 m_introTimer;
    u32 m_timer;

    static constexpr u16 STAGE_COUNTDOWN_DURATION = 240;

    static RaceManager *s_instance; ///< @addr{0x809BD730}
};

} // namespace System
