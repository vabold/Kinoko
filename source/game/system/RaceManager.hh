#pragma once

#include <egg/math/Vector.hh>

namespace System {

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

    Stage stage() const;

    static RaceManager *CreateInstance();
    static RaceManager *Instance();
    static void DestroyInstance();

private:
    RaceManager();
    ~RaceManager();

    Stage m_stage;
    u32 m_introTimer;

    static RaceManager *s_instance;
};

} // namespace System
