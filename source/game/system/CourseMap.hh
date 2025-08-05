#pragma once

#include "game/system/map/MapdataAccessorBase.hh"
#include "game/system/map/MapdataCannonPoint.hh"
#include "game/system/map/MapdataCheckPath.hh"
#include "game/system/map/MapdataCheckPoint.hh"
#include "game/system/map/MapdataFileAccessor.hh"
#include "game/system/map/MapdataGeoObj.hh"
#include "game/system/map/MapdataJugemPoint.hh"
#include "game/system/map/MapdataPointInfo.hh"
#include "game/system/map/MapdataStageInfo.hh"
#include "game/system/map/MapdataStartPoint.hh"

#include <egg/math/Vector.hh>

class SavestateManager;

/// @brief High-level handling for generic system operations, such as input reading, race
/// configuration, and resource management.
namespace System {

template <typename T>
concept MapdataDerived = is_derived_from_template_v<MapdataAccessorBase, T>;

/// @brief Contains course metadata, notably the starting position.
/// @addr{0x809BD6E8}
/// @nosubgrouping
class CourseMap : EGG::Disposer {
    friend class ::SavestateManager;

public:
    void init();

    template <MapdataDerived T>
    [[nodiscard]] T *parseMapdata(u32 sectionName) const {
        const MapSectionHeader *sectionPtr = m_course->findSection(sectionName);
        return sectionPtr ? new T(sectionPtr) : nullptr;
    }

    [[nodiscard]] s16 findSector(const EGG::Vector3f &pos, u16 checkpointIdx, f32 &distanceRatio);
    [[nodiscard]] s16 findRecursiveSector(const EGG::Vector3f &pos, s16 depth,
            bool searchBackwardsFirst, MapdataCheckPoint *checkpoint, f32 &completion,
            bool playerIsForwards) const;

    /// @beginGetters
    [[nodiscard]] u16 getCheckPointEntryOffsetMs(u16 i, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos) const;
    [[nodiscard]] f32 getCheckPointEntryOffsetExact(u16 i, const EGG::Vector3f &pos,
            const EGG::Vector3f &prevPos) const;

    /// @addr{0x80518AE0}
    [[nodiscard]] MapdataCannonPoint *getCannonPoint(u16 i) const {
        return i < getCannonPointCount() ? m_cannonPoint->get(i) : nullptr;
    }

    /// @addr{0x80515C70}
    [[nodiscard]] MapdataCheckPath *getCheckPath(u16 i) const {
        return i < getCheckPathCount() ? m_checkPath->get(i) : nullptr;
    }

    /// @addr{0x80515C24}
    [[nodiscard]] MapdataCheckPoint *getCheckPoint(u16 i) const {
        return i < getCheckPointCount() ? m_checkPoint->get(i) : nullptr;
    }

    /// @addr{0x80514148}
    [[nodiscard]] MapdataGeoObj *getGeoObj(u16 i) const {
        return i < getGeoObjCount() ? m_geoObj->get(i) : nullptr;
    }

    /// @addr{0x80515E04}
    [[nodiscard]] MapdataPointInfo *getPointInfo(u16 i) const {
        return i < getPointInfoCount() ? m_pointInfo->get(i) : nullptr;
    }

    /// @addr{0x80518920}
    [[nodiscard]] MapdataJugemPoint *getJugemPoint(u16 i) const {
        return i < getJugemPointCount() ? m_jugemPoint->get(i) : nullptr;
    }

    /// @addr{0x80518B78}
    [[nodiscard]] MapdataStageInfo *getStageInfo() const {
        return getStageInfoCount() != 0 ? m_stageInfo->get(0) : nullptr;
    }

    /// @addr{0x80514B30}
    [[nodiscard]] MapdataStartPoint *getStartPoint(u16 i) const {
        return i < getStartPointCount() ? m_startPoint->get(i) : nullptr;
    }

