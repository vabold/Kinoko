#pragma once

#include "game/system/KPadController.hh"

namespace System {

/// @brief The highest level abstraction for controller processing.
/// @addr{0x809BD70C}
class KPadDirector : EGG::Disposer {
public:
    void calc();
    void calcPads();
    void clear();
    void reset();
    void startGhostProxies();
    void endGhostProxies();

    [[nodiscard]] const KPadPlayer &playerInput() const;
    [[nodiscard]] KPadHostController *hostController();

    void setGhostPad(const u8 *inputs, bool driftIsAuto);
    void setHostPad(bool driftIsAuto);

    static KPadDirector *CreateInstance();
    static void DestroyInstance();
    [[nodiscard]] static KPadDirector *Instance();

private:
    KPadDirector();
    ~KPadDirector() override;

    KPadPlayer m_playerInput;
    KPadGhostController *m_ghostController;
    KPadHostController *m_hostController;

    static KPadDirector *s_instance; ///< @addr{0x809BD70C}
};

} // namespace System
