#pragma once

#include <egg/math/Vector.hh>

namespace System {

class RaceManager {
public:
    void findKartStartPoint(EGG::Vector3f &pos, EGG::Vector3f &angles);

    static RaceManager *CreateInstance();
    static RaceManager *Instance();
    static void DestroyInstance();

private:
    RaceManager();
    ~RaceManager();

    static RaceManager *s_instance;
};

} // namespace System
