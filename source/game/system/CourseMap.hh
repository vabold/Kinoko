#pragma once

#include <Common.hh>

namespace System {

class MapdataFileAccessor;
class MapdataStageInfo;
class MapdataStageInfoAccessor;
class MapdataStartPoint;
class MapdataStartPointAccessor;

class CourseMap {
public:
    void init();
    MapdataStageInfoAccessor *parseStageInfo(u32 sectionName);
    MapdataStartPointAccessor *parseStartPoint(u32 sectionName);

    MapdataStageInfo *getStageInfo() const;
    MapdataStartPoint *getStartPoint(u16 i) const;
    u32 version() const;
    f32 startTmpAngle() const;
    f32 startTmp0() const;
    f32 startTmp1() const;
    f32 startTmp2() const;
    f32 startTmp3() const;

    static CourseMap *CreateInstance();
    static void DestroyInstance();
    static CourseMap *Instance();

private:
    CourseMap();
    ~CourseMap();

    MapdataFileAccessor *m_course;
    MapdataStartPointAccessor *m_startPoint;
    MapdataStageInfoAccessor *m_stageInfo;

    // TODO: Better names
    f32 m_startTmpAngle;
    f32 m_startTmp0;
    f32 m_startTmp1;
    f32 m_startTmp2;
    f32 m_startTmp3;

    static void *LoadFile(const char *filename);

    static CourseMap *s_instance;
};

} // namespace System
