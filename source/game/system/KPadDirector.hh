#pragma once

#include "game/system/KPadController.hh"

namespace System {

/// @brief The highest level abstraction for controller processing.
/// @addr{0x809BD70C}
class KPadDirector {
public:
    void calc();
    void calcPads();
    void clear();
    void reset();
    void startGhostProxies();
    void endGhostProxies();

    [[nodiscard]] const KPadPlayer &playerInput() const;

    void setGhostPad(const u8 *inputs, bool driftIsAuto);

    static KPadDirector *CreateInstance();
    static void DestroyInstance();
    [[nodiscard]] static KPadDirector *Instance();

private:
    KPadDirector();
    ~KPadDirector();

    KPadPlayer m_playerInput;
    KPadGhostController *m_ghostController;

    static KPadDirector *s_instance; ///< @addr{0x809BD70C}
};

} // namespace System
