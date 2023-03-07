#include "KartObjectManager.hh"

#include "source/game/system/RaceConfig.hh"

namespace Kart {

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
    auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    m_count = raceScenario.playerCount();
    m_objects = new KartObject *[m_count];
    for (size_t i = 0; i < m_count; ++i) {
        auto &player = raceScenario.player(i);
        m_objects[i] = KartObject::Create(player.character(), player.vehicle());
    }
}

KartObjectManager::~KartObjectManager() = default;

KartObjectManager *KartObjectManager::s_instance = nullptr;

} // namespace Kart
