#include "RaceScene.hh"

#include "game/field/BoxColManager.hh"
#include "game/field/CollisionDirector.hh"
#include "game/field/ObjectDirector.hh"
#include "game/item/ItemDirector.hh"
#include "game/kart/KartObjectManager.hh"
#include "game/system/CourseMap.hh"
#include "game/system/KPadDirector.hh"
#include "game/system/RaceConfig.hh"
#include "game/system/RaceManager.hh"
#include "game/system/ResourceManager.hh"

namespace Scene {

/// @addr{0x80553B88}
RaceScene::RaceScene() {
    m_heap->setName("RaceSceneHeap");
}

/// @addr{0x80553BD4}
RaceScene::~RaceScene() = default;

/// @addr{0x80554208}
void RaceScene::createEngines() {
    System::CourseMap::CreateInstance()->init();
    System::RaceManager::CreateInstance();
    Field::BoxColManager::CreateInstance();
    Kart::KartObjectManager::CreateInstance();
    Field::CollisionDirector::CreateInstance();
    Item::ItemDirector::CreateInstance();
    Field::ObjectDirector::CreateInstance();
}

/// @addr{0x8055472C}
void RaceScene::initEngines() {
    Kart::KartObjectManager::Instance()->init();
    // System::RaceManager::Instance()->init();
    Item::ItemDirector::Instance()->init();
    Field::ObjectDirector::Instance()->init();
}

/// @addr{0x80554E6C}
/// @details In Kinoko, it is not possible to pause the race scene, so Kinoko's implementation for
/// this function is really the base game's `calcEnginesUnpaused` located at `0x80554AD4`.
void RaceScene::calcEngines() {
    auto *raceMgr = System::RaceManager::Instance();
    raceMgr->calc();
    Field::BoxColManager::Instance()->calc();
    Field::ObjectDirector::Instance()->calc();
    Kart::KartObjectManager::Instance()->calc();
    Item::ItemDirector::Instance()->calc();
    // raceMgr->random1()->nextU32();
}

/// @addr{0x805549B0}
void RaceScene::destroyEngines() {
    System::KPadDirector::Instance()->endGhostProxies();
    Kart::KartObjectManager::DestroyInstance();
    Field::ObjectDirector::DestroyInstance();
    Field::CollisionDirector::DestroyInstance();
    Item::ItemDirector::DestroyInstance();
    Field::BoxColManager::DestroyInstance();
    System::RaceManager::DestroyInstance();
    System::CourseMap::DestroyInstance();
}

/// @brief Retrieves Common.szs and the course archive.
/// @addr{0x80553C50}
void RaceScene::configure() {
    auto *raceCfg = System::RaceConfig::Instance();
    auto *resMgr = System::ResourceManager::Instance();

    raceCfg->initRace();

    auto *commonArc = resMgr->load(0, nullptr);
    appendResource(commonArc, 0);

    auto *courseArc = resMgr->load(raceCfg->raceScenario().course);
    appendResource(courseArc, 1);
}

/// @brief This is called on race shutdown in order to prep for the next race.
/// @addr{0x80554A94}
void RaceScene::onReinit() {
    configure();
}

} // namespace Scene
