#pragma once

#include "game/system/KPadController.hh"

class SavestateManager;

namespace System {

/// @brief The highest level abstraction for controller processing.
/// @addr{0x809BD70C}
class KPadDirector : EGG::Disposer {
    friend class ::SavestateManager;

public:
    void calc();
    void calcPads();

    /// @addr{0x80523724}
    void clear() {}

    void reset();
    void startGhostProxies();
    void endGhostProxies();

    [[nodiscard]] const KPadPlayer &playerInput() const {
        return m_playerInput;
    }

    [[nodiscard]] KPadHostController *hostController() {
        return m_hostController;
    }

    void setGhostPad(const u8 *inputs, bool driftIsAuto);
    void setHostPad(bool driftIsAuto);

    static KPadDirector *CreateInstance();
    static void DestroyInstance();

    [[nodiscard]] static KPadDirector *Instance() {
        return s_instance;
    }

private:
    KPadDirector();
    ~KPadDirector() override;

    KPadPlayer m_playerInput;
    KPadGhostController *m_ghostController;
    KPadHostController *m_hostController;

    static KPadDirector *s_instance; ///< @addr{0x809BD70C}
};

} // namespace System
