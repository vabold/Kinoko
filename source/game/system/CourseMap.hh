#pragma once

#include <Common.hh>

/// @brief High-level handling for generic system operations, such as input reading, race
/// configuration, and resource management.
namespace System {

class MapdataCannonPoint;
class MapdataCannonPointAccessor;
class MapdataFileAccessor;
class MapdataGeoObj;
class MapdataGeoObjAccessor;
class MapdataStageInfo;
class MapdataStageInfoAccessor;
class MapdataStartPoint;
class MapdataStartPointAccessor;

/// @brief Contains course metadata, notably the starting position.
/// @addr{0x809BD6E8}
/// @nosubgrouping
class CourseMap : EGG::Disposer {
public:
    void init();
    [[nodiscard]] MapdataCannonPointAccessor *parseCannonPoint(u32 sectionName);
    [[nodiscard]] MapdataGeoObjAccessor *parseGeoObj(u32 sectionName);
    [[nodiscard]] MapdataStageInfoAccessor *parseStageInfo(u32 sectionName);
    [[nodiscard]] MapdataStartPointAccessor *parseStartPoint(u32 sectionName);

    /// @beginGetters
    [[nodiscard]] MapdataCannonPoint *getCannonPoint(u16 i) const;
    [[nodiscard]] MapdataGeoObj *getGeoObj(u16 i) const;
    [[nodiscard]] MapdataStageInfo *getStageInfo() const;
    [[nodiscard]] MapdataStartPoint *getStartPoint(u16 i) const;
    [[nodiscard]] u16 getGeoObjCount() const;
    [[nodiscard]] u32 version() const;
    [[nodiscard]] f32 startTmpAngle() const;
    [[nodiscard]] f32 startTmp0() const;
    [[nodiscard]] f32 startTmp1() const;
    [[nodiscard]] f32 startTmp2() const;
    [[nodiscard]] f32 startTmp3() const;
    /// @endGetters

    static CourseMap *CreateInstance();
    static void DestroyInstance();
    [[nodiscard]] static CourseMap *Instance();

private:
    CourseMap();
    ~CourseMap() override;

    MapdataFileAccessor *m_course;
    MapdataStartPointAccessor *m_startPoint;
    MapdataGeoObjAccessor *m_geoObj;
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
