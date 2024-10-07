#include "KartObjectManager.hh"

#include "game/kart/KartCollide.hh"
#include "game/kart/KartParamFileManager.hh"
#include "game/system/RaceConfig.hh"

namespace Kart {

/// @addr{0x8058FEE0}
void KartObjectManager::init() {
    for (size_t i = 0; i < m_count; ++i) {
        m_objects[i]->initImpl();
        m_objects[i]->prepare();
    }
}

/// @addr{0x8058FFE8}
void KartObjectManager::calc() {
    for (size_t i = 0; i < m_count; ++i) {
        KartObject *object = m_objects[i];
        object->collide()->setMovement(EGG::Vector3f::zero);
    }

    for (size_t i = 0; i < m_count; ++i) {
        KartObject *object = m_objects[i];
        object->calcSub();
        object->calc();
    }
}

/// @addr{0x80590100}
KartObject *KartObjectManager::object(size_t i) const {
    ASSERT(i < m_count);
    return m_objects[i];
}

/// @addr{0x8058FAA8}
KartObjectManager *KartObjectManager::CreateInstance() {
    ASSERT(!s_instance);
    s_instance = new KartObjectManager;
    return s_instance;
}

/// @addr{0x8058FAF8}
void KartObjectManager::DestroyInstance() {
    ASSERT(s_instance);
    delete s_instance;
    s_instance = nullptr;
}

KartObjectManager *KartObjectManager::Instance() {
    return s_instance;
}

/// @addr{0x8058FB2C}
KartObjectManager::KartObjectManager() {
    const auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    m_count = raceScenario.playerCount;
    m_objects = new KartObject *[m_count];
    KartParamFileManager::CreateInstance();
    for (size_t i = 0; i < m_count; ++i) {
        const auto &player = raceScenario.players[i];
        KartObject *object = KartObject::Create(player.character, player.vehicle, i);
        object->createModel();
        m_objects[i] = object;
    }
}

/// @addr{0x8058FDD4}
KartObjectManager::~KartObjectManager() {
    KartParamFileManager::DestroyInstance();

    for (size_t i = 0; i < m_count; ++i) {
        delete m_objects[i];
    }

    delete[] m_objects;
}

KartObjectManager *KartObjectManager::s_instance = nullptr; ///< @addr{0x809C18F8}

} // namespace Kart
