#include "CourseMap.hh"

#include "game/system/map/MapdataCannonPoint.hh"
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
    constexpr u32 GEO_OBJ_SIGNATURE = 0x474f424a;
    constexpr u32 START_POINT_SIGNATURE = 0x4b545054;
    constexpr u32 STAGE_INFO_SIGNATURE = 0x53544749;

    m_startPoint = parseStartPoint(START_POINT_SIGNATURE);
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
MapdataCannonPointAccessor *CourseMap::parseCannonPoint(u32 sectionName) {
    const MapSectionHeader *sectionPtr = m_course->findSection(sectionName);

    MapdataCannonPointAccessor *accessor = nullptr;
    if (sectionPtr) {
        accessor = new MapdataCannonPointAccessor(sectionPtr);
    }

    return accessor;
}

/// @addr{0x805134C8}
MapdataGeoObjAccessor *CourseMap::parseGeoObj(u32 sectionName) {
    const MapSectionHeader *sectionPtr = m_course->findSection(sectionName);

    MapdataGeoObjAccessor *accessor = nullptr;
    if (sectionPtr) {
        accessor = new MapdataGeoObjAccessor(sectionPtr);
    }

    return accessor;
}

/// @addr{0x80512D64}
MapdataStageInfoAccessor *CourseMap::parseStageInfo(u32 sectionName) {
    const MapSectionHeader *sectionPtr = m_course->findSection(sectionName);

    MapdataStageInfoAccessor *accessor = nullptr;
    if (sectionPtr) {
        accessor = new MapdataStageInfoAccessor(sectionPtr);
    }

    return accessor;
}

/// @addr{0x80513F5C}
MapdataStartPointAccessor *CourseMap::parseStartPoint(u32 sectionName) {
    const MapSectionHeader *sectionPtr = m_course->findSection(sectionName);

    MapdataStartPointAccessor *accessor = nullptr;
    if (sectionPtr) {
        accessor = new MapdataStartPointAccessor(sectionPtr);
    }

    return accessor;
}

/// @addr{0x80518AF8}
MapdataCannonPoint *CourseMap::getCannonPoint(u16 i) const {
    return m_cannonPoint && m_cannonPoint->size() != 0 ? m_cannonPoint->get(i) : nullptr;
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
    assert(!s_instance);
    s_instance = new CourseMap;
    return s_instance;
}

/// @addr{0x8051271C}
void CourseMap::DestroyInstance() {
    assert(s_instance);
    delete s_instance;
    s_instance = nullptr;
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
