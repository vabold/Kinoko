#pragma once

#include "game/system/KPadController.hh"

#include <egg/math/Vector.hh>

namespace System {

class RaceManagerPlayer {
public:
    RaceManagerPlayer();
    virtual ~RaceManagerPlayer() {}

    const KPad *inputs() const;

private:
    const KPad *m_inputs;
};

class RaceManager {
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

    const RaceManagerPlayer &player() const;
    Stage stage() const;

    static RaceManager *CreateInstance();
    static RaceManager *Instance();
    static void DestroyInstance();

private:
    RaceManager();
    ~RaceManager();

    RaceManagerPlayer m_player;
    Stage m_stage;
    u16 m_introTimer;
    u32 m_timer;

    static RaceManager *s_instance;
};

} // namespace System
