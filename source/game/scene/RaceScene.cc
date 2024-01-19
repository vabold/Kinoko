#include "RaceScene.hh"

#include "game/field/CollisionDirector.hh"
#include "game/kart/KartObjectManager.hh"
#include "game/system/CourseMap.hh"
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
    // Item::ItemDirector::CreateInstance();
}

void RaceScene::initEngines() {
    Kart::KartObjectManager::Instance()->init();
    // System::RaceManager::Instance()->init();
    // Item::ItemDirector::Instance()->init();
}

void RaceScene::calcEngines() {
    // auto *raceMgr = System::RaceManager::Instance();
    // raceMgr->calc();
    // if (!raceMgr->spectatorMode())
    Kart::KartObjectManager::Instance()->calc();
    // if (raceMgr->isStateReached(State::Countdown)) {
    //     Item::ItemDirector::Instance()->calc();
    // }
    // raceMgr->random1()->nextU32();
}

void RaceScene::destroyEngines() {
    Kart::KartObjectManager::DestroyInstance();
    Field::CollisionDirector::DestroyInstance();
    // Item::ItemDirector::DestroyInstance();
    System::RaceManager::DestroyInstance();
    System::CourseMap::DestroyInstance();
}

void RaceScene::configure() {
    auto *raceCfg = System::RaceConfig::Instance();
    auto *resMgr = System::ResourceManager::Instance();

    raceCfg->initRace();

    auto *commonArc = resMgr->load(0, nullptr);
    appendResource(commonArc, 0);

    auto *courseArc = resMgr->load(raceCfg->raceScenario().m_course);
    appendResource(courseArc, 1);
}

void RaceScene::onReinit() {
    System::RaceConfig::Instance()->initRace();
}

} // namespace Scene