    [[nodiscard]] u16 getCannonPointCount() const {
        return m_cannonPoint ? m_cannonPoint->size() : 0;
    }

    [[nodiscard]] u16 getCheckPathCount() const {
        return m_checkPath ? m_checkPath->size() : 0;
    }

    [[nodiscard]] u16 getCheckPointCount() const {
        return m_checkPoint ? m_checkPoint->size() : 0;
    }

    [[nodiscard]] u16 getGeoObjCount() const {
        return m_geoObj ? m_geoObj->size() : 0;
    }

    [[nodiscard]] u16 getPointInfoCount() const {
        return m_pointInfo ? m_pointInfo->size() : 0;
    }

    [[nodiscard]] u16 getJugemPointCount() const {
        return m_jugemPoint ? m_jugemPoint->size() : 0;
    }

    [[nodiscard]] u16 getStageInfoCount() const {
        return m_stageInfo ? m_stageInfo->size() : 0;
    }

    [[nodiscard]] u16 getStartPointCount() const {
        return m_startPoint ? m_startPoint->size() : 0;
    }

    [[nodiscard]] u32 version() const {
        return m_course->version();
    }

    [[nodiscard]] MapdataCheckPathAccessor *checkPath() const {
        return m_checkPath;
    }

    [[nodiscard]] MapdataCheckPointAccessor *checkPoint() const {
        return m_checkPoint;
    }

    [[nodiscard]] f32 startTmpAngle() const {
        return m_startTmpAngle;
    }

    [[nodiscard]] f32 startTmp0() const {
        return m_startTmp0;
    }

    [[nodiscard]] f32 startTmp1() const {
        return m_startTmp1;
    }

    [[nodiscard]] f32 startTmp2() const {
        return m_startTmp2;
    }

    [[nodiscard]] f32 startTmp3() const {
        return m_startTmp3;
    }
    /// @endGetters

    static CourseMap *CreateInstance();
    static void DestroyInstance();

    [[nodiscard]] static CourseMap *Instance() {
        return s_instance;
    }

private:
    CourseMap();
    ~CourseMap() override;

    [[nodiscard]] s16 findSectorBetweenSides(const EGG::Vector3f &pos,
            MapdataCheckPoint *checkpoint, f32 &distanceRatio);
    [[nodiscard]] s16 findSectorOutsideSector(const EGG::Vector3f &pos,
            MapdataCheckPoint *checkpoint, f32 &distanceRatio);
    [[nodiscard]] s16 findSectorRegional(const EGG::Vector3f &pos, MapdataCheckPoint *checkpoint,
            f32 &distanceRatio);
    [[nodiscard]] s16 searchNextCheckpoint(const EGG::Vector3f &pos, s16 depth,
            const MapdataCheckPoint *checkpoint, f32 &completion, bool playerIsForwards,
            bool useCache) const;
    [[nodiscard]] s16 searchPrevCheckpoint(const EGG::Vector3f &pos, s16 depth,
            const MapdataCheckPoint *checkpoint, f32 &completion, bool playerIsForwards,
            bool useCache) const;
    void clearSectorChecked();

    MapdataFileAccessor *m_course;
    MapdataStartPointAccessor *m_startPoint;
    MapdataCheckPathAccessor *m_checkPath;
    MapdataCheckPointAccessor *m_checkPoint;
    MapdataPointInfoAccessor *m_pointInfo;
    MapdataGeoObjAccessor *m_geoObj;
    MapdataJugemPointAccessor *m_jugemPoint;
    MapdataCannonPointAccessor *m_cannonPoint;
    MapdataStageInfoAccessor *m_stageInfo;

    // TODO: Better names
    f32 m_startTmpAngle;
    f32 m_startTmp0;
    f32 m_startTmp1;
    f32 m_startTmp2;
    f32 m_startTmp3;

    static void *LoadFile(const char *filename); ///< @addr{0x809BD6E8}

    static CourseMap *s_instance;
};

} // namespace System
