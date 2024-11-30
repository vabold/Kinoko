#include "CourseMap.hh"

#include "game/system/map/MapdataCannonPoint.hh"
#include "game/system/map/MapdataCheckPath.hh"
#include "game/system/map/MapdataCheckPoint.hh"
#include "game/system/map/MapdataFileAccessor.hh"
#include "game/system/map/MapdataGeoObj.hh"
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
    constexpr u32 START_POINT_SIGNATURE = 0x4b545054;
    constexpr u32 STAGE_INFO_SIGNATURE = 0x53544749;

    m_startPoint = parseStartPoint(START_POINT_SIGNATURE);
    m_checkPath = parseCheckPath(CHECK_PATH_SIGNATURE);
    m_checkPoint = parseCheckPoint(CHECK_POINT_SIGNATURE);
    m_geoObj = parseGeoObj(GEO_OBJ_SIGNATURE);
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

/// @addr{0x80518B78}
MapdataStageInfo *CourseMap::getStageInfo() const {
    return m_stageInfo && m_stageInfo->size() != 0 ? m_stageInfo->get(0) : nullptr;
}

/// @addr{0x80514B30}
MapdataStartPoint *CourseMap::getStartPoint(u16 i) const {
    return m_startPoint && m_startPoint->size() != 0 ? m_startPoint->get(i) : nullptr;
}

u16 CourseMap::getGeoObjCount() const {
    return m_geoObj ? m_geoObj->size() : 0;
}

u32 CourseMap::version() const {
    return m_course->version();
}

