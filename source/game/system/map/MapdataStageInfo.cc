#include "MapdataStageInfo.hh"

#include "game/system/CourseMap.hh"

namespace System {

MapdataStageInfo::MapdataStageInfo(const SData *data) : m_rawData(data) {
    EGG::RamStream stream = EGG::RamStream(data, sizeof(SData));
    read(stream);
}

void MapdataStageInfo::read(EGG::Stream & /*stream*/) {}

u8 MapdataStageInfo::polePosition() const {
    return m_rawData->polePosition;
}

u8 MapdataStageInfo::translationMode() const {
    return m_rawData->translationMode;
}

MapdataStageInfoAccessor::MapdataStageInfoAccessor(const MapSectionHeader *header)
    : MapdataAccessorBase<MapdataStageInfo, MapdataStageInfo::SData>(header) {
    init(reinterpret_cast<const MapdataStageInfo::SData *>(m_sectionHeader + 1),
            parse<u16>(m_sectionHeader->count));
}

MapdataStageInfoAccessor::~MapdataStageInfoAccessor() = default;

} // namespace System
