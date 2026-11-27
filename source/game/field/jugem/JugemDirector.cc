#include "JugemDirector.hh"

#include "game/field/jugem/JugemUnit.hh"

#include "game/field/ObjectDrivableDirector.hh"

#include "game/kart/KartObjectManager.hh"

namespace Kinoko::Field {

/// @addr{0x8071E638}
/// @brief Creates the @ref JugemUnit for the player and initializes it
void JugemDirector::init() {
    createUnits();
    m_unit->init();
}

/// @addr{0x8071E6C0}
/// @brief Updates the @ref JugemUnit for the player
/// @details Lakitu is only implemented because it can refresh the transformation matrix of drivable
/// objects. For Kinoko, we can introduce a performance improvement by skipping Lakitu calculation
/// if there are no drivable objects on the current course.
void JugemDirector::calc() {
    if (ObjectDrivableDirector::Instance()->hasCalcObjects()) {
        m_unit->calc();
    }
}

/// @addr{0x8071E270}
/// @brief Creates the singleton instance of @ref JugemDirector
/// @return A pointer to the newly created singleton instance of @ref JugemDirector
/// @pre The singleton instance must not already exist.
JugemDirector *JugemDirector::CreateInstance() {
    ASSERT(!s_instance);
    s_instance = EGG::egg_new<JugemDirector>();
    return s_instance;
}

/// @addr{0x8071E2FC}
/// @brief Destroys the singleton instance of @ref JugemDirector
/// @pre The singleton instance must exist already.
void JugemDirector::DestroyInstance() {
    ASSERT(s_instance);
    auto *instance = s_instance;
    s_instance = nullptr;
    EGG::egg_delete(instance);
}

/// @addr{0x8071E330}
/// @brief Private constructor
/// @details Initializes @ref m_unit to `nullptr`.
JugemDirector::JugemDirector() : m_unit(nullptr) {}

/// @addr{0x8071E390}
/// @brief Private virtual destructor that destroys the underlying @ref JugemUnit
JugemDirector::~JugemDirector() {
    EGG::egg_delete(m_unit);
}

/// @addr{0x8071E480}
/// @brief Constructs a @ref JugemUnit for the player and its associated @ref JugemSwitch instances
/// @note For Kinoko, we assume there is only one player and thus only one @ref JugemUnit is
/// created.
void JugemDirector::createUnits() {
    m_unit = EGG::egg_new<JugemUnit>(Kart::KartObjectManager::Instance()->object(0));
    m_unit->createSwitchRace();
}

JugemDirector *JugemDirector::s_instance = nullptr;

} // namespace Kinoko::Field
