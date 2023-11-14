#include "MapdataStartPoint.hh"

#include "game/system/CourseMap.hh"

namespace System {

MapdataStartPoint::MapdataStartPoint(const SData *data) : m_rawData(data) {
    u8 *unsafeData = reinterpret_cast<u8 *>(const_cast<SData *>(data));
    EGG::RamStream stream = EGG::RamStream(unsafeData, sizeof(SData));
    read(stream);
}

void MapdataStartPoint::read(EGG::Stream &stream) {
    m_position.read(stream);
    m_rotation.read(stream);
    if (CourseMap::Instance()->version() > 1830) {
        m_playerIndex = stream.read_s16();
    } else {
        m_playerIndex = 0;
    }
}

MapdataStartPointAccessor::MapdataStartPointAccessor(const MapSectionHeader *header)
    : MapdataAccessorBase<MapdataStartPoint, MapdataStartPoint::SData>(header) {
    if (CourseMap::Instance()->version() > 1830) {
        init(reinterpret_cast<const MapdataStartPoint::SData *>(m_sectionHeader + 1),
                m_sectionHeader->count);
    } else {
        init(reinterpret_cast<const MapdataStartPoint::SData *>(
                     reinterpret_cast<const u8 *>(m_sectionHeader + 4)),
                1);
    }
}

} // namespace System
