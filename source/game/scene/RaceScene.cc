#include "RaceScene.hh"

#include "game/field/CollisionDirector.hh"
#include "game/field/obj/ObjectDirector.hh"
#include "game/item/ItemDirector.hh"
#include "game/kart/KartObjectManager.hh"
#include "game/system/CourseMap.hh"
#include "game/system/KPadDirector.hh"
#include "game/system/RaceConfig.hh"
#include "game/system/RaceManager.hh"
#include "game/system/ResourceManager.hh"

namespace Scene {

RaceScene::RaceScene() = default;

RaceScene::~RaceScene() = default;

void RaceScene::createEngines() {
    System::CourseMap::CreateInstance()->init();
    System::RaceManager::CreateInstance();
    Kart::KartObjectManager::CreateInstance();
    Field::CollisionDirector::CreateInstance();
    Item::ItemDirector::CreateInstance();
    Field::ObjectDirector::CreateInstance();
}

void RaceScene::initEngines() {
    Kart::KartObjectManager::Instance()->init();
    // System::RaceManager::Instance()->init();
    Item::ItemDirector::Instance()->init();
}

void RaceScene::calcEngines() {
    auto *raceMgr = System::RaceManager::Instance();
    raceMgr->calc();
    Kart::KartObjectManager::Instance()->calc();
    Item::ItemDirector::Instance()->calc();
    // raceMgr->random1()->nextU32();
}

void RaceScene::destroyEngines() {
    System::KPadDirector::Instance()->endGhostProxies();
    Kart::KartObjectManager::DestroyInstance();
    Field::ObjectDirector::DestroyInstance();
    Field::CollisionDirector::DestroyInstance();
    Item::ItemDirector::DestroyInstance();
    System::RaceManager::DestroyInstance();
    System::CourseMap::DestroyInstance();
}

void RaceScene::configure() {
    auto *raceCfg = System::RaceConfig::Instance();
    auto *resMgr = System::ResourceManager::Instance();

    raceCfg->initRace();

    auto *commonArc = resMgr->load(0, nullptr);
    appendResource(commonArc, 0);

    auto *courseArc = resMgr->load(raceCfg->raceScenario().course);
    appendResource(courseArc, 1);
}

void RaceScene::onReinit() {
    configure();
}

} // namespace Scene
