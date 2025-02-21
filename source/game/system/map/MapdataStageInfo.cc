#include "MapdataStageInfo.hh"

#include "game/system/CourseMap.hh"

namespace System {

MapdataStageInfo::MapdataStageInfo(const SData *data) : m_rawData(data) {
    EGG::RamStream stream = EGG::RamStream(data, sizeof(SData));
    read(stream);
}

void MapdataStageInfo::read(EGG::Stream & /*stream*/) {}

MapdataStageInfoAccessor::MapdataStageInfoAccessor(const MapSectionHeader *header)
    : MapdataAccessorBase<MapdataStageInfo, MapdataStageInfo::SData>(header) {
    init(reinterpret_cast<const MapdataStageInfo::SData *>(m_sectionHeader + 1),
            parse<u16>(m_sectionHeader->count));
}

MapdataStageInfoAccessor::~MapdataStageInfoAccessor() = default;

} // namespace System
