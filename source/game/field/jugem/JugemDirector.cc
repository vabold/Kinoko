#include "JugemDirector.hh"

#include "game/field/ObjectDrivableDirector.hh"

#include "game/kart/KartObjectManager.hh"

namespace Kinoko::Field {

/// @addr{8071E6C0}
/// @details Lakitu is only implemented because it can refresh the transformation matrix of drivable
/// objects. For Kinoko, we can introduce a performance improvement by skipping Lakitu calculation
/// if there are no drivable objects on the current course.
void JugemDirector::calc() {
    if (!ObjectDrivableDirector::Instance()->hasCalcObjects()) {
        return;
    }

    m_unit->calc();
}

/// @addr{0x8071E270}
JugemDirector *JugemDirector::CreateInstance() {
    ASSERT(!s_instance);
    s_instance = EGG::egg_new<JugemDirector>();
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
    EGG::egg_delete(instance);
}

/// @addr{0x8071E330}
JugemDirector::JugemDirector() : m_unit(nullptr) {}

/// @addr{0x8071E390}
JugemDirector::~JugemDirector() {
    EGG::egg_delete(m_unit);
}

/// @addr{0x8071E480}
void JugemDirector::createUnits() {
    // Assumes one unit
    const auto *kartObj = Kart::KartObjectManager::Instance()->object(0);
    m_unit = EGG::egg_new<JugemUnit>(kartObj);

    m_unit->createSwitchRace();
}

JugemDirector *JugemDirector::s_instance = nullptr; ///< @addr{0x809C28B8}

} // namespace Kinoko::Field
