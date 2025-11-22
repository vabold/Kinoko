#include "RaceManager.hh"

#include "game/system/CourseMap.hh"
#include "game/system/KPadDirector.hh"
#include "game/system/RaceConfig.hh"
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

/// @addr{0x80533C6C}
void RaceManager::endPlayerRace(u32 /*idx*/) {
    // We only have one player, so most of the logic is much simpler
    m_stage = Stage::FinishGlobal;
}

/// @addr{0x805331B4}
void RaceManager::calc() {
    constexpr u16 STAGE_INTRO_DURATION = 172;

    m_timerManager.calc();
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
            m_timerManager.setStarted(true);
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

/// @addr{0x8053621C}
MapdataJugemPoint *RaceManager::jugemPoint() const {
    s8 jugemId = std::max<s8>(m_player.jugemId(), 0);
    return System::CourseMap::Instance()->getJugemPoint(static_cast<u16>(jugemId));
}

/// @addr{0x80532084}
RaceManager *RaceManager::CreateInstance() {
    ASSERT(!s_instance);
    s_instance = new RaceManager;
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
RaceManager::RaceManager()
    : m_random(RNG_SEED), m_stage(Stage::Intro), m_introTimer(0), m_timer(0) {}

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
    m_raceCompletion = 0.0f;
    m_checkpointFactor = -1.0f;
    m_checkpointStartLapCompletion = 0.0f;
    m_lapCompletion = 0.999999f;

    auto *courseMap = CourseMap::Instance();

    if (courseMap->getCheckPointCount() > 0 && courseMap->getCheckPathCount() > 0) {
        m_maxKcp = courseMap->checkPoint()->lastKcpType();
    } else {
        m_maxKcp = -1;
    }

    m_currentLap = 0;
    m_maxLap = 1;
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

    m_raceCompletion = static_cast<f32>(m_currentLap) +
            (m_checkpointStartLapCompletion + m_checkpointFactor * distanceRatio);
    m_raceCompletion = std::min(m_raceCompletion, static_cast<f32>(m_currentLap) + 0.99999f);
}

/// @addr{0x8053572C}
/// @brief Gets the lap split, which is the difference between the given lap and the previous one.
/// @param lap One-indexed lap.
/// @return The split timer.
Timer RaceManager::Player::getLapSplit(size_t lap) const {
    ASSERT(lap <= m_lapTimers.size());

    if (lap < 2) {
        return m_lapTimers[0];
    }

    const Timer &currentLap = m_lapTimers[lap - 1];
    const Timer &previousLap = m_lapTimers[lap - 2];
    if (!currentLap.valid || !previousLap.valid) {
        return Timer(std::numeric_limits<u16>::max(), 0, 0);
    }

    return currentLap - previousLap;
}

/// @addr{0x80534DF8}
MapdataCheckPoint *RaceManager::Player::calcCheckpoint(u16 checkpointId, f32 distanceRatio) {
    auto *courseMap = CourseMap::Instance();

    u16 oldCheckpointId = m_checkpointId;
    m_checkpointId = checkpointId;

    f32 lapProportion = courseMap->checkPath()->lapProportion();
    MapdataCheckPath *checkPath = courseMap->checkPath()->findCheckpathForCheckpoint(checkpointId);
    m_checkpointFactor = checkPath->oneOverCount() * lapProportion;

    m_checkpointStartLapCompletion = static_cast<f32>(checkPath->depth()) * lapProportion +
            (m_checkpointFactor * static_cast<f32>(checkpointId - checkPath->start()));

    f32 newLapCompletion = m_checkpointStartLapCompletion + distanceRatio * m_checkpointFactor;
    f32 deltaLapCompletion = m_lapCompletion - newLapCompletion;

    MapdataCheckPoint *newCheckpoint = courseMap->getCheckPoint(checkpointId);
    const MapdataCheckPoint *oldCheckpoint = courseMap->getCheckPoint(oldCheckpointId);

    s8 newJugemIdx = newCheckpoint->jugemIndex();
    if (newJugemIdx >= 0) {
        m_jugemId = newJugemIdx;
    }

    if (!newCheckpoint->isNormalCheckpoint()) {
        if (newCheckpoint->checkArea() > m_maxKcp) {
            m_maxKcp = newCheckpoint->checkArea();
        } else if (m_maxKcp == courseMap->checkPoint()->lastKcpType()) {
            if ((newCheckpoint->isFinishLine() &&
                        areCheckpointsSubsequent(oldCheckpoint, checkpointId)) ||
                    deltaLapCompletion > 0.95f) {
                incrementLap();
            }
        }
    }

    if ((oldCheckpoint->isFinishLine() &&
                areCheckpointsSubsequent(newCheckpoint, oldCheckpointId)) ||
            deltaLapCompletion < -0.95f) {
        decrementLap();
    }

    m_lapCompletion = newLapCompletion;

    return newCheckpoint;
}

/// @addr{Inlined in 0x80534DF8}
bool RaceManager::Player::areCheckpointsSubsequent(const MapdataCheckPoint *checkpoint,
        u16 nextCheckpointId) const {
    for (size_t i = 0; i < checkpoint->nextCount(); ++i) {
        if (nextCheckpointId == checkpoint->nextPoint(i)->id()) {
            return true;
        }
    }

    return false;
}

/// @addr{0x80534D6C}
void RaceManager::Player::decrementLap() {
    auto *courseMap = CourseMap::Instance();

    if (courseMap->getCheckPointCount() > 0 && courseMap->getCheckPathCount() > 0) {
        m_maxKcp = courseMap->checkPoint()->lastKcpType();
    } else {
        m_maxKcp = -1;
    }

    --m_currentLap;
}

/// @addr{0x805349B8}
void RaceManager::Player::incrementLap() {
    m_maxKcp = 0;
    if (++m_currentLap <= m_maxLap) {
        return;
    }

    const auto *kart = Kart::KartObjectManager::Instance()->object(0);
    u16 addMs = CourseMap::Instance()->getCheckPointEntryOffsetMs(m_checkpointId, kart->pos(),
            kart->prevPos());

    const Timer &currentTimer = RaceManager::Instance()->timerManager().currentTimer();
    Timer timer = currentTimer + static_cast<f32>(addMs);

    // TODO: Handle this case more gracefully
    ASSERT(static_cast<size_t>(m_maxLap - 1) < m_lapTimers.size());
    m_lapTimers[m_maxLap - 1] = timer;

    if (m_maxLap >= 3) {
        endRace(timer);
    } else {
        m_maxLap = m_currentLap;
    }
}

/// @addr{0x805347F4}
void RaceManager::Player::endRace(const Timer &finishTime) {
    m_raceTimer = finishTime;
    RaceManager::Instance()->endPlayerRace(0);
}

RaceManager *RaceManager::s_instance = nullptr; ///< @addr{0x809BD730}

} // namespace System
