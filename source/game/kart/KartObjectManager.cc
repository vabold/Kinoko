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
        KartObject *object = m_objects[i];
        object->calcSub();
        object->calc();
    }
}

KartObject *KartObjectManager::object(size_t i) const {
    assert(i < m_count);
    return m_objects[i];
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
    m_count = raceScenario.playerCount;
    m_objects = new KartObject *[m_count];
    KartParamFileManager::CreateInstance();
    for (size_t i = 0; i < m_count; ++i) {
        const auto &player = raceScenario.players[i];
        m_objects[i] = KartObject::Create(player.character, player.vehicle, i);
    }
}

KartObjectManager::~KartObjectManager() = default;

KartObjectManager *KartObjectManager::s_instance = nullptr;

} // namespace Kart
