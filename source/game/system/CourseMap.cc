#include "CourseMap.hh"

#include "game/system/map/MapdataCannonPoint.hh"
#include "game/system/map/MapdataCheckPath.hh"
#include "game/system/map/MapdataCheckPoint.hh"
#include "game/system/map/MapdataFileAccessor.hh"
#include "game/system/map/MapdataGeoObj.hh"
#include "game/system/map/MapdataJugemPoint.hh"
#include "game/system/map/MapdataPointInfo.hh"
#include "game/system/map/MapdataStageInfo.hh"
#include "game/system/map/MapdataStartPoint.hh"

#include "game/system/ResourceManager.hh"

namespace System {

/// @addr{0x805127EC}
void CourseMap::init() {
    void *buffer = LoadFile("course.kmp");
    m_course =
            new MapdataFileAccessor(reinterpret_cast<const MapdataFileAccessor::SData *>(buffer));

    constexpr u32 CANNON_POINT_SIGNATURE = 0x434e5054;
    constexpr u32 CHECK_PATH_SIGNATURE = 0x434b5048;
    constexpr u32 CHECK_POINT_SIGNATURE = 0x434b5054;
    constexpr u32 GEO_OBJ_SIGNATURE = 0x474f424a;
    constexpr u32 JUGEM_POINT_SIGNATURE = 0x4a475054;
    constexpr u32 START_POINT_SIGNATURE = 0x4b545054;
    constexpr u32 POINT_INFO_SIGNATURE = 0x504f5449;
    constexpr u32 STAGE_INFO_SIGNATURE = 0x53544749;

    m_startPoint = parseStartPoint(START_POINT_SIGNATURE);
    m_checkPath = parseCheckPath(CHECK_PATH_SIGNATURE);
    m_checkPoint = parseCheckPoint(CHECK_POINT_SIGNATURE);
    m_geoObj = parseGeoObj(GEO_OBJ_SIGNATURE);
    m_pointInfo = parsePointInfo(POINT_INFO_SIGNATURE);
    m_jugemPoint = parseJugemPoint(JUGEM_POINT_SIGNATURE);
    m_cannonPoint = parseCannonPoint(CANNON_POINT_SIGNATURE);
    m_stageInfo = parseStageInfo(STAGE_INFO_SIGNATURE);

    MapdataStageInfo *stageInfo = getStageInfo();
    constexpr u8 TRANSLATION_MODE_NARROW = 1;
    if (stageInfo && stageInfo->translationMode() == TRANSLATION_MODE_NARROW) {
        m_startTmpAngle = 25.0f;
        m_startTmp2 = 250.0f;
        m_startTmp3 = 0.0f;
    } else {
        m_startTmpAngle = 30.0f;
        m_startTmp2 = 400.0f;
        m_startTmp3 = 100.0f;
    }

    m_startTmp0 = 800.0f;
    m_startTmp1 = 1200.0f;
}

/// @addr{0x80512FA4}
MapdataCannonPointAccessor *CourseMap::parseCannonPoint(u32 sectionName) const {
    const MapSectionHeader *sectionPtr = m_course->findSection(sectionName);

    MapdataCannonPointAccessor *accessor = nullptr;
    if (sectionPtr) {
        accessor = new MapdataCannonPointAccessor(sectionPtr);
    }

    return accessor;
}

/// @addr{0x8051377C}
MapdataCheckPathAccessor *CourseMap::parseCheckPath(u32 sectionName) const {
    const MapSectionHeader *sectionPtr = m_course->findSection(sectionName);

    MapdataCheckPathAccessor *accessor = nullptr;
    if (sectionPtr) {
        accessor = new MapdataCheckPathAccessor(sectionPtr);
    }

    return accessor;
}

/// @addr{0x80513640}
MapdataCheckPointAccessor *CourseMap::parseCheckPoint(u32 sectionName) const {
    const MapSectionHeader *sectionPtr = m_course->findSection(sectionName);

    MapdataCheckPointAccessor *accessor = nullptr;
    if (sectionPtr) {
        accessor = new MapdataCheckPointAccessor(sectionPtr);
    }

    return accessor;
}

/// @addr{0x805134C8}
MapdataGeoObjAccessor *CourseMap::parseGeoObj(u32 sectionName) const {
    const MapSectionHeader *sectionPtr = m_course->findSection(sectionName);

    MapdataGeoObjAccessor *accessor = nullptr;
    if (sectionPtr) {
        accessor = new MapdataGeoObjAccessor(sectionPtr);
    }

    return accessor;
}

/// @addr{0x805130C4}
MapdataJugemPointAccessor *CourseMap::parseJugemPoint(u32 sectionName) const {
    const MapSectionHeader *sectionPtr = m_course->findSection(sectionName);

    MapdataJugemPointAccessor *accessor = nullptr;
    if (sectionPtr) {
        accessor = new MapdataJugemPointAccessor(sectionPtr);
    }

    return accessor;
}

/// @addr{0x80513398}
MapdataPointInfoAccessor *CourseMap::parsePointInfo(u32 sectionName) const {
    const MapSectionHeader *sectionPtr = m_course->findSection(sectionName);

    MapdataPointInfoAccessor *accessor = nullptr;
    if (sectionPtr) {
        accessor = new MapdataPointInfoAccessor(sectionPtr);
    }

    return accessor;
}

/// @addr{0x80512D64}
MapdataStageInfoAccessor *CourseMap::parseStageInfo(u32 sectionName) const {
    const MapSectionHeader *sectionPtr = m_course->findSection(sectionName);

    MapdataStageInfoAccessor *accessor = nullptr;
    if (sectionPtr) {
        accessor = new MapdataStageInfoAccessor(sectionPtr);
    }

    return accessor;
}

/// @addr{0x80513F5C}
MapdataStartPointAccessor *CourseMap::parseStartPoint(u32 sectionName) const {
    const MapSectionHeader *sectionPtr = m_course->findSection(sectionName);

    MapdataStartPointAccessor *accessor = nullptr;
    if (sectionPtr) {
        accessor = new MapdataStartPointAccessor(sectionPtr);
    }

    return accessor;
}

/// @addr{0x80511500}
s16 CourseMap::findSector(const EGG::Vector3f &pos, u16 checkpointIdx, f32 &distanceRatio) {
    clearSectorChecked();

    MapdataCheckPoint *checkpoint = getCheckPoint(checkpointIdx);
    s16 id = -1;

    MapdataCheckPoint::SectorOccupancy occupancy =
            checkpoint->checkSectorAndDistanceRatio(pos, distanceRatio);
    checkpoint->setSearched();

    switch (occupancy) {
    // The player is fully inside the current checkpoint, so just set to current checkpoint
    case MapdataCheckPoint::SectorOccupancy::InsideSector:
        id = checkpoint->id();
        break;

    // The player is between the sides of the quad, but NOT between this checkpoint and
    // next; player is likely in the same checkpoint group
    case MapdataCheckPoint::SectorOccupancy::BetweenSides:
        id = findSectorBetweenSides(pos, checkpoint, distanceRatio);
        break;

    // The player is not between the sides of the quad (may still be between this checkpoint
    // and next); player is likely in a different checkpoint group
    case MapdataCheckPoint::SectorOccupancy::OutsideSector:
        id = findSectorOutsideSector(pos, checkpoint, distanceRatio);
        break;

    default:
        break;
    }

    return id > -1 ? id : findSectorRegional(pos, checkpoint, distanceRatio);
}

/// @addr{0x80511110}
s16 CourseMap::findRecursiveSector(const EGG::Vector3f &pos, s16 depth, bool searchBackwardsFirst,
        MapdataCheckPoint *checkpoint, f32 &distanceRatio, bool playerIsForwards) const {
    constexpr s16 MAX_DEPTH = 6;

    if (depth >= 0 && depth > MAX_DEPTH) {
        return -1;
    }

    MapdataCheckPoint::SectorOccupancy completion =
            MapdataCheckPoint::SectorOccupancy::OutsideSector;

    if (!checkpoint->searched()) {
        completion = checkpoint->checkSectorAndDistanceRatio(pos, distanceRatio);
        checkpoint->setSearched();
    }

    // If player is inside current checkpoint, stop searching
    if (completion == MapdataCheckPoint::SectorOccupancy::InsideSector) {
        return checkpoint->id();
    }

    // Search type 0: Search forwards first, then backwards
    if (!searchBackwardsFirst) {
        // If "player is forwards" but completion < 0, force completion to 0 and return
        // current checkpoint (GHOST CHECKPOINT!)
        if (playerIsForwards && completion == MapdataCheckPoint::SectorOccupancy::BetweenSides &&
                distanceRatio < 0.0f) {
            distanceRatio = 0.0f;
            return checkpoint->id();
        }

        // Stop if current checkpoint is a KCP
        if (checkpoint->checkArea() >= 0) {
            return -1;
        }

        // If player is between the sides of the quad but NOT between this checkpoint and next, AND
        // completion > 0, then "player is forwards"
        bool forward = completion == MapdataCheckPoint::SectorOccupancy::BetweenSides &&
                distanceRatio > 0.0f;

        // Search forwards, including checkpoints already searched
        s16 id = searchNextCheckpoint(pos, depth, checkpoint, distanceRatio, forward, false);

        // If that fails, search backwards, excluding checkpoints already searched
        return id == -1 ?
                searchPrevCheckpoint(pos, depth, checkpoint, distanceRatio, forward, true) :
                id;
    }

    // Search type 1: Search backwards first, then forwards

    // If "player is backwards" flag is true but completion > 1, force completion to 1 and return
    // current checkpoint (GHOST CHECKPOINT!)
    if (playerIsForwards && completion == MapdataCheckPoint::SectorOccupancy::BetweenSides &&
            distanceRatio > 1.0f) {
        distanceRatio = 1.0f;
        return checkpoint->id();
    }

    // Stop if current checkpoint is a KCP (skipped for online players, but they aren't supported)
    if (checkpoint->checkArea() >= 0) {
        return -1;
    }

    // If player is between the sides of the quad but NOT between this checkpoint and next, AND
    // completion < 0, then set "player is backwards" flag
    bool forward =
            completion == MapdataCheckPoint::SectorOccupancy::BetweenSides && distanceRatio < 0.0f;

    // Search backwards, including checkpoints already searched
    s16 id = searchPrevCheckpoint(pos, depth, checkpoint, distanceRatio, forward, false);

    // If that fails, search forwards, excluding checkpoints already searched
    return id == -1 ? searchNextCheckpoint(pos, depth, checkpoint, distanceRatio, forward, true) :
                      id;
}

/// @addr{0x80511E7C}
u16 CourseMap::getCheckPointEntryOffsetMs(u16 i, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos) const {
    EGG::Vector2f prevPos_ = EGG::Vector2f(prevPos.x, prevPos.z);
    EGG::Vector2f pos_ = EGG::Vector2f(pos.x, pos.z);

    MapdataCheckPoint *checkPoint = getCheckPoint(i);
    ASSERT(checkPoint);
    return checkPoint->getEntryOffsetMs(prevPos_, pos_);
}

f32 CourseMap::getCheckPointEntryOffsetExact(u16 i, const EGG::Vector3f &pos,
        const EGG::Vector3f &prevPos) const {
    EGG::Vector2f prevPos_ = EGG::Vector2f(prevPos.x, prevPos.z);
    EGG::Vector2f pos_ = EGG::Vector2f(pos.x, pos.z);

    MapdataCheckPoint *checkPoint = getCheckPoint(i);
    ASSERT(checkPoint);
    return checkPoint->getEntryOffsetExact(prevPos_, pos_);
}

/// @addr{0x80518AE0}
MapdataCannonPoint *CourseMap::getCannonPoint(u16 i) const {
    return m_cannonPoint && m_cannonPoint->size() != 0 ? m_cannonPoint->get(i) : nullptr;
}

/// @addr{0x80515C70}
MapdataCheckPath *CourseMap::getCheckPath(u16 i) const {
    return m_checkPath && m_checkPath->size() != 0 ? m_checkPath->get(i) : nullptr;
}

/// @addr{0x80515C24}
MapdataCheckPoint *CourseMap::getCheckPoint(u16 i) const {
    return m_checkPoint && m_checkPoint->size() != 0 ? m_checkPoint->get(i) : nullptr;
}

/// @addr{0x80514148}
MapdataGeoObj *CourseMap::getGeoObj(u16 i) const {
    return i < getGeoObjCount() ? m_geoObj->get(i) : nullptr;
}

/// @addr{0x80515E04}
MapdataPointInfo *CourseMap::getPointInfo(u16 i) const {
    return i < getPointInfoCount() ? m_pointInfo->get(i) : nullptr;
}

/// @addr{0x80518920}
MapdataJugemPoint *CourseMap::getJugemPoint(u16 i) const {
    return i < getJugemPointCount() ? m_jugemPoint->get(i) : nullptr;
}

/// @addr{0x80518B78}
MapdataStageInfo *CourseMap::getStageInfo() const {
    return m_stageInfo && m_stageInfo->size() != 0 ? m_stageInfo->get(0) : nullptr;
}

/// @addr{0x80514B30}
MapdataStartPoint *CourseMap::getStartPoint(u16 i) const {
    return m_startPoint && m_startPoint->size() != 0 ? m_startPoint->get(i) : nullptr;
}

u16 CourseMap::getCheckPathCount() const {
    return m_checkPath ? m_checkPath->size() : 0;
}

u16 CourseMap::getCheckPointCount() const {
    return m_checkPoint ? m_checkPoint->size() : 0;
}

u16 CourseMap::getGeoObjCount() const {
    return m_geoObj ? m_geoObj->size() : 0;
}

u16 CourseMap::getPointInfoCount() const {
    return m_pointInfo ? m_pointInfo->size() : 0;
}

u16 CourseMap::getJugemPointCount() const {
    return m_jugemPoint ? m_jugemPoint->size() : 0;
}

u32 CourseMap::version() const {
    return m_course->version();
}

MapdataCheckPathAccessor *CourseMap::checkPath() const {
    return m_checkPath;
}

MapdataCheckPointAccessor *CourseMap::checkPoint() const {
    return m_checkPoint;
}

f32 CourseMap::startTmpAngle() const {
    return m_startTmpAngle;
}

f32 CourseMap::startTmp0() const {
    return m_startTmp0;
}

f32 CourseMap::startTmp1() const {
    return m_startTmp1;
}

f32 CourseMap::startTmp2() const {
    return m_startTmp2;
}

f32 CourseMap::startTmp3() const {
    return m_startTmp3;
}

/// @addr{0x80512694}
CourseMap *CourseMap::CreateInstance() {
    ASSERT(!s_instance);
    s_instance = new CourseMap;
    return s_instance;
}

/// @addr{0x8051271C}
void CourseMap::DestroyInstance() {
    ASSERT(s_instance);
    auto *instance = s_instance;
    s_instance = nullptr;
    delete instance;
}

CourseMap *CourseMap::Instance() {
    return s_instance;
}

/// @addr{0x8051276C}
CourseMap::CourseMap()
    : m_course(nullptr), m_startPoint(nullptr), m_stageInfo(nullptr), m_startTmpAngle(0.0f),
      m_startTmp0(0.0f), m_startTmp1(0.0f), m_startTmp2(0.0f), m_startTmp3(0.0f) {}

/// @addr{0x805127AC}
CourseMap::~CourseMap() {
    if (s_instance) {
        s_instance = nullptr;
        WARN("CourseMap instance not explicitly handled!");
    }

    delete m_course;
    delete m_startPoint;
    delete m_stageInfo;
}

s16 CourseMap::findSectorBetweenSides(const EGG::Vector3f &pos, MapdataCheckPoint *checkpoint,
        f32 &distanceRatio) {
    s16 id = -1;

    // Search order varies depending on whether player is closer to the next or previous checkpoint.
    if (distanceRatio > 0.5f) {
        // Step 1: Starting at current checkpoint, search forwards
        id = searchNextCheckpoint(pos, 0, checkpoint, distanceRatio, false, false);

        if (id != -1) {
            return id;
        }

        // Step 2: If step 1 fails, start at next checkpoint(s) and search backwards
        for (size_t i = 0; i < checkpoint->nextCount(); ++i) {
            MapdataCheckPoint *next = checkpoint->nextPoint(i);

            for (size_t j = 0; j < next->prevCount(); ++j) {
                MapdataCheckPoint *prev = next->prevPoint(j);

                if (prev == checkpoint) {
                    continue;
                }

                id = findRecursiveSector(pos, 1, true, prev, distanceRatio, false);
                if (id != -1) {
                    return id;
                }
            }
        }

        // Step 3: If step 2 fails, start at previous checkpoint(s) and search forwards
        for (size_t i = 0; i < checkpoint->prevCount(); ++i) {
            MapdataCheckPoint *prev = checkpoint->prevPoint(i);

            for (size_t j = 0; j < prev->nextCount(); ++j) {
                MapdataCheckPoint *next = prev->nextPoint(j);

                if (next == checkpoint) {
                    continue;
                }

                id = findRecursiveSector(pos, 1, false, next, distanceRatio, false);
                if (id != -1) {
                    return id;
                }
            }
        }

        // Step 4: If step 3 fails, start at current checkpoint and search backwards
        return searchPrevCheckpoint(pos, 0, checkpoint, distanceRatio, false, false);
    } else {
        // Step 1: Starting at current checkpoint, search backwards
        id = searchPrevCheckpoint(pos, 0, checkpoint, distanceRatio, false, false);

        if (id != -1) {
            return id;
        }

        // Step 2: If step 1 fails, start at prev checkpoint(s) and search forwards
        for (size_t i = 0; i < checkpoint->prevCount(); ++i) {
            MapdataCheckPoint *prev = checkpoint->prevPoint(i);

            for (size_t j = 0; j < prev->nextCount(); ++j) {
                MapdataCheckPoint *next = prev->nextPoint(j);

                if (next == checkpoint) {
                    continue;
                }

                id = findRecursiveSector(pos, 1, false, next, distanceRatio, false);

                if (id != -1) {
                    return id;
                }
            }
        }

        // Step 3: If step 2 fails, start at next checkpoint(s) and search backwards
        for (size_t i = 0; i < checkpoint->nextCount(); ++i) {
            MapdataCheckPoint *next = checkpoint->nextPoint(i);

            for (size_t j = 0; j < next->prevCount(); ++j) {
                MapdataCheckPoint *prev = next->prevPoint(j);

                if (prev == checkpoint) {
                    continue;
                }

                id = findRecursiveSector(pos, 1, true, prev, distanceRatio, false);

                if (id != -1) {
                    return id;
                }
            }
        }

        // Step 4: If step 3 fails, start at current checkpoint and search forwards
        return searchNextCheckpoint(pos, 0, checkpoint, distanceRatio, false, false);
    }

    return id;
}

s16 CourseMap::findSectorOutsideSector(const EGG::Vector3f &pos, MapdataCheckPoint *checkpoint,
        f32 &distanceRatio) {
    s16 id = -1;

    // Step 1: Starting at next checkpoint(s), search backwards
    for (size_t i = 0; i < checkpoint->nextCount(); ++i) {
        MapdataCheckPoint *next = checkpoint->nextPoint(i);

        for (size_t j = 0; j < next->prevCount(); ++j) {
            MapdataCheckPoint *prev = next->prevPoint(j);

            if (prev == checkpoint) {
                continue;
            }

            id = findRecursiveSector(pos, 1, true, prev, distanceRatio, false);

            if (id != -1) {
                return id;
            }
        }
    }

    // Step 2: If step 1 fails, start at prev checkpoint(s) and search forwards
    for (size_t i = 0; i < checkpoint->prevCount(); ++i) {
        MapdataCheckPoint *prev = checkpoint->prevPoint(i);

        for (size_t j = 0; j < prev->nextCount(); ++j) {
            MapdataCheckPoint *next = prev->nextPoint(j);

            if (next == checkpoint) {
                continue;
            }

            id = findRecursiveSector(pos, 1, false, next, distanceRatio, false);

            if (id != -1) {
                return id;
            }
        }
    }

    // Step 3: If step 2 fails, start at next checkpoint(s) and search forwards
    for (size_t i = 0; i < checkpoint->nextCount(); ++i) {
        id = findRecursiveSector(pos, 1, false, checkpoint->nextPoint(i), distanceRatio, false);

        if (id != -1) {
            return id;
        }
    }

    // Step 4: If step 3 fails, start at prev checkpoint(s) and search backwards
    for (size_t i = 0; i < checkpoint->prevCount(); ++i) {
        id = findRecursiveSector(pos, 1, true, checkpoint->prevPoint(i), distanceRatio, false);

        if (id != -1) {
            return id;
        }
    }

    return id;
}

// If local search fails, remove depth limit and search all "loaded" checkpoints
s16 CourseMap::findSectorRegional(const EGG::Vector3f &pos, MapdataCheckPoint *checkpoint,
        f32 &distanceRatio) {
    s16 id = -1;

    // Step 1: Search all next checkpoints until player or key checkpoint is found
    for (size_t i = 0; i < checkpoint->nextCount(); ++i) {
        id = findRecursiveSector(pos, -1, false, checkpoint->nextPoint(i), distanceRatio, false);

        if (id != -1) {
            return id;
        }
    }

    // Step 2: Search all previous checkpoints until player or key checkpoint is found
    for (size_t i = 0; i < checkpoint->prevCount(); ++i) {
        id = findRecursiveSector(pos, -1, true, checkpoint->prevPoint(i), distanceRatio, false);

        if (id != -1) {
            return id;
        }
    }

    return id;
}

/// @addr{0x80510F58}
s16 CourseMap::searchNextCheckpoint(const EGG::Vector3f &pos, s16 depth,
        const MapdataCheckPoint *checkpoint, f32 &completion, bool playerIsForwards,
        bool useCache) const {
    s16 id = -1;
    depth = depth >= 0 ? depth + 1 : -1;

    for (size_t i = 0; i < checkpoint->nextCount(); ++i) {
        MapdataCheckPoint *next = checkpoint->nextPoint(i);

        if (!useCache || !next->searched()) {
            id = findRecursiveSector(pos, depth, false, next, completion, playerIsForwards);

            if (id != -1) {
                return id;
            }
        }
    }

    return id;
}

/// @addr{0x80511034}
s16 CourseMap::searchPrevCheckpoint(const EGG::Vector3f &pos, s16 depth,
        const MapdataCheckPoint *checkpoint, f32 &completion, bool playerIsForwards,
        bool useCache) const {
    s16 id = -1;
    depth = depth >= 0 ? depth + 1 : -1;

    for (size_t i = 0; i < checkpoint->prevCount(); ++i) {
        MapdataCheckPoint *prev = checkpoint->prevPoint(i);

        if (!useCache || !prev->searched()) {
            id = findRecursiveSector(pos, depth, true, prev, completion, playerIsForwards);

            if (id != -1) {
                return id;
            }
        }
    }

    return id;
}

/// @addr{0x80511E00}
void CourseMap::clearSectorChecked() {
    for (size_t i = 0; i < m_checkPoint->size(); ++i) {
        getCheckPoint(i)->clearSearched();
    }
}

/// @addr{0x80512C10}
void *CourseMap::LoadFile(const char *filename) {
    return ResourceManager::Instance()->getFile(filename, nullptr, ArchiveId::Course);
}

CourseMap *CourseMap::s_instance = nullptr; ///< @addr{0x809BD6E8}

} // namespace System
