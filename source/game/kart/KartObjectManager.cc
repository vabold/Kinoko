#include "KartObjectManager.hh"

#include "game/kart/KartParamFileManager.hh"
#include "game/system/RaceConfig.hh"

namespace Kart {

void KartObjectManager::init() {
    for (size_t i = 0; i < m_count; ++i) {
        m_objects[i]->initImpl();
        m_objects[i]->prepare();
    }
}

void KartObjectManager::calc() {
    for (size_t i = 0; i < m_count; ++i) {
        m_objects[i]->calcSub();
    }
}

KartObjectManager *KartObjectManager::CreateInstance() {
    assert(!s_instance);
    s_instance = new KartObjectManager;
    return s_instance;
}

void KartObjectManager::DestroyInstance() {
    assert(s_instance);
    delete s_instance;
    s_instance = nullptr;
}

KartObjectManager *KartObjectManager::Instance() {
    return s_instance;
}

KartObjectManager::KartObjectManager() {
    const auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    m_count = raceScenario.m_playerCount;
    m_objects = new KartObject *[m_count];
    KartParamFileManager::CreateInstance();
    for (size_t i = 0; i < m_count; ++i) {
        const auto &player = raceScenario.m_players[i];
        m_objects[i] = KartObject::Create(player.m_character, player.m_vehicle, i);
    }
}

KartObjectManager::~KartObjectManager() = default;

KartObjectManager *KartObjectManager::s_instance = nullptr;

} // namespace Kart
