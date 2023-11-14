#pragma once

#include <Common.hh>

namespace System {

class MapdataFileAccessor;
class MapdataStartPointAccessor;

class CourseMap {
public:
    void init();
    MapdataStartPointAccessor *parseStartPoint(u32 sectionName);

    u32 version() const;

    static CourseMap *CreateInstance();
    static void DestroyInstance();
    static CourseMap *Instance();

private:
    CourseMap();
    ~CourseMap();

    MapdataFileAccessor *m_course;
    MapdataStartPointAccessor *m_startPoint;

    static void *LoadFile(const char *filename);

    static CourseMap *s_instance;
};

} // namespace System
