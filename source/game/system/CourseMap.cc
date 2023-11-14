#include "CourseMap.hh"

#include "game/system/map/MapdataFileAccessor.hh"
#include "game/system/map/MapdataStartPoint.hh"

#include "game/system/ResourceManager.hh"

namespace System {

void CourseMap::init() {
    void *buffer = LoadFile("course.kmp");
    m_course =
            new MapdataFileAccessor(reinterpret_cast<const MapdataFileAccessor::SData *>(buffer));

    constexpr u32 START_POINT_SIGNATURE = 0x4b545054;
    m_startPoint = parseStartPoint(START_POINT_SIGNATURE);
}

MapdataStartPointAccessor *CourseMap::parseStartPoint(u32 sectionName) {
    const MapSectionHeader *sectionPtr = m_course->findSection(sectionName);

    MapdataStartPointAccessor *accessor = nullptr;
    if (sectionPtr) {
        accessor = new MapdataStartPointAccessor(sectionPtr);
    }

    return accessor;
}

u32 CourseMap::version() const {
    return m_course->version();
}

CourseMap *CourseMap::CreateInstance() {
    assert(!s_instance);
    s_instance = new CourseMap;
    return s_instance;
}

void CourseMap::DestroyInstance() {
    assert(s_instance);
    delete s_instance;
    s_instance = nullptr;
}

CourseMap *CourseMap::Instance() {
    return s_instance;
}

CourseMap::CourseMap() : m_course(nullptr), m_startPoint(nullptr) {}

CourseMap::~CourseMap() = default;

void *CourseMap::LoadFile(const char *filename) {
    return ResourceManager::Instance()->getFile(filename, nullptr, 1);
}

CourseMap *CourseMap::s_instance = nullptr;

} // namespace System
