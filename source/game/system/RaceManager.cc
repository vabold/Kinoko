#include "RaceManager.hh"

#include "game/system/CourseMap.hh"
#include "game/system/KPadDirector.hh"
#include "game/system/map/MapdataCheckPath.hh"
#include "game/system/map/MapdataStartPoint.hh"

#include "game/kart/KartObjectManager.hh"
#include "game/kart/KartState.hh"

namespace System {

/// @addr{0x80532F88}
void RaceManager::init() {
    m_player.init();
}

/// @addr{0x805362DC}
/// @todo When expanding to other gamemodes, we will need to pass the player index
void RaceManager::findKartStartPoint(EGG::Vector3f &pos, EGG::Vector3f &angles) {
    u32 placement = 1;
    u32 playerCount = 1;
    u32 startPointIdx = 0;

    MapdataStartPoint *kartpoint = CourseMap::Instance()->getStartPoint(startPointIdx);

    if (kartpoint) {
        kartpoint->findKartStartPoint(pos, angles, placement - 1, playerCount);
    } else {
        pos.setZero();
        angles = EGG::Vector3f::ex;
    }
}

/// @addr{0x805331B4}
void RaceManager::calc() {
    constexpr u16 STAGE_INTRO_DURATION = 172;

    m_player.calc();

    switch (m_stage) {
    case Stage::Intro:
        if (++m_introTimer >= STAGE_INTRO_DURATION) {
            m_stage = Stage::Countdown;
            KPadDirector::Instance()->startGhostProxies();
        }
        break;
    case Stage::Countdown:
        if (++m_timer >= STAGE_COUNTDOWN_DURATION) {
            m_stage = Stage::Race;
        }
        break;
    case Stage::Race:
        ++m_timer;
        break;
    default:
        break;
    }
}

/// @addr{0x80536230}
bool RaceManager::isStageReached(Stage stage) const {
    return static_cast<std::underlying_type_t<Stage>>(m_stage) >=
            static_cast<std::underlying_type_t<Stage>>(stage);
}

/// @addr{0x80533090}
int RaceManager::getCountdownTimer() const {
    return STAGE_COUNTDOWN_DURATION - m_timer;
}

const RaceManager::Player &RaceManager::player() const {
    return m_player;
}

RaceManager::Stage RaceManager::stage() const {
    return m_stage;
}

/// @addr{0x80532084}
RaceManager *RaceManager::CreateInstance() {
    ASSERT(!s_instance);
    s_instance = new RaceManager;
    return s_instance;
}

RaceManager *RaceManager::Instance() {
    return s_instance;
}

/// @addr{0x805320D4}
void RaceManager::DestroyInstance() {
    ASSERT(s_instance);
    auto *instance = s_instance;
    s_instance = nullptr;
    delete instance;
}

/// @addr{0x805327A0}
RaceManager::RaceManager() : m_stage(Stage::Intro), m_introTimer(0), m_timer(0) {}

/// @addr{0x80532E3C}
RaceManager::~RaceManager() {
    if (s_instance) {
        s_instance = nullptr;
        WARN("RaceManager instance not explicitly handled!");
    }
}

/// @addr{0x80533ED8}
RaceManager::Player::Player() {
    m_checkpointId = 0;
    m_checkpointFactor = -1.0f;
    m_inputs = &KPadDirector::Instance()->playerInput();
}

/// @addr{0x80534194}
void RaceManager::Player::init() {
    auto *courseMap = CourseMap::Instance();

    if (courseMap->getCheckPointCount() != 0 && courseMap->getCheckPathCount() != 0) {
        const EGG::Vector3f &pos = Kart::KartObjectManager::Instance()->object(0)->pos();
        f32 distanceRatio;
        s16 checkpointId = courseMap->findSector(pos, 0, distanceRatio);

        m_checkpointId = std::max<s16>(0, checkpointId);
        m_jugemId = courseMap->getCheckPoint(m_checkpointId)->jugemIndex();
    } else {
        m_jugemId = 0;
    }
}

/// @addr{0x80535304}
void RaceManager::Player::calc() {
    auto *courseMap = CourseMap::Instance();
    const auto *kart = Kart::KartObjectManager::Instance()->object(0);

    if (courseMap->getCheckPointCount() == 0 || courseMap->getCheckPathCount() == 0 ||
            kart->state()->isBeforeRespawn()) {
        return;
    }

    f32 distanceRatio;
    s16 checkpointId = courseMap->findSector(kart->pos(), m_checkpointId, distanceRatio);

    if (checkpointId == -1) {
        return;
    }

    if (m_checkpointFactor < 0.0f || m_checkpointId != checkpointId) {
        calcCheckpoint(checkpointId, distanceRatio);
    }
}

const KPad *RaceManager::Player::inputs() const {
    return m_inputs;
}

/// @addr{0x80534DF8}
MapdataCheckPoint *RaceManager::Player::calcCheckpoint(u16 checkpointId, f32 /*distanceRatio*/) {
    auto *courseMap = CourseMap::Instance();
    m_checkpointId = checkpointId;
    MapdataCheckPath *checkPath = courseMap->checkPath()->findCheckpathForCheckpoint(checkpointId);
    m_checkpointFactor = checkPath->oneOverCount() * courseMap->checkPath()->lapProportion();

    return courseMap->getCheckPoint(checkpointId);
}

/// @addr{Inlined in 0x80534DF8}
bool RaceManager::Player::areCheckpointsSubsequent(MapdataCheckPoint *checkpoint,
        u16 nextCheckpointId) const {
    for (size_t i = 0; i < checkpoint->nextCount(); ++i) {
        if (nextCheckpointId == checkpoint->nextPoint(i)->id()) {
            return true;
        }
    }

    return false;
}

RaceManager *RaceManager::s_instance = nullptr; ///< @addr{0x809BD730}

} // namespace System
