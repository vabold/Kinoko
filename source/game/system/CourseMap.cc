#include "CourseMap.hh"

#include "game/system/map/MapdataFileAccessor.hh"
#include "game/system/map/MapdataGeoObj.hh"
#include "game/system/map/MapdataStageInfo.hh"
#include "game/system/map/MapdataStartPoint.hh"

#include "game/system/ResourceManager.hh"

namespace System {

void CourseMap::init() {
    void *buffer = LoadFile("course.kmp");
    m_course =
            new MapdataFileAccessor(reinterpret_cast<const MapdataFileAccessor::SData *>(buffer));

    constexpr u32 START_POINT_SIGNATURE = 0x4b545054;
    constexpr u32 GEO_OBJ_SIGNATURE = 0x474f424a;
    constexpr u32 STAGE_INFO_SIGNATURE = 0x53544749;

    m_startPoint = parseStartPoint(START_POINT_SIGNATURE);
    m_geoObj = parseGeoObjs(GEO_OBJ_SIGNATURE);
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

MapdataStageInfoAccessor *CourseMap::parseStageInfo(u32 sectionName) {
    const MapSectionHeader *sectionPtr = m_course->findSection(sectionName);

    MapdataStageInfoAccessor *accessor = nullptr;
    if (sectionPtr) {
        accessor = new MapdataStageInfoAccessor(sectionPtr);
    }

    return accessor;
}

MapdataStartPointAccessor *CourseMap::parseStartPoint(u32 sectionName) {
    const MapSectionHeader *sectionPtr = m_course->findSection(sectionName);

    MapdataStartPointAccessor *accessor = nullptr;
    if (sectionPtr) {
        accessor = new MapdataStartPointAccessor(sectionPtr);
    }

    return accessor;
}

MapdataGeoObjAccessor *CourseMap::parseGeoObjs(u32 sectionName) {
    const MapSectionHeader *sectionPtr = m_course->findSection(sectionName);

    MapdataGeoObjAccessor *accessor = nullptr;
    if (sectionPtr) {
        accessor = new MapdataGeoObjAccessor(sectionPtr);
    }

    return accessor;
}

MapdataStageInfo *CourseMap::getStageInfo() const {
    return m_stageInfo && m_stageInfo->size() != 0 ? m_stageInfo->get(0) : nullptr;
}

MapdataStartPoint *CourseMap::getStartPoint(u16 i) const {
    return m_startPoint && m_startPoint->size() != 0 ? m_startPoint->get(i) : nullptr;
}

MapdataGeoObj *CourseMap::getGeoObj(u16 i) const {
    return m_geoObj && m_geoObj->size() != 0 ? m_geoObj->get(i) : nullptr;
}

u32 CourseMap::version() const {
    return m_course->version();
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

MapdataGeoObjAccessor *CourseMap::geoObj() const {
    return m_geoObj;
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

CourseMap::CourseMap()
    : m_course(nullptr), m_startPoint(nullptr), m_geoObj(nullptr), m_stageInfo(nullptr), m_startTmpAngle(0.0f),
      m_startTmp0(0.0f), m_startTmp1(0.0f), m_startTmp2(0.0f), m_startTmp3(0.0f) {}

CourseMap::~CourseMap() {
    delete m_course;
    delete m_startPoint;
    delete m_geoObj;
    delete m_stageInfo;
}

void *CourseMap::LoadFile(const char *filename) {
    return ResourceManager::Instance()->getFile(filename, nullptr, ArchiveId::Course);
}

CourseMap *CourseMap::s_instance = nullptr;

} // namespace System