MapdataCheckPathAccessor *CourseMap::checkPath() const {
    return m_checkPath;
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

/// @addr{0x80511E00}
void CourseMap::clearSectorChecked() {
    for (size_t i = 0; i < m_checkPoint->size(); i++) {
        getCheckPoint(i)->clearSearched();
    }
}

/// @addr{0x80511500}
s16 CourseMap::findSector(const EGG::Vector3f &pos, u16 checkpointIdx, f32 *checkpointCompletion,
        bool isRemote) {
    clearSectorChecked();
    MapdataCheckPoint *checkpoint = getCheckPoint(checkpointIdx);
    s16 id = -1;
    // check if player is in starting checkpoint
    MapdataCheckPoint::SectorOccupancy completion =
            checkpoint->checkSectorAndCheckpointCompletion(pos, checkpointCompletion);
    // flag starting checkpoint as searched
    checkpoint->setSearched();
    u32 params = 0;
    if (isRemote) {
        params = 6;
    }

    // LOCAL SEARCH
    // Searches through checkpoints recursively with a depth limit of 6 (or 12 for online players).
    // Search stops if the player is found, the depth limit is reached, or a key checkpoint is
    // encountered.
    switch (completion) {
    // Case 1: the player is fully inside the current checkpoint, so just set to current checkpoint
    case MapdataCheckPoint::SectorOccupancy::InsideSector:
        id = checkpoint->id();
        break;

    // Case 2: the player is between the sides of the quad, but NOT between this checkpoint and
    // next; player is likely in the same checkpoint group
    case MapdataCheckPoint::SectorOccupancy::OutsideSector_BetweenSides:

        // Case 2a: the player is closer to the next checkpoint than the current checkpoint; player
        // is most likely to be in NEXT checkpoints
        if (*checkpointCompletion > 0.5f) {
            // Step 1: Starting at current checkpoint, search forwards
            for (size_t i = 0; i < checkpoint->nextCount(); i++) {
                MapdataCheckPoint *checkpoint_ =
                        i < checkpoint->nextCount() ? checkpoint->nextPoint(i) : nullptr;
                id = findRecursiveSector(pos, 1, false, *checkpoint_, checkpointCompletion, params);
                if (id != -1) {
                    break;
                }
            }
            // Step 2: If step 1 fails, start at next checkpoint(s) and search backwards
            if (id == -1) {
                for (size_t i = 0; i < checkpoint->nextCount(); i++) {
                    MapdataCheckPoint *next =
                            i < checkpoint->nextCount() ? checkpoint->nextPoint(i) : nullptr;
                    for (size_t j = 0; j < next->prevCount(); j++) {
                        MapdataCheckPoint *prev =
                                j < next->prevCount() ? next->prevPoint(j) : nullptr;
                        if (prev == checkpoint) {
                            continue;
                        }

                        id = findRecursiveSector(pos, 1, true, *prev, checkpointCompletion, params);
                        if (id != -1) {
                            break;
                        }
                    }
                }
                // Step 3: If step 2 fails, start at previous checkpoint(s) and search forwards
                if (id == -1) {
                    for (size_t i = 0; i < checkpoint->prevCount(); i++) {
                        MapdataCheckPoint *prev = (s32)i < checkpoint->prevCount() ?
                                checkpoint->prevPoint(i) :
                                nullptr;
                        for (size_t j = 0; j < prev->nextCount(); j++) {
                            MapdataCheckPoint *next =
                                    (s32)j < prev->nextCount() ? prev->nextPoint(j) : nullptr;
                            if (next == checkpoint) {
                                continue;
                            }

                            id = findRecursiveSector(pos, 1, false, *next, checkpointCompletion,
                                    params);
                            if (id != -1) {
                                break;
                            }
                        }
                    }
                }
            }
            // Step 4: If step 3 fails, start at current checkpoint and search backwards
            if (id == -1) {
                for (size_t i = 0; i < checkpoint->prevCount(); i++) {
                    MapdataCheckPoint *checkpoint_ =
                            i < checkpoint->prevCount() ? checkpoint->prevPoint(i) : nullptr;
                    id = findRecursiveSector(pos, 1, true, *checkpoint_, checkpointCompletion,
                            params);
                    if (id != -1) {
                        break;
                    }
                }
            }

            // Case 2b: the player is closer to the current checkpoint than the next checkpoint;
            // player is most likely to be in PREVIOUS checkpoints
        } else {
            // (do all the steps from 2a in reverse)
            // Search prev
            for (size_t i = 0; i < checkpoint->prevCount(); i++) {
                MapdataCheckPoint *checkpoint_ =
                        i < checkpoint->prevCount() ? checkpoint->prevPoint(i) : nullptr;
                id = findRecursiveSector(pos, 1, true, *checkpoint_, checkpointCompletion, params);
                if (id != -1) {
                    break;
                }
            }
            // If that fails, search prev -> next
            if (id == -1) {
                for (size_t i = 0; i < checkpoint->prevCount(); i++) {
                    MapdataCheckPoint *prev =
                            (s32)i < checkpoint->prevCount() ? checkpoint->prevPoint(i) : nullptr;
                    for (size_t j = 0; j < prev->nextCount(); j++) {
                        MapdataCheckPoint *next =
                                (s32)j < prev->nextCount() ? prev->nextPoint(j) : nullptr;
                        if (next == checkpoint) {
                            continue;
                        }

                        id = findRecursiveSector(pos, 1, false, *next, checkpointCompletion,
                                params);
                        if (id != -1) {
                            break;
                        }
                    }
                }

                // If that fails, search next -> prev
                if (id == -1) {
                    for (size_t i = 0; i < checkpoint->nextCount(); i++) {
                        MapdataCheckPoint *next = (s32)i < checkpoint->nextCount() ?
                                checkpoint->nextPoint(i) :
                                nullptr;
                        for (size_t j = 0; j < next->prevCount(); j++) {
                            MapdataCheckPoint *prev =
                                    (s32)j < next->prevCount() ? next->prevPoint(j) : nullptr;
                            if (prev == checkpoint) {
                                continue;
                            }

                            id = findRecursiveSector(pos, 1, true, *prev, checkpointCompletion,
                                    params);
                            if (id != -1) {
                                break;
                            }
                        }
                    }
                }
            }
            // If that fails, search next
            if (id == -1) {
                for (size_t i = 0; i < checkpoint->nextCount(); i++) {
                    MapdataCheckPoint *checkpoint_ =
                            i < checkpoint->nextCount() ? checkpoint->nextPoint(i) : nullptr;
                    id = findRecursiveSector(pos, 1, false, *checkpoint_, checkpointCompletion,
                            params);
                    if (id != -1) {
                        break;
                    }
                }
            }
        }
        break;

    // Case 3: the player is not between the sides of the quad (may still be between this checkpoint
    // and next); player is likely in a different checkpoint group
    case MapdataCheckPoint::SectorOccupancy::OutsideSector:
        // Search next -> prev
        for (size_t i = 0; i < checkpoint->nextCount(); i++) {
            MapdataCheckPoint *next =
                    (s32)i < checkpoint->nextCount() ? checkpoint->nextPoint(i) : nullptr;
            for (size_t j = 0; j < next->prevCount(); j++) {
                MapdataCheckPoint *prev = (s32)j < next->prevCount() ? next->prevPoint(j) : nullptr;
                if (prev == checkpoint) {
                    continue;
                }

                id = findRecursiveSector(pos, 1, true, *prev, checkpointCompletion, params);
                if (id != -1) {
                    break;
                }
            }
        }

        // If that fails, search prev -> next
        if (id == -1) {
            for (size_t i = 0; i < checkpoint->prevCount(); i++) {
                MapdataCheckPoint *prev =
                        i < checkpoint->prevCount() ? checkpoint->prevPoint(i) : nullptr;
                for (size_t j = 0; j < prev->nextCount(); j++) {
                    MapdataCheckPoint *next = j < prev->nextCount() ? prev->nextPoint(j) : nullptr;
                    if (next == checkpoint) {
                        continue;
                    }

                    id = findRecursiveSector(pos, 1, false, *next, checkpointCompletion, params);
                    if (id != -1) {
                        break;
                    }
                }
            }
        }

        // If that fails, search next
        if (id == -1) {
            for (size_t i = 0; i < checkpoint->nextCount(); i++) {
                MapdataCheckPoint *checkpoint_ =
                        i < checkpoint->nextCount() ? checkpoint->nextPoint(i) : nullptr;
                id = findRecursiveSector(pos, 1, false, *checkpoint_, checkpointCompletion, params);
                if (id != -1) {
                    break;
                }
            }
        }

        // If that fails, search prev
        if (id == -1) {
            for (size_t i = 0; i < checkpoint->prevCount(); i++) {
                MapdataCheckPoint *checkpoint_ =
                        i < checkpoint->prevCount() ? checkpoint->prevPoint(i) : nullptr;
                id = findRecursiveSector(pos, 1, true, *checkpoint_, checkpointCompletion, params);
                if (id != -1) {
                    break;
                }
            }
        }

    default:
        break;
    }

    // GLOBAL SEARCH
    // if local search fails, remove depth limit and search all "loaded" checkpoints

    // Step 1: Search all next checkpoints until player or key checkpoint is found
    if (id == -1) {
        for (size_t i = 0; i < checkpoint->nextCount(); i++) {
            MapdataCheckPoint *checkpoint_ =
                    i < checkpoint->nextCount() ? checkpoint->nextPoint(i) : nullptr;
            id = findRecursiveSector(pos, -1, false, *checkpoint_, checkpointCompletion, 0);
            if (id != -1) {
                break;
            }
        }
        // Step 2: Search all previous checkpoints until player or key checkpoint is found
        if (id == -1) {
            for (size_t i = 0; i < checkpoint->prevCount(); i++) {
                MapdataCheckPoint *checkpoint_ =
                        i < checkpoint->prevCount() ? checkpoint->prevPoint(i) : nullptr;
                id = findRecursiveSector(pos, -1, true, *checkpoint_, checkpointCompletion, 0);
                if (id != -1) {
                    break;
                }
            }
        }
    }

    // ONLINE PLAYERS ONLY
    // if both local and global searches fail, just iterate through ALL checkpoints (ignoring kcps)
    // until player is found or last checkpoint reached
    if (isRemote && id == -1) {
        for (size_t i = 0; i < m_checkPoint->size(); i++) {
            MapdataCheckPoint *checkpoint_ = getCheckPoint(i);
            if (!checkpoint_->searched()) { // search all unsearched checkpoints
                MapdataCheckPoint::SectorOccupancy completion =
                        checkpoint_->checkSectorAndCheckpointCompletion(pos, checkpointCompletion);
                checkpoint_->setSearched();
                if (completion == MapdataCheckPoint::SectorOccupancy::InsideSector) {
                    id = i;
                    break;
                }
            }
        }
    }

    return id;
}

s16 CourseMap::searchNextCheckpoint(const EGG::Vector3f &pos, s16 depth,
        const MapdataCheckPoint &checkpoint, float *completion, u32 params,
        const bool param_8) const {
    s16 id = -1;
    // increment depth counter unless it's -1
    s16 depth_ = depth >= 0 ? depth + 1 : -1;

    // iterate through each next checkpoint
    for (size_t i = 0; i < checkpoint.nextCount(); i++) {
        MapdataCheckPoint *checkpoint_ =
                (s32)i < checkpoint.nextCount() ? checkpoint.nextPoint(i) : nullptr;
        // if checkpoint hasn't been searched yet OR param_8 is false
        if (!param_8 || !checkpoint_->searched()) {
            // call recursive function to continue the search forwards
            id = findRecursiveSector(pos, depth_, false, *checkpoint_, completion, params);
            // stop if player's checkpoint has been found
            if (id != -1) {
                break;
            }
        }
    }
    return id;
}

s16 CourseMap::searchPrevCheckpoint(const EGG::Vector3f &pos, s16 depth,
        const MapdataCheckPoint &checkpoint, float *completion, u32 params,
        const bool param_8) const {
    s16 id = -1;
    // increment depth counter unless it's -1
    s16 depth_ = depth >= 0 ? depth + 1 : -1;

    // iterate through each previous checkpoint
    for (size_t i = 0; i < checkpoint.prevCount(); i++) {
        MapdataCheckPoint *checkpoint_ =
                (s32)i < checkpoint.prevCount() ? checkpoint.prevPoint(i) : nullptr;
        // if checkpoint hasn't been searched yet OR param_8 is false
        if (!param_8 || !checkpoint_->searched()) {
            // call recursive function to continue the search backwards
            id = findRecursiveSector(pos, depth_, true, *checkpoint_, completion, params);
            // stop if player's checkpoint has been found
            if (id != -1) {
                break;
            }
        }
    }
    return id;
}

// TODO: param_5 is "eSearchType"
/// @addr{0x80511110}
s16 CourseMap::findRecursiveSector(const EGG::Vector3f &pos, s16 depth, bool searchBackwardsFirst,
        MapdataCheckPoint &checkpoint, float *checkpointCompletion, u32 params) const {
    // set depth limit to 12 for online players, otherwise 6
    s16 maxDepth = params & 4 ? 12 : 6;
    // return immediately if max depth is reached
    if (depth >= 0 && depth > maxDepth) {
        return -1;
    }

    // if this checkpoint has been searched already, force set completion type to Completion_1
    // (why?)
    bool flagged = checkpoint.searched();
    MapdataCheckPoint::SectorOccupancy completion =
            MapdataCheckPoint::SectorOccupancy::OutsideSector;
    if (!flagged) {
        completion = checkpoint.checkSectorAndCheckpointCompletion(pos, checkpointCompletion);
    }
    checkpoint.setSearched();

    // if player is inside current checkpoint, return current checkpoint
    if (completion == MapdataCheckPoint::SectorOccupancy::InsideSector) {
        return checkpoint.id();
    }

    // Search type 0: Search forwards first, then backwards
    if (!searchBackwardsFirst) {
        // If "player is forwards" flag is true but completion < 0, force completion to 0 and return
        // current checkpoint (GHOST CHECKPOINT!)
        if (params & 1 &&
                completion == MapdataCheckPoint::SectorOccupancy::OutsideSector_BetweenSides &&
                *checkpointCompletion < 0.0f) {
            *checkpointCompletion = 0.0f;
            return checkpoint.id();
        }

        // Stop if current checkpoint is a KCP, unless this is an online player
        if (!(params & 2) && checkpoint.checkArea() >= 0) {
            return -1;
        }

        // If player is between the sides of the quad but NOT between this checkpoint and next, AND
        // completion > 0, then set "player is forwards" flag
        u32 params_;
        if (completion == MapdataCheckPoint::SectorOccupancy::OutsideSector_BetweenSides &&
                *checkpointCompletion > 0.0f) {
            params_ = params | 1;
        } else {
            params_ = params & ~1;
        }

        // search forwards INCLUDING checkpoints already searched
        s16 id = searchNextCheckpoint(pos, depth, checkpoint, checkpointCompletion, params_, false);
        // if that fails, search backwards EXCLUDING checkpoints already searched
        return id == -1 ?
                searchPrevCheckpoint(pos, depth, checkpoint, checkpointCompletion, params_, true) :
                id;
    }

    // Search type 1: Search backwards first, then forwards

    // If "player is backwards" flag is true but completion > 1, force completion to 1 and return
    // current checkpoint (GHOST CHECKPOINT!)
    if (params & 1 &&
            completion == MapdataCheckPoint::SectorOccupancy::OutsideSector_BetweenSides &&
            *checkpointCompletion > 1.0f) {
        *checkpointCompletion = 1.0f;
        return checkpoint.id();
    }

    // Stop if current checkpoint is a KCP, unless this is an online player
    if (!(params & 2) && checkpoint.checkArea() >= 0) {
        return -1;
    }

    // If player is between the sides of the quad but NOT between this checkpoint and next, AND
    // completion < 0, then set "player is backwards" flag
    u32 params_;
    if (completion == MapdataCheckPoint::SectorOccupancy::OutsideSector_BetweenSides &&
            *checkpointCompletion < 0.0f) {
        params_ = params | 1;
    } else {
        params_ = params & ~1;
    }

    // search backwards INCLUDING checkpoints already searched
    s16 id = searchPrevCheckpoint(pos, depth, checkpoint, checkpointCompletion, params_, false);
    // if that fails, search forwards EXCLUDING checkpoints already searched
    return id == -1 ?
            searchNextCheckpoint(pos, depth, checkpoint, checkpointCompletion, params_, true) :
            id;
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

/// @addr{0x80512C10}
void *CourseMap::LoadFile(const char *filename) {
    return ResourceManager::Instance()->getFile(filename, nullptr, ArchiveId::Course);
}

CourseMap *CourseMap::s_instance = nullptr; ///< @addr{0x809BD6E8}

} // namespace System
