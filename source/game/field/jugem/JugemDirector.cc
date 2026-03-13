#include "JugemDirector.hh"

#include "game/kart/KartObjectManager.hh"

namespace Kinoko::Field {

/// @addr{0x8071E270}
JugemDirector *JugemDirector::CreateInstance() {
    ASSERT(!s_instance);
    s_instance = new JugemDirector;
    return s_instance;
}

/// @addr{0x809C28B8}
JugemDirector *JugemDirector::Instance() {
    return s_instance;
}

/// @addr{0x8071E2FC}
void JugemDirector::DestroyInstance() {
    ASSERT(s_instance);
    auto *instance = s_instance;
    s_instance = nullptr;
    delete instance;
}

/// @addr{0x8071E330}
JugemDirector::JugemDirector() : m_unit(nullptr) {}

/// @addr{0x8071E390}
JugemDirector::~JugemDirector() {
    delete m_unit;
}

/// @addr{0x8071E480}
void JugemDirector::createUnits() {
    // Assumes one unit
    const auto *kartObj = Kart::KartObjectManager::Instance()->object(0);
    m_unit = new JugemUnit(kartObj);

    m_unit->createSwitchRace();
}

JugemDirector *JugemDirector::s_instance = nullptr; ///< @addr{0x809C28B8}

} // namespace Kinoko::Field
