#pragma once

#include "game/system/KPadController.hh"

namespace System {

// TODO: full implementation
class KPadDirector {
public:
    void calc();
    void calcPads();
    void clear();
    void reset();
    void startGhostProxies();

    const KPadPlayer &playerInput() const;

    void setGhostPad(const u8 *inputs, bool driftIsAuto);

    static KPadDirector *CreateInstance();
    static void DestroyInstance();
    static KPadDirector *Instance();

private:
    KPadDirector();
    ~KPadDirector();

    KPadPlayer m_playerInput;
    KPadGhostController *m_ghostController;

    static KPadDirector *s_instance;
};

} // namespace System
