#pragma once

#include <Common.hh>

namespace Host {

class System {
public:
    void init();

    static System *CreateInstance();
    static void DestroyInstance();
    static System *Instance();

private:
    System();
    ~System();

    static System *s_instance;
};

} // namespace Host
