#pragma once

#include <Common.hh>

class HostSystem {
public:
    void create();
    void init();
    void calc();

    static HostSystem *CreateInstance();
    static void DestroyInstance();
    static HostSystem *Instance();

private:
    HostSystem();
    ~HostSystem();

    static HostSystem *s_instance;
};
