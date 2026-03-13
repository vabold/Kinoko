#pragma once

#include "game/field/jugem/JugemUnit.hh"

namespace Kinoko::Field {

/// @brief Manager class for the lifecycle of Jugem objects for players.
class JugemDirector {
    friend class Host::Context;

public:
    /// @addr{0x8071E638}
    void init() {
        createUnits();
        m_unit->init();
    }

    /// @addr{8071E6C0}
    void calc() {
        m_unit->calc();
    }

    static JugemDirector *CreateInstance();
    [[nodiscard]] static JugemDirector *Instance();
    static void DestroyInstance();

private:
    JugemDirector();
    ~JugemDirector();

    void createUnits();

    JugemUnit *m_unit; ///< Assumes 1 Lakitu because 1 player

    static JugemDirector *s_instance; ///< @addr{0x809C28B8}
};

} // namespace Kinoko::Field
