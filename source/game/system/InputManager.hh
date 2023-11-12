#pragma once

#include <Common.hh>

namespace System {

// TODO: full implementation
class InputManager {
public:
    void calc();
    void clear();
    void init();

    void setGhostPad();

    static InputManager *CreateInstance();
    static void DestroyInstance();
    static InputManager *Instance();

private:
    InputManager();
    ~InputManager();

    static InputManager *s_instance;
};

} // namespace System
